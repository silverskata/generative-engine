#include <stdio.h>
#include <stdlib.h>
#include "hardware/timer.h"
#include "sequencer.h"

#define GENERATOR_AMOUNT 5

typedef struct
{
    bool active;
    uint16_t base_prob;      //starting probability of generator
    uint16_t current_prob;   //where the probability is.
    uint16_t scale;          //the probability scalilng factor, by how much the probability will increase per cycle
    uint16_t changes_made;   //how many changes has occured
    uint16_t max_changes;    //maximum amount of changes/cycle;
    uint8_t delay;          //how many cycles should the generator wait after a change is made.
    int16_t reset;           //after how many cycles the probabilities are reset (-1) means never
    bool reset_on_change;   //set if the generator should reset probabilities after a change has occured.
    uint32_t cycles;        //how many cycles the generator has gone through since last delay
    bool changed;           //shorthand for if a changed ocurred in a given cycle;
    uint8_t range;          //How many steps at a time the generator can jump
    int8_t direction;       //if the generator will increase (1) or decrease(-1) or random (0)
    uint8_t possible_val[12]; //Array of possible values that the generator can set.
} generator_t;

void create_generator(generator_t *gen);

uint16_t gen_random(uint8_t amount);

void gen_sequence_setup(generator_t *gen, sequence_t * seq);

void gen_sequence_update(generator_t *gen,sequence_t * seq);

bool gen_sequence_run(generator_t *gen, sequence_t * seq);

bool gen_scale_run(generator_t *gen,sequencer_t * seq);

bool gen_key_run(generator_t *gen,sequencer_t * seq);

bool gen_harmony_run(generator_t *gen,sequence_t * seq);
