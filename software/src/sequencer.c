#include "../include/sequencer.h"
#include "stdlib.h"


    //TODO WRITE AND READ from memory location
float calibration1[]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
float calibration2[]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};


void init_sequencer(sequencer_t *self, sequence_t * s1,sequence_t * s2, repeating_timer_t *timer){

    self->sequencers[0] = *s1;
    self->sequencers[1] = *s2;
    self->timer = timer;

    for(uint8_t k = 0; k<SEQUENCER_AMOUNT;k++){
        self->sequencers[k].current_page = 0;
        self->sequencers[k].selected_step = 0;
        self->sequencers[k].active = true;
        self->sequencers[k].lowest_octave = 255;
        for(uint16_t j=0;j<PAGESIZE;j++){
            for(uint16_t i = 0; i < STEPSIZE*MAX_NOTE_LENGTH;i++){
                self->sequencers[k].note_value[j][i].type = NO_NOTE;
                self->sequencers[k].note_value[j][i].length = NO_NOTE;
            }
        }

    }
    self->sequencers[0].CV_CHANNEL = DAC_PITCH3;
    self->sequencers[0].GATE_CHANNEL = GATE3;
    self->sequencers[1].CV_CHANNEL = DAC_CV1;
    self->sequencers[1].GATE_CHANNEL = GATE1;
    self->current_key = keys[KEY_C];
    self->playing = false;
    self->legato = false;
    self->current_step = 0;
    self->current_scale = scales[IONIAN];
    self->key_select = 5;
    self->scale_select = 0;
    self->active_sequence = 0;
    self->tempo = 120;
    sleep_ms(200);
}
void select_page(sequencer_t *self, int8_t direction){
    self->sequencers[self->active_sequence].current_page = self->sequencers[self->active_sequence].current_page + direction;
    if(self->sequencers[self->active_sequence].current_page>PAGESIZE-1) self->sequencers[self->active_sequence].current_page = 0;
    else if(self->sequencers[self->active_sequence].current_page <0) self->sequencers[self->active_sequence].current_page = PAGESIZE-1;
}
void select_active_sequencer(sequencer_t * self, int8_t direction){
            self->active_sequence =self->active_sequence + direction;
            if(self->active_sequence >= SEQUENCER_AMOUNT) self->active_sequence = 0;
            else if(self->active_sequence < 0) self->active_sequence = SEQUENCER_AMOUNT -1;
}

void sequence_on_off(sequencer_t * self){
    self->sequencers[self->active_sequence].active = !self->sequencers[self->active_sequence].active;
    gpio_put(self->sequencers[self->active_sequence].GATE_CHANNEL,GATE_OFF);
}
void play_note(sequencer_t *self, note note, uint8_t channel){
    float calibration = 1.0;
    uint8_t value=note.value;
    if(self->scale_select != CHROMATIC) {
        note.value = self->current_scale.tonal[value] + self->current_key.value + note.modulation + default_scale[value];
    }
    // SUPPOSED TO BE ALL 3 PITCHES WHEN ALL DACS ARE IN PLACE   *** TODO ***
    if(channel == DAC_PITCH3) calibration = calibration1[(note.octave * 12)+note.value];
    else if(channel == DAC_CV1) calibration = calibration2[(note.octave * 12)+note.value];
    set_voltage(channel, (((note.octave * 12)+note.value) * NOTE_VOLTAGE * calibration) );
}
void free_play_pressed(sequencer_t *self,uint8_t channel,uint16_t value,uint8_t octave){
    set_voltage(self->sequencers[channel].CV_CHANNEL, (value+(octave*12))*NOTE_VOLTAGE);
    gpio_put(self->sequencers[channel].GATE_CHANNEL,GATE_ON);
}
void free_play_released(sequencer_t *self,uint8_t channel){
    gpio_put(self->sequencers[channel].GATE_CHANNEL,GATE_OFF);
}

 int64_t blink_off(alarm_id_t id, void * user_data){
    gpio_put(29,0);
    return 0;
}


void play_step(sequencer_t *self){
    if (self->current_step % 8 == 0){ gpio_put(29,1);
    add_alarm_in_ms(20, blink_off, NULL, false);
    }
    for(uint8_t i = 0; i<SEQUENCER_AMOUNT;i++){
        if(self->sequencers[i].active){
        uint8_t step_type = get_note_type(&self->sequencers[i], self->current_step);
        if(step_type == REMAIN_NOTE) {
            gpio_put(self->sequencers[i].GATE_CHANNEL,GATE_ON);
        }
        else if(step_type == REGULAR_NOTE){
            gpio_put(self->sequencers[i].GATE_CHANNEL,GATE_ON);
            play_note(self, self->sequencers[i].note_value[self->sequencers[i].current_page][self->current_step], self->sequencers[i].CV_CHANNEL);
        }
        else if(step_type == REST_NOTE || !step_type || step_type == NOTE_STACCATO)
            gpio_put(self->sequencers[i].GATE_CHANNEL,GATE_OFF);
        }
    }
}

void stop_sequence(sequencer_t *self){
    self->playing =false;
    self->current_step = 0;
}

void set_current_step(sequencer_t *self,uint16_t step){
    self->current_step = step;
}

void sequencer_pause_play(sequencer_t *self){
    self->playing = !self->playing;
    if(!self->playing){
        for(uint8_t i = 0; i<SEQUENCER_AMOUNT;i++){
            gpio_put(self->sequencers[i].GATE_CHANNEL,GATE_OFF);
        }
    }
}

void set_time_signature(sequence_t *self, uint8_t over, uint8_t under){
    self->time_signature[0] = over;
    self->time_signature[1] = under;
}


void set_key(sequencer_t *self, key key){
    self->current_key = key;
}

void select_key(sequencer_t *self, int8_t dir){
    if(dir>=0) self->key_select ++;
    else if(dir<0) self->key_select --;
    if(self->key_select<0) self->key_select = 11;
    else if(self->key_select>11) self->key_select = 0;
    self->current_key = keys[key_selector[self->key_select]];
}

void select_scale(sequencer_t *self, int8_t dir){
    if (dir >=0)  self->scale_select ++;
    else if (dir < 0)  self->scale_select --;
    if(self->scale_select<0)self->scale_select = 7;
    if(self->scale_select > 7)self->scale_select = 0;
    self->current_scale = scales[self->scale_select];
}

void add_note(sequence_t *self, note note){
     set_note(self, self->selected_step, note);
     self->last_note = self->selected_step;
     select_step(self, 1);
}

void set_note(sequence_t *self, uint16_t step, note value)
{
    int8_t replace = self->note_value[self->current_page][step].length - value.length;  
    if(value.octave * 7 + value.value < self->lowest_octave ) self->lowest_octave = value.octave;
    self->note_value[self->current_page][step] = value;
    int8_t diff = value.length - value.legato;
    if(value.legato>0){
    for (uint8_t i = 1; i < value.legato; i++)
        self->note_value[self->current_page][step + i].type = REMAIN_NOTE;
    for (uint8_t i = 0; i < diff ; i++)
        self->note_value[self->current_page][step + value.legato + i].type = NOTE_STACCATO;
    }
    else {
        for (uint8_t i = 1; i < value.length ; i++)
        self->note_value[self->current_page][step + i].type = NOTE_STACCATO;
    }
    if(replace==0) return;
    else if( replace > 0){
    note rest = {
        .legato = 0,
        .length = replace,
        .type = REST_NOTE,
        .octave = 5,
        .value = 0,
    };
        set_note(self,step + value.length, rest);
    }
    else{
    uint8_t stepcheck = 0;
    // Check for remain notes and replace them with no notes to free up space
    while (self->note_value[self->current_page][step + value.length + stepcheck].type == REMAIN_NOTE ||
        self->note_value[self->current_page][step + value.length + stepcheck].type == NOTE_STACCATO)
            stepcheck++;
    note rest = {
        .legato = 0,
        .length = stepcheck,
        .type = REST_NOTE,
        .octave = 5,
        .value = 0,
    };
    if(stepcheck>0)
    set_note(self,step + value.length, rest);
    }
    
}
void select_step(sequence_t *self, int8_t direction)
{
    int8_t stepcheck;
    if (direction > 0)
    {
        stepcheck = 1;
        while (self->note_value[self->current_page][self->selected_step + stepcheck].type == REMAIN_NOTE || 
                self->note_value[self->current_page][self->selected_step + stepcheck].type == NOTE_STACCATO)
            stepcheck++;
        self->selected_step = self->selected_step + stepcheck;
    }
    else if (direction < 0)
    {
        stepcheck = -1;
        while (self->note_value[self->current_page][self->selected_step + stepcheck].type == REMAIN_NOTE || 
                self->note_value[self->current_page][self->selected_step + stepcheck].type == NOTE_STACCATO)
            stepcheck--;
        self->selected_step = self->selected_step + stepcheck;
    }
}

void step_forward(sequencer_t * self){
    select_step(&self->sequencers[self->active_sequence],1);
}

void step_reverse(sequencer_t * self){
    select_step(&self->sequencers[self->active_sequence],-1);
}

const uint16_t keyboard_scale_map[] =  {0,0,1,1,2,3,3,4,4,5,5,6,6,7};

void keyboard_step_value(note *self, int16_t value){
    uint8_t val = keyboard_scale_map[value];
    self->value = val;
}

void edit_step_type(sequence_t * self, uint8_t type){
    self->note_value[self->current_page][self->selected_step].type = type;
}
void set_rest_step(sequence_t * self, uint8_t length){
    note rest = {
        .legato = 0,
        .length = length,
        .type = REST_NOTE,
        .octave = 5,
        .value = 0,
    };
    set_note(self, self->selected_step,rest);
}

void edit_step_value(sequence_t *self, int8_t value)
{
    self->note_value[self->current_page][self->selected_step].value += value;
    if(self->note_value[self->current_page][self->selected_step].octave >= 5 && self->note_value[self->current_page][self->selected_step].value > 0){
        self->note_value[self->current_page][self->selected_step].octave = 5;
        self->note_value[self->current_page][self->selected_step].value = 0;
        }
    else if(self->note_value[self->current_page][self->selected_step].value > 6){
        self->note_value[self->current_page][self->selected_step].value = 0;
        self->note_value[self->current_page][self->selected_step].octave ++;
    }
    else if(self->note_value[self->current_page][self->selected_step].value < 0){
        self->note_value[self->current_page][self->selected_step].value = 6;
        self->note_value[self->current_page][self->selected_step].octave --;
        if(self->note_value[self->current_page][self->selected_step].octave < 0) {
        self->note_value[self->current_page][self->selected_step].octave = 0;
        self->note_value[self->current_page][self->selected_step].value = 0;
        }
    }
    if(self->note_value[self->current_page][self->selected_step].type = REST_NOTE){ 
        self->note_value[self->current_page][self->selected_step].type = REGULAR_NOTE;
        self->note_value[self->current_page][self->selected_step].legato = self->note_value[self->current_page][self->selected_step].length -1;
        set_note(self,self->selected_step,self->note_value[self->current_page][self->selected_step]);
    }
}

void edit_step_length(sequence_t *self,uint8_t length){
    if(self->note_value[self->current_page]->length != length){
        self->note_value[self->current_page][self->selected_step].length = length;
        set_note(self, self->selected_step, self->note_value[self->current_page][self->selected_step]);
    }
}

void edit_step_legato(sequence_t *self,uint8_t legato){
    if(self->note_value[self->current_page]->legato != legato){
        self->note_value[self->current_page][self->selected_step].legato = legato;
        set_note(self, self->selected_step, self->note_value[self->current_page][self->selected_step]);
    }
}


uint8_t get_note_value(sequencer_t *self, uint16_t step,uint8_t sequence)
{  if(self->scale_select == CHROMATIC) return self->sequencers[sequence].note_value[self->sequencers[sequence].current_page][step].value;
    else{
    uint8_t val = self->sequencers->note_value[step]->value;
    return self->current_key.modulation[val] + self->current_scale.tonal[val] + val;
    }
}

uint8_t get_note_type(sequence_t *self, uint16_t step)
{
    return self->note_value[self->current_page][step].type;
}

void set_current_page(sequence_t *self, uint8_t page)
{
    self->current_page = page;
}

void set_tempo(sequencer_t *self, int16_t set)
{
    self->tempo += set;
    if (self->tempo < 20)
        self->tempo = 20;
    if (self->tempo > 360)
        self->tempo = 360;
    // OUR BPM needs to be in respect to quarter notes
    float diviser = 60 / (float)(MAX_NOTE_LENGTH / 4);
    double time = self->tempo / diviser;
    time = 1 / time * 1000 * 1000;
    self->timer->delay_us = -(time);
}

 scale_modulation scale_modulation_identifier[] = {
    {"Cbb", "Cb", "C", "C#", "C##"},
    {"Dbb", "Db", "D", "D#", "D##"},
    {"Ebb", "Eb", "E", "E#", "E##"},
    {"Fbb", "Fb", "F", "F#", "F##"},
    {"Gbb", "Gb", "G", "G#", "G##"},
    {"Abb", "Ab", "A", "A#", "A##"},
    {"Bbb", "Bb", "B", "B#", "B##"}};

char * note_to_string(sequencer_t *seq, uint16_t note_step){
    uint8_t val = seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][note_step].value;
    int8_t key_mod = seq->current_key.modulation[val];
    int8_t scale_mod = seq->current_scale.tonal[val];
    return scale_modulation_identifier[val].name[2 + key_mod +scale_mod];
}