#include <stdio.h>
#include <stdint.h>
#include "hardware/timer.h"
#include "include/controller.h"

//FUNCTION DECLARATIONS
void input_core();
bool repeating_timer_callback(struct repeating_timer *t);
int64_t UI_refresh_callback(alarm_id_t id, void * user_data);

//SHARED RESOURCES
controller_t controller ={
    .menu_state = 0,
    .selection_state = 0,
};
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
    add_repeating_timer_us(-62500, repeating_timer_callback, NULL, &timer);
    multicore_launch_core1(input_core);
    flash_led(6,500);
    uint64_t time = time_us_64();

    //MAIN LOOP
    while (1)
    {       
        //MAIN CONTROLLER TASK
        control_task(&controller);

        //UI AND SCREEN REFRESH
        if(time-time_us_64() > 30000){
        UI_main_menu(&sequencer,controller.menu_state, controller.selection_state, controller.note);
        time = time_us_64();
        }

    }
}

 //SECOND CORE FUNCTION
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
    while (1)
    {
        amount = read_encoder(&encoder);
        if (amount != 0)
            multicore_fifo_push_blocking(amount);
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
                read_keyboard(&buttons);
                break;
                default:
                read_keyboard_released(&buttons);
            }
        }
        uPC ++;
        if(uPC>controller.keyboard_active) uPC = 0;
    }
};

//TIMER FUNCTIONS
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
