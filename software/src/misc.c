#include "../include/misc.h"
#include "pico/stdlib.h"

void output_setup()
{
    gpio_set_function(29, GPIO_FUNC_NULL);
    gpio_init(29);
    gpio_set_dir(29, true);
    gpio_put(29, 1);
    gpio_init(GATE1);
    gpio_set_dir(GATE1, true);
    gpio_put(GATE1, GATE_OFF);
    gpio_init(GATE2);
    gpio_set_dir(GATE2, true);
    gpio_put(GATE2, GATE_OFF);
    gpio_init(GATE3);
    gpio_set_dir(GATE3, true);
    gpio_put(GATE3, GATE_OFF);
}

void flash_led(uint8_t times,uint32_t duration_ms){
    uint8_t time = (duration_ms / times)/2;
    for (int i = 0; i < times; i++)
    {
        gpio_put(29, 1);
        sleep_ms(time);
        gpio_put(29, 0);
        sleep_ms(time);
    }
}