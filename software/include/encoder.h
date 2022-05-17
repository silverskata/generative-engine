#include <stdio.h>
#include "pico/stdlib.h"

#define ENCA 17
#define ENCB 16

typedef struct {
int8_t previous_state;
int8_t state;
int16_t value;
uint8_t max_value;
int8_t change;
} encoder_t;

void init_encoder();

int16_t read_encoder(encoder_t *self);

void increment (encoder_t *self, int16_t amount);
