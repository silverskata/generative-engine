#include "hardware/timer.h"
#include "include/controller.h"

// FUNCTION DECLARATIONS
void input_core();
bool repeating_timer_callback(struct repeating_timer *t);
int64_t UI_refresh_callback(alarm_id_t id, void *user_data);

// SHARED RESOURCES
controller_t controller = {
    .menu_state = 0,
    .selection_state = 0,
};
sequencer_t sequencer;
sequence_t s1;
sequence_t s2;

int main()
{
    /* DEBUG */
/*     

    struct repeating_timer timer;
    stdio_init_all();
    setup_ui();
    output_setup();
    init_sequencer(&sequencer, &s1, &s2, &timer);
    uint8_t numbers[] = {1,1,1,1,1,1,1};
 */


    /* DEBUG */

    //SETUP
    set_sys_clock_khz(100 * 1000, true);
    struct repeating_timer timer;
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

    multicore_launch_core1(input_core);
    flash_led(6,500);
    uint64_t time = time_us_64();
    
    add_repeating_timer_us(-62500, repeating_timer_callback, NULL, &timer);



/* 
int button_press=0;
int keyboard_press = -1;
int encoder_direction = 0; */

    //MAIN LOOP
    while (1)
    {

/* 
     if (!queue_is_empty(&controller.buttons->controlqueue))
    {
        button_press = read_control_queue(&controller);
        if (!update_shift(&controller, button_press));
        UI_Test(keyboard_press, button_press, encoder_direction,&controller);
    }
    else
        button_press = 0;
            if(controller.keyboard_active == 3 && !queue_is_empty(&controller.buttons->keyboardqueue)){
                keyboard_press = read_keyboard_queue(&controller);
                        UI_Test(keyboard_press, button_press, encoder_direction,&controller);
            }
            else keyboard_press = -1;
    if (multicore_fifo_rvalid()){
        encoder_direction = multicore_fifo_pop_blocking();
        UI_Test(keyboard_press, button_press, encoder_direction,&controller);
    }
    else
        encoder_direction = 0;
      */

         //MAIN CONTROLLER TASK
        control_task(&controller);

        //UI AND SCREEN REFRESH
        if(time-time_us_64() > 30000){
        UI_main_menu(&sequencer, controller.menu_state, controller.menu_active, controller.selection_state, controller.note);
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
        if (sequencer.current_step == sequencer.end_of_sequence){
            sequencer.current_step = 0;
            if(controller.dynamic_generation)
                dynamic_generation(&controller);
        }
    }
    return true;
}
