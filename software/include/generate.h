#include <stdio.h>
#include <stdlib.h>
#include "hardware/timer.h"
#include "sequencer.h"

typedef struct
{
    uint8_t base_prob;      //starting probability of generator, saved to be able to reset to base easier
    uint8_t current_prob;   //where the probability is.
    uint8_t scale;          //the probability scalilng factor, by how much the probability will increase per cycle
    uint8_t changes_made;   //how many changes has occured
    uint8_t max_changes;    //maximum amount of changes/cycle;
    uint8_t delay;          //how many cycles should the generator wait after a change is made.
    int8_t reset;           //after how many cycles the probabilities are reset (-1) means never
    bool reset_on_change;   //set if the generator should reset probabilities after a change has occured.
    uint32_t cycles;        //how many cycles the generator has gone through since last delay
    bool changed;           //shorthand for if a changed ocurred in a given cycle;
    uint8_t range;          //How many steps at a time the generator can jump
    int8_t direction;       //if the generator will increase (1) or decrease(-1) or random (0)
    uint8_t possible_val[10]; //Array of possible values that the generator can set.
} generator_t;

void create_generator(generator_t *gen, uint8_t base_probability, uint8_t probability_scaling_factor, uint8_t delay);

uint16_t gen_random(uint8_t amount);

void gen_sequence_setup(generator_t *gen, sequence_t * seq);

void gen_sequence_update(generator_t *gen,sequence_t * seq);

bool gen_sequence_run(generator_t *gen, sequence_t * seq);