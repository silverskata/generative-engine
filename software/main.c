#include <stdio.h>
#include <stdint.h>
#include "hardware/timer.h"
#include "include/controller.h"




controller_t controller ={
    .menu_state = 0,
    .selection_state = 0,
};

void input_core();
bool repeating_timer_callback(struct repeating_timer *t);
int64_t UI_refresh_callback(alarm_id_t id, void * user_data);
sequencer_t sequencer;
sequence_t s1;
sequence_t s2;

int main()
{

    //SETUP
    set_sys_clock_khz(100 * 1000, true);
    struct repeating_timer timer;
    sleep_ms(500);
    stdio_init_all();
    sleep_ms(100);
    dac_init();
    sleep_ms(100);
    output_setup();
    sleep_ms(100);
    init_sequencer(&sequencer, &s1, &s2, &timer);
    sleep_ms(100);
    setup_ui();
    UI_startup();
    sequencer.playing = false;
    add_repeating_timer_us(-62500, repeating_timer_callback, NULL, &timer);
    multicore_launch_core1(input_core);

//CONTROLLER
    controller.keyboard_active = 3;
    uint16_t button_press = 0;
    int16_t  keyboard_press = -1;
    int8_t encoder_direction = 0;
    sequencer.current_scale = scales[IONIAN];
    flash_led(6,500);

    uint64_t time = time_us_64();
    while (1)
    {       

            
            if(!queue_is_empty(&controller.buttons->controlqueue)){
                button_press = read_control_queue(&controller);
                if(!update_shift(&controller, button_press));
            }
            else button_press = 0;
            if(controller.keyboard_active == 3 && !queue_is_empty(&controller.buttons->keyboardqueue)){
                keyboard_press = read_keyboard_queue(&controller);
                    if(is_input(keyboard_press))
                       keyboard_press = keyboard_press >> 1;
                    else keyboard_press = -1;
                       // free_play_released(keyboard_press);
            }
            else keyboard_press = -1;

        if(time-time_us_64() > 30000){
        UI_main_menu(&sequencer,controller.menu_state, controller.selection_state, controller.note);
        time = time_us_64();
        }

            if (multicore_fifo_rvalid())
              encoder_direction = multicore_fifo_pop_blocking();
            else encoder_direction =0;

            switch(controller.menu_state){
                case MAIN_PLAYING:
                    update_state_playing(&controller, button_press,keyboard_press, encoder_direction);
                break;
                case MENU_CLOSED:
                    update_state_main(&controller, button_press,keyboard_press, encoder_direction);
                break;
                case MENU_OPEN:

                break;
                case EDIT_MENU:
                    update_state_edit(&controller, button_press, keyboard_press, encoder_direction);
                break;
                case GENERATE_MENU:
                    update_state_generation(&controller, button_press, encoder_direction);

            } 
    }
}
bool repeating_timer_callback(struct repeating_timer *t)
{
    if (sequencer.playing)
    {
        play_step(&sequencer);
        sequencer.current_step++;
        if (sequencer.current_step == sequencer.end_of_sequence)
            sequencer.current_step = 0;
    }
    return true;
}

  bool encoder_in_use;
int64_t encoder_callback(alarm_id_t id, void *user_data){
    encoder_in_use =false;
    return 0;
}

void input_core()
{
    button_control_t buttons;
    encoder_t encoder;
    button_init(&buttons);
    init_encoder(&encoder);
    init_controller(&controller,&buttons,&sequencer);
    int16_t amount = 0;
    uint8_t uPC = 0;
    flash_led(2,400);
    encoder_in_use = false;
    while (1)
    {
        amount = read_encoder(&encoder);
        if (amount != 0){
            multicore_fifo_push_blocking(amount);
            if(!encoder_in_use) { 
                add_alarm_in_ms(50,encoder_callback,NULL,true);
                 encoder_in_use = true; //ENCODER takes prio over the keyboard
            }

        }
        else
        {
            switch(uPC){
                case 0:
                read_control_buttons(&buttons);
                break;
                case 1:
                read_control_released(&buttons);
                break;
                case 2:
                if(!encoder_in_use)
                read_keyboard(&buttons);
                break;
                default:
                if(!encoder_in_use)
                read_keyboard_released(&buttons);
            }
        }
        uPC ++;
        if(uPC>controller.keyboard_active) uPC = 0;
    }
};
