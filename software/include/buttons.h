#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "button_definition.h"

typedef struct
{
    bool control_buttons[6];
    bool control_buttons_flag[6];
    bool keyboard_prev_buttons[13];
    bool keyboard_buttons_flag[13];
    queue_t controlqueue;
    queue_t keyboardqueue;
} button_control_t;

void button_init(button_control_t *self);

int16_t matrix_poll();

bool button_poll(uint8_t button);

void read_control_buttons(button_control_t *buttons);

void read_keyboard(button_control_t *buttons);

void read_control_released(button_control_t *buttons);

void read_keyboard_released(button_control_t *buttons);

uint8_t unison_high_note_priority(button_control_t *buttons);