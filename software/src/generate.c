#include "../include/generate.h"


void create_generator(generator_t *gen, uint8_t base_probability, uint8_t probability_scaling_factor, uint8_t delay){
    srand(time_us_64());
    gen->base_prob = base_probability;
    gen->changes_made = 0;
    gen->current_prob = base_probability;
    gen->scale = probability_scaling_factor;
    gen->cycles = 0;
    gen->max_changes = 1;
    gen->reset = -1;
    gen->reset_on_change = true;
    gen->range = 1;
    gen->direction = 0;
    gen->delay = delay;
    for(int i = 0; i<10;i++)
    gen->possible_val[i]=1;
};

uint16_t gen_random(uint8_t amount)
{
    srand(time_us_64());
    return rand() % amount;
}

uint8_t *changed_notes;
uint16_t prev_total;

void gen_sequence_setup(generator_t *gen,sequence_t * seq){
    changed_notes = (uint8_t *)malloc(seq->total_notes);
    for(uint16_t i = 0; i<seq->total_notes;i++)
        changed_notes[i]=0;
    prev_total = seq->total_notes;
}

void gen_sequence_update(generator_t *gen,sequence_t * seq){
    if(prev_total == seq->total_notes) return;
        changed_notes = (uint8_t *)realloc(changed_notes,seq->total_notes);
    for(uint16_t i = 0; i<seq->total_notes;i++)
        changed_notes[i]=0;
    prev_total = seq->total_notes;
}

bool gen_sequence_run(generator_t *gen,sequence_t * seq){
    if(gen->cycles < gen->delay) {
        gen->cycles ++;
        return false;
    }
    if(gen->direction==0 || gen->cycles == 0){
        if(gen_random(2))
            gen->direction = 1;
        else 
            gen->direction = -1;
    }
     for(int16_t i = 0;i<seq->last_step || gen->max_changes >= gen->changes_made; i++){
        if(seq->note_value[seq->current_page][i].type==REGULAR_NOTE ){
            note temp = seq->note_value[seq->current_page][i];
            if(gen_random(100)<gen->current_prob){
                int8_t val = temp.value +  (1 * gen->direction);
                 if(val<0){ 
                    val += 7;
                    if(temp.octave > 0)
                        temp.octave --;
                }
                else if(val>6){ 
                    val -=6;
                    if(temp.octave<5)
                        temp.octave ++;
                }
                int8_t count = 0;
                while(!gen->possible_val[val+count]){
                    if(val + count > 6) count = -val;
                    else count++;
                }
                temp.value = val+count;
                gen->changes_made ++;
                gen->changed = true;
                seq->note_value[seq->current_page][i] = temp;
            }
        }
    } 
        gen->cycles ++;
        if(gen->cycles == gen->reset || gen->reset_on_change && gen->changed){
            gen->cycles = 0;
            gen->current_prob = gen->base_prob;
        }
        else gen->current_prob += gen->scale;
        if(gen->changed){
            gen->changed = false;
            gen->changes_made = 0;
            return true;
        }
        else return false;
}


void gen_dynamic(generator_t *gen)
{
}

