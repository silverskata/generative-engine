#include "../include/sequencer.h"
#include "stdlib.h"

// TODO WRITE AND READ from flash memory
float calibration1[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
float calibration2[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

void init_sequencer(sequencer_t *self, sequence_t *s1, sequence_t *s2, repeating_timer_t *timer)
{

    self->sequencers[0] = *s1;
    self->sequencers[1] = *s2;
    self->timer = timer;
    for(uint8_t i = 0; i<SEQUENCER_AMOUNT;i++){
        clear_sequence(&self->sequencers[i]);
        self->sequencers[i].id = i;
    }
    //*** TODO when hardware is fixed ***//
    self->sequencers[0].CV_CHANNEL = DAC_PITCH3;
    self->sequencers[0].GATE_CHANNEL = GATE3;
    self->sequencers[1].CV_CHANNEL = DAC_CV1;
    self->sequencers[1].GATE_CHANNEL = GATE1;
    self->keyboard_mode= KEYBOARD_UNISON;
    self->current_key = keys[KEY_C];
    self->playing = false;
    self->current_step = 0;
    self->current_scale = scales[IONIAN];
    self->end_of_sequence = MAX_NOTE_LENGTH;
    self->key_select = 5;
    self->scale_select = 0;
    self->active_sequence = 0;
    self->tempo = 120;
    sleep_ms(200);
}

void clear_sequence(sequence_t  *seq){
        for (uint16_t j = 0; j < PAGESIZE; j++)
        {
            for (uint16_t i = 0; i < STEPSIZE * MAX_NOTE_LENGTH; i++)
            {
                seq->note_value[j][i].type = NO_NOTE;
                seq->note_value[j][i].length = NO_NOTE;
                seq->note_value[j][i].protected = false;
            }
            seq->note_value[j][0].type = NEXT_NOTE;
        }
        seq->total_notes = 0;
        seq->current_page = 0;
        seq->selected_step = 0;
        seq->active = false;
        seq->harmonize = 0;
        seq->lowest_octave = 255;
}


void select_page(sequencer_t *self, int8_t direction)
{
    self->sequencers[self->active_sequence].current_page = self->sequencers[self->active_sequence].current_page + direction;
    if (self->sequencers[self->active_sequence].current_page > PAGESIZE - 1)
        self->sequencers[self->active_sequence].current_page = 0;
    else if (self->sequencers[self->active_sequence].current_page < 0)
        self->sequencers[self->active_sequence].current_page = PAGESIZE - 1;
}
void select_active_sequencer(sequencer_t *self, int8_t direction)
{
    self->active_sequence = self->active_sequence + direction;
    if (self->active_sequence >= SEQUENCER_AMOUNT)
        self->active_sequence = 0;
    else if (self->active_sequence < 0)
        self->active_sequence = SEQUENCER_AMOUNT - 1;
}


void sequence_on_off(sequencer_t *self)
{
    self->sequencers[self->active_sequence].active = !self->sequencers[self->active_sequence].active;
    gpio_put(self->sequencers[self->active_sequence].GATE_CHANNEL, GATE_OFF);
}
void play_note(sequencer_t *self, note note, uint8_t channel)
{
    float calibration = 1.0;
    uint8_t value = note.value;
    if (channel == DAC_PITCH3){
        calibration = calibration1[(note.octave * 12) + note.value];
    }
    else if (channel == DAC_CV1){
        calibration = calibration2[(note.octave * 12) + note.value];
    }
    if (self->scale_select != CHROMATIC)
    {
        note.value = self->current_scale.tonal[value] + self->current_key.value + note.modulation + default_scale[value];
    }
    // SUPPOSED TO BE ALL 3 PITCHES WHEN ALL DACS ARE IN PLACE   *** TODO ***

    set_voltage(channel, (((note.octave * 12) + note.value) * NOTE_VOLTAGE * calibration));
}

uint8_t played_note[2];
uint8_t active_notes=0;

void reset_notes(){
    played_note[0] = 0;
    played_note[1] = 0;
    active_notes = 0;
}

void free_play_pressed(sequencer_t *self, uint16_t value)
{
    if(value >12) return;
    if(self->keyboard_mode == KEYBOARD_OFF) return;
    played_note[active_notes] = value;
    active_notes ++;

    switch(self->keyboard_mode){
        case KEYBOARD_UNISON:
        for(uint8_t i = 0; i< SEQUENCER_AMOUNT;i++){
            if(!self->sequencers[i].active || !self->playing){
            set_voltage(self->sequencers[i].CV_CHANNEL, (value + (self->keyboard_octave * 12)) * NOTE_VOLTAGE);
            gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_ON);
            }
        }
        break;
        case KEYBOARD_POLY:
            for(uint8_t i = 0; i< active_notes+1;i++){
            set_voltage(self->sequencers[i].CV_CHANNEL, (played_note[i] + (self->keyboard_octave * 12)) * NOTE_VOLTAGE);
            gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_ON);
        }
        break;
    }
}
void free_play_released(sequencer_t *self, uint16_t value)
{   
   
    if(self->keyboard_mode == KEYBOARD_OFF) return; 
        active_notes --;
    if(value >12) active_notes = 0;
    switch(self->keyboard_mode){
        case KEYBOARD_UNISON:
            if(active_notes == 0){
            for(uint8_t i = 0; i< SEQUENCER_AMOUNT; i++){
                if(!self->sequencers[i].active || !self->playing){
                    gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_OFF);
                }
            }
            }
            else{
                for(uint8_t i = 0; i< SEQUENCER_AMOUNT; i++){
                    if(!self->sequencers[i].active || !self->playing){
                        set_voltage(self->sequencers[i].CV_CHANNEL, (value + (self->keyboard_octave * 12)) * NOTE_VOLTAGE);
                    }
                }
            }
        break;
        case KEYBOARD_POLY:

            gpio_put(self->sequencers[active_notes].GATE_CHANNEL, GATE_ON);
        break;
    }
}


int64_t blink_off(alarm_id_t id, void *user_data)
{
    gpio_put(29, 0);
    gpio_put(11, GATE_OFF);
    return 0;
}

void play_step(sequencer_t *self)
{
    if (self->current_step % 8 == 0)
    {
        gpio_put(29, 1);
        gpio_put(11, GATE_ON);
        add_alarm_in_ms(20, blink_off, NULL, false);
    }
    for (uint8_t i = 0; i < SEQUENCER_AMOUNT; i++)
    {   

        if (self->sequencers[i].active)
        {   
            note step;
            if(self->sequencers[i].harmonize!=0){
                step = self->sequencers[i+self->sequencers[i].harmonize].note_value[self->sequencers[i+self->sequencers[i].harmonize].current_page][self->current_step];
                step.value += self->sequencers[i].harmony;
            }
            else
                step = self->sequencers[i].note_value[self->sequencers[i].current_page][self->current_step];
            uint8_t step_type = step.type;
            if (step_type == REMAIN_NOTE)
            {
                gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_ON);
            }
            else if (step_type == REGULAR_NOTE)
            {
                gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_ON);
                    play_note(self, step, self->sequencers[i].CV_CHANNEL);    
            }
            else if (step_type == REST_NOTE || !step_type || step_type == NOTE_STACCATO)
                gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_OFF);
        }
    }
}

void stop_sequence(sequencer_t *self)
{
    self->playing = false;
    self->current_step = 0;
}

void sequencer_pause_play(sequencer_t *self)
{
    self->playing = !self->playing;
    if (!self->playing)
    {
        for (uint8_t i = 0; i < SEQUENCER_AMOUNT; i++)
        {
            gpio_put(self->sequencers[i].GATE_CHANNEL, GATE_OFF);
        }
    }
}

void set_time_signature(sequence_t *self, uint8_t over, uint8_t under)
{
    self->time_signature[0] = over;
    self->time_signature[1] = under;
}

void set_key(sequencer_t *self, key key)
{
    self->current_key = key;
}

void select_key(sequencer_t *self, int8_t dir)
{
    if (dir >= 0)
        self->key_select++;
    else if (dir < 0)
        self->key_select--;
    if (self->key_select < 0)
        self->key_select = 11;
    else if (self->key_select > 11)
        self->key_select = 0;
    self->current_key = keys[key_selector[self->key_select]];
}

void select_scale(sequencer_t *self, int8_t dir)
{
    if (dir >= 0)
        self->scale_select++;
    else if (dir < 0)
        self->scale_select--;
    if (self->scale_select < 0)
        self->scale_select = 7;
    if (self->scale_select > 7)
        self->scale_select = 0;
    self->current_scale = scales[self->scale_select];
}

void add_note(sequencer_t *self, note note)
{
    self->sequencers[self->active_sequence].active=true;
    set_note(self, &self->sequencers[self->active_sequence], self->sequencers[self->active_sequence].selected_step, note);
    step_forward(self);
}



void set_note(sequencer_t *seq, sequence_t *self, uint16_t step, note value)
{
    int8_t replace = self->note_value[self->current_page][step].length - value.length;
    bool next = false;
    if (self->note_value[self->current_page][step].type == NEXT_NOTE)
        next = true;
    if (value.octave * 7 + value.value < self->lowest_octave)
        self->lowest_octave = value.octave;
    self->note_value[self->current_page][step] = value;
    int8_t diff = value.length - value.legato;
    if (value.legato > 0)
    {
        for (uint8_t i = 1; i < value.legato; i++)
            self->note_value[self->current_page][step + i].type = REMAIN_NOTE;
        for (uint8_t i = 0; i < diff; i++)
            self->note_value[self->current_page][step + value.legato + i].type = NOTE_STACCATO;
    }
    else
    {
        for (uint8_t i = 1; i < value.length; i++)
            self->note_value[self->current_page][step + i].type = NOTE_STACCATO;
    }
    if (next)
    {
        self->note_value[self->current_page][step + value.length].type = NEXT_NOTE;
        self->last_step = step + value.length;
        self->total_notes ++;
        if(value.type == REGULAR_NOTE) self->total_notes ++;
        while (step + value.length > seq->end_of_sequence)
            seq->end_of_sequence += MAX_NOTE_LENGTH;
        return;
    }
    if (replace == 0)
        return;
    else if (replace > 0)
    {
        note rest = {
            .legato = 0,
            .length = replace,
            .type = REST_NOTE,
            .octave = 5,
            .value = 0,
        };
        set_note(seq, self, step + value.length, rest);
    }
    else
    {
        uint8_t stepcheck = 0;
        // Check for remain notes and replace them with no notes with rests
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
        if (stepcheck > 0)
            set_note(seq, self, step + value.length, rest);
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
        {
            if (self->note_value[self->current_page][self->selected_step + stepcheck].type == NEXT_NOTE)
                break;
            stepcheck++;
        }
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

void step_forward(sequencer_t *self)
{
    if (self->sequencers[self->active_sequence].note_value[self->sequencers[self->active_sequence].current_page][self->sequencers[self->active_sequence].selected_step].type != NEXT_NOTE)
        select_step(&self->sequencers[self->active_sequence], 1);
}

void step_reverse(sequencer_t *self)
{
    select_step(&self->sequencers[self->active_sequence], -1);
}

const uint16_t keyboard_scale_map[] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7};

void keyboard_step_value(note *self, int16_t value)
{
    uint8_t val = keyboard_scale_map[value];
    self->value = val;
}

void edit_step_type(sequence_t *self, uint8_t type)
{
    self->note_value[self->current_page][self->selected_step].type = type;
}
void set_rest_step(sequencer_t *self, uint8_t length)
{
    note rest = {
        .legato = 0,
        .length = length,
        .type = REST_NOTE,
        .octave = 5,
        .value = 0,
    };
    set_note(self, &self->sequencers[self->active_sequence], self->sequencers[self->active_sequence].selected_step, rest);
}

void edit_step_value(sequencer_t *self, int8_t value)
{
    note *note_p = &self->sequencers[self->active_sequence].note_value[self->sequencers[self->active_sequence].current_page][self->sequencers[self->active_sequence].selected_step];
    note_p->value += value;
    if (note_p->octave >= 5 && note_p->value > 0)
    {
        note_p->octave = 5;
        note_p->value = 0;
    }
    else if (note_p->value > 6)
    {
        note_p->value = 0;
        note_p->octave++;
    }
    else if (note_p->value < 0)
    {
        note_p->value = 6;
        note_p->octave--;
        if (note_p->octave < 0)
        {
            note_p->octave = 0;
            note_p->value = 0;
        }
    }
    if (note_p->type = REST_NOTE)
    {
        note_p->type = REGULAR_NOTE;
        note_p->legato = note_p->length - 1;
        set_note(self, &self->sequencers[self->active_sequence], self->sequencers[self->active_sequence].selected_step, *note_p);
    }
}

uint8_t get_note_value(sequencer_t *self, uint16_t step, uint8_t sequence)
{
    if (self->scale_select == CHROMATIC)
        return self->sequencers[sequence].note_value[self->sequencers[sequence].current_page][step].value;
    else
    {
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
    {"C$$", "C$", "C", "C#", "C##"},
    {"D$$", "D$", "D", "D#", "D##"},
    {"E$$", "E$", "E", "E#", "E##"},
    {"F$$", "F$", "F", "F#", "F##"},
    {"G$$", "G$", "G", "G#", "G##"},
    {"A$$", "A$", "A", "A#", "A#"},
    {"B$$", "B$", "B", "B#", "B##"}};

char *note_to_string(sequencer_t *seq, uint16_t note_step)
{
    uint8_t val = seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][note_step].value;
    int8_t key_mod = seq->current_key.modulation[val];
    int8_t scale_mod = seq->current_scale.tonal[val];
    int8_t key_scaling = seq->current_key.number;

        switch (key_scaling){
            case KEY_F_SHARP:
            key_scaling -= 6;
            break;
            case KEY_B_FLAT:
            case KEY_A_FLAT:
            key_scaling -= 8;
            break;
            case KEY_D_FLAT:
            case KEY_E_FLAT:
                key_scaling -= 10;
            break;
        }
    key_scaling += val;
    while(key_scaling >= 7)
        key_scaling -= 7;
    return scale_modulation_identifier[key_scaling].name[2 + key_mod + scale_mod];
}