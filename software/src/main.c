#include <stdio.h>
#include <stdint.h>
#include "hardware/timer.h"
#include "../include/controller.h"




controller_t controller ={
    .menu_state = 0,
    .selection_state = 0,
};

/*  // DEBUG PURPOSES
ssd1306_t disp; 
void setup_ui()
{
    disp.external_vcc = false;
    display_setup(i2c0);
    display_init(&disp, 128, 64, 0x3C, i2c0);
}
  */


// TEST STUFF
char buf[30];
char buf2[30];
char buf3[30];
int16_t count = 0;
bool display_updated = false;
note temp;


bool UI_refresh = true;

void input_core();
bool repeating_timer_callback(struct repeating_timer *t);
int64_t UI_refresh_callback(alarm_id_t id, void * user_data);
sequencer_t sequencer;
sequence_t s1;
sequence_t s2;

int main()
{
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



//DEBUG

/*      ssd1306_t disp;
    disp.external_vcc=false;
    display_setup(i2c0);
    display_init(&disp, 128, 64, 0x3C, i2c0); 
    */

//DEBUG
    note test;
    test.type =REGULAR_NOTE;
    test.length = 4;
    test.legato = 3;
    test.octave = 1;

    sleep_ms(500);
    for(int j =0;j<16;j++){
    for (int i = 0; i < 8; i++)
    {
        if(i!=7){
        test.value = i;
        set_note(&sequencer.sequencers[0], sequencer.sequencers[0].selected_step, test); // 1 C
        select_step(&sequencer.sequencers[0], 1);
        }
        else{
        test.value = 0;
        test.octave ++;
        set_note(&sequencer.sequencers[0], sequencer.sequencers[0].selected_step, test); // 1 C
        select_step(&sequencer.sequencers[0], 1);
        test.octave --;
        }
    }
    }
    sequencer.sequencers[0].selected_step = 0;

   sequencer.active_sequence = 0;


    sequencer.sequencers[1].active = false;
//DEBUG
    char buff[30];
    char buff2[30];
    char buff3[30];
    char yes[] ={" pressed "};
    char no[] = {" pressed "};
    uint16_t a;
    uint16_t b;
    int c = 0;
    sleep_ms(200);
    flash_led(6,500);
     controller.keyboard_active = 3;
    uint16_t button_press = 0;
    int16_t  keyboard_press = -1;
    int8_t encoder_direction = 0;
    sequencer.current_scale = scales[IONIAN];
    uint64_t time = time_us_64();
//DEBUG
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
                
        
           //BUTTONS DEBUG
          //   ssd1306_clear(&disp);
      /*       if(!queue_is_empty(&buttons.keyboardqueue))
            queue_remove_blocking(&buttons.keyboardqueue, &a);
            if(!queue_is_empty(&buttons.controlqueue))
            queue_remove_blocking(&buttons.controlqueue, &b);
            if (multicore_fifo_rvalid())
              c+=  multicore_fifo_pop_blocking();
            sprintf(buff,"%x %x %d",a,b,c);
            
            
            display_string(&disp,0,10,1,buff);
            sprintf(buff2,"%d%d",buttons.keyboard_buttons[0],buttons.keyboard_buttons[1]);
            sprintf(buff3,"%d %d %d %d %d %d",buttons.control_buttons[0],buttons.control_buttons[1],buttons.control_buttons[2],buttons.control_buttons[3],buttons.control_buttons[4],buttons.control_buttons[5]);
            display_write_tight(&disp,0,20,2,buff2);
            display_string(&disp,0,30,1,buff3);
            if(!(a & 1))
                display_string(&disp,0,40,1,no);
            else
                display_write_tight(&disp,0,40,2,yes);
                ssd1306_show(&disp);   */
       // }
 


    }
}
bool repeating_timer_callback(struct repeating_timer *t)
{
    if (sequencer.playing)
    {
        play_step(&sequencer);
        set_current_step(&sequencer, sequencer.current_step + 1);
        if (sequencer.current_step >= MAX_NOTE_LENGTH * STEPSIZE)
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
