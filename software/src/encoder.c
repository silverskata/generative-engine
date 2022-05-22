
#include "../include/encoder.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t old_AB = 0;
int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

void init_encoder(encoder_t *self)
{
    gpio_set_dir(ENCA, false);
    gpio_set_dir(ENCB, false);
    gpio_pull_up(ENCA);
    gpio_pull_up(ENCB);
    self->previous_state = gpio_get(ENCA);
    self->value = 0;
    self->max_value = 16;
    self->change = 0;
};

int16_t read_encoder(encoder_t *self)
{
    self->change = 0;
    old_AB <<= 2;
    old_AB |= (gpio_get(ENCA) << 1) | gpio_get(ENCB);
    increment(self, enc_states[(old_AB & 0x0f)]);
    return self->change;
};

void increment(encoder_t *self, int16_t amount)
{
    if (amount == 0)
        return;
    self->value += amount;
    if (abs(self->value) < 4)
        return;
    self->change = self->value > 0 ? 1 : -1;
    self->value = 0;
}