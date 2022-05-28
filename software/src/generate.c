#include "../include/generate.h"


void create_generator(generator_t *gen){
    gen->active = false;
    gen->base_prob = 0;
    gen->changes_made = 0;
    gen->current_prob = 0;
    gen->scale = 0;
    gen->cycles = 0;
    gen->max_changes = 1;
    gen->reset = -1;
    gen->reset_on_change = true;
    gen->range = 1;
    gen->direction = 0;
    gen->delay = 0;
    for(int i = 0; i<12;i++)
        gen->possible_val[i]=1;
};

uint16_t gen_random(uint8_t amount)
{
    srand(time_us_64());
    return rand() % amount;
}


bool delayed(generator_t *gen){
    if(gen->cycles < gen->delay) {
        gen->cycles ++;
        return true;
    }
    return false;
}

void random_direction(generator_t *gen){
    if(gen->direction==0 || gen->cycles == 0){
        if(gen_random(2))
            gen->direction = 1;
        else 
            gen->direction = -1;
    }
}

bool gen_update(generator_t *gen){
    if(gen->cycles == gen->reset || gen->reset_on_change && gen->changed){
        gen->cycles = 0;
        gen->current_prob = gen->base_prob;
    }
    else {
        gen->current_prob += gen->scale;
        if(gen->current_prob>100)
        gen->current_prob = 100;
    }

    if(gen->changed){
        gen->changed = false;
        gen->changes_made = 0;

    }
    gen->cycles ++;
    return !gen->changed;
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
void gen_key_set(generator_t *gen, uint8_t selection, int8_t direction){
    switch(selection){
        case 0: //NOP State, the menu item is selected
        break;
        case 1: //turn the generator on
            if(direction>0)
                gen->active = 1;
            else if(direction<0)
                gen->active = 0;
        break;
        case 2:  //Base probability
            if(!(gen->base_prob==0 && direction <0)) gen->base_prob += direction;
            if(gen->base_prob >100) gen->base_prob = 100;
            gen->current_prob = gen->base_prob;
        break;
        case 3: //Scaling
            if(!(gen->scale==0 && direction <0)) gen->scale += direction;
            if(gen->scale >100) gen->scale = 100;
        break;
        case 5: //delay
            if(!(gen->delay==0 && direction <0)) gen->delay += direction;
            if(gen->delay>511) gen->delay = 511;
        break;
        case 4: //Range
        if(!(gen->range==0 && direction <0)) gen->range += direction;
            if(gen->range >11) gen->range = 11;
        break;      
        case 6: // changes per cycle
        if(!(gen->max_changes == 0 && direction <0)) gen->max_changes += direction;
            if(gen->max_changes >512) gen->max_changes = 512;
        break;               
        default:
        if(direction >0)
        gen->possible_val[selection-7] = 1;
        else if(direction<0)
        gen->possible_val[selection-7] = 0;
        break;
    }
}

void gen_scale_set(generator_t *gen, uint8_t selection, int8_t direction){
    switch(selection){
        case 0: //NOP State, the menu item is selected
        break;
        case 1: //turn the generator on
            if(direction>0)
                gen->active = 1;
            else if(direction<0)
                gen->active = 0;
        break;
        case 2:  //Base probability
            if(!(gen->base_prob==0 && direction <0)) gen->base_prob += direction;
            if(gen->base_prob >100) gen->base_prob = 100;
            gen->current_prob = gen->base_prob;
        break;
        case 3: //Scaling
            if(!(gen->scale==0 && direction <0)) gen->scale += direction;
            if(gen->scale >100) gen->scale = 100;
        break;
        case 5: //delay
            if(!(gen->delay==0 && direction <0)) gen->delay += direction;
            if(gen->delay>511) gen->delay = 511;
        break;
        case 4: //Range
        if(!(gen->range==0 && direction <0)) gen->range += direction;
            if(gen->range >6) gen->range = 6;
        break; 
            case 6: // changes per cycle
        if(!(gen->max_changes == 0 && direction <0)) gen->max_changes += direction;
            if(gen->max_changes >512) gen->max_changes = 512;
        break;       
        default:
        if(direction >0)
        gen->possible_val[selection-7] = 1;
        else if(direction<0)
        gen->possible_val[selection-7] = 0;
        break;
    }
}

void gen_harmony_set(generator_t *gen, uint8_t selection, int8_t direction){
    switch(selection){
        case 0: //NOP State, the menu item is selected
        break;
        case 1: //turn the generator on
            if(direction>0)
                gen->active = 1;
            else if(direction<0)
                gen->active = 0;
        break;
        case 2:  //Base probability
            if(!(gen->base_prob==0 && direction <0)) gen->base_prob += direction;
            if(gen->base_prob >100) gen->base_prob = 100;
            gen->current_prob = gen->base_prob;
        break;
        case 3: //Scaling
            if(!(gen->scale==0 && direction <0)) gen->scale += direction;
            if(gen->scale >100) gen->scale = 100;
        break;
        case 5: //delay
            if(!(gen->delay==0 && direction <0)) gen->delay += direction;
            if(gen->delay>511) gen->delay = 511;
        break;
        case 4: //Range
        if(!(gen->range==0 && direction <0)) gen->range += direction;
            if(gen->range >6) gen->range = 6;
        break;
        case 6: // duration
        if(!(gen->max_changes == 0 && direction <0)) gen->max_changes += direction;
            if(gen->max_changes >511) gen->max_changes = 511;
        break;           
        default:
        if(direction >0)
        gen->possible_val[selection-7] = 1;
        else if(direction<0)
        gen->possible_val[selection-7] = 0;
        break;
    }
}

void gen_sequence_set(generator_t *gen, uint8_t selection, int8_t direction){
    switch(selection){
        case 0: //NOP State, the menu item is selected
        break;
        case 1: //turn the generator on
            if(direction>0)
                gen->active = 1;
            else if(direction<0)
                gen->active = 0;
        break;
        case 2:  //Base probability
            if(!(gen->base_prob==0 && direction <0)) gen->base_prob += direction;
            if(gen->base_prob >100) gen->base_prob = 100;
            gen->current_prob = gen->base_prob;
        break;
        case 3: //Scaling
            if(!(gen->scale==0 && direction <0)) gen->scale += direction;
            if(gen->scale >100) gen->scale = 100;
        break;
        case 5: //delay
            if(!(gen->delay==0 && direction <0)) gen->delay += direction;
            if(gen->delay>511) gen->delay = 511;
        break;
        case 4: //Range
        if(!(gen->range==0 && direction <0)) gen->range += direction;
            if(gen->range >6) gen->range = 6;
        break;
        case 6: // changes per cycle
        if(!(gen->max_changes == 0 && direction <0)) gen->max_changes += direction;
            if(gen->max_changes >512) gen->max_changes = 512;
        break;       
        default:
        if(direction >0)
        gen->possible_val[selection-7] = 1;
        else if(direction<0)
        gen->possible_val[selection-7] = 0;
        break;
    }
}
bool gen_sequence_run(generator_t *gen,sequence_t * seq){
if(gen->cycles < gen->delay) {
        gen->cycles ++;
        return false;
    }
    if(gen->direction==0){
        if(gen_random(2))
            gen->direction = 1;
        else 
            gen->direction = -1;
    }

     for(int16_t i = 0; i < seq->last_step && gen->changes_made < gen->max_changes; i++){
        if(seq->note_value[seq->current_page][i].type==REGULAR_NOTE && !seq->note_value[seq->current_page][i].protected){
            if(gen_random(100)<gen->current_prob){
                note temp = seq->note_value[seq->current_page][i];
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
            gen->direction = 0;
        }
        else gen->current_prob += gen->scale;
        if(gen->changed){
            gen->changed = false;
            gen->changes_made = 0;
            return true;
        }
        else return false;
}

bool gen_scale_run(generator_t *gen,sequencer_t * seq){
    if(delayed(gen)) return false;
    if(gen->current_prob < gen_random(100)){
        random_direction(gen);
        uint8_t count;
        if(gen->range>1)
            count=gen_random(gen->range) + 1;
        else count = 1;
        while(!gen->possible_val[seq->current_scale.number + count * gen->direction] && count<7)
            count++;
        if(count<7)
            seq->current_scale = scales[seq->current_scale.number + count * gen->direction];
        gen->changed = true;
    }
    return gen_update(gen);
}

bool gen_key_run(generator_t *gen,sequencer_t * seq){
    if(delayed(gen)) return false;
    if(gen->current_prob > gen_random(100)){
        random_direction(gen);
        uint8_t count = 0;
        int16_t chosen_key = seq->key_select + gen->direction;
        while(!gen->possible_val[chosen_key] && count<12){
            count++;
            if(gen->direction>0){
                chosen_key ++;
                if(chosen_key>11)
                chosen_key=0;
            }
            else{
                chosen_key --;
                if(chosen_key<0)
                chosen_key=11;
            }
        }
            
        if(count<12)
            set_key(seq, keys[key_selector[chosen_key]]);
        seq->key_select = chosen_key;
        gen->changed = true;
    }
    return gen_update(gen);
}
bool gen_harmony_run(generator_t *gen, sequence_t * seq){
    if(delayed(gen)) return false;
    if( gen->current_prob > gen_random(100) && seq->harmonize == 0){
        uint8_t count;
        if(gen->range>1)
            count=gen_random(gen->range) + 1;
        else count = 0;
        while(!gen->possible_val[count] && count<7)
            count++;
        if(count<7){
            seq->harmony=count;
            seq->harmonize = -1;
            gen->changed = true;
            gen->changes_made = gen->cycles;
        }
    }
    if(gen->cycles == gen->reset || (gen->max_changes + gen->changes_made) == gen->cycles){
        seq->harmonize = 0;
        seq->harmony = 0;
        gen->cycles = 0;
        gen->changed = false;
        gen->current_prob = gen->base_prob;
    }
    else{ 
        gen->current_prob += gen->scale;
        gen->cycles ++;
    }

}



