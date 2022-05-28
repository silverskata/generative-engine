#pragma once

#include "scale.h"
#include "key.h"
#include "misc.h"
#include "dac.h"
#include "button_definition.h"
#include "note.h"
#define PLAY true
#define STOP false
#define SEQUENCER_AMOUNT 2 // ONLY USING 2 SEQUENCER ATM CHANGE IF MORE CHANNELS ARE ADDED
#define STEPSIZE 16
#define PAGESIZE 8
#define NOTE_VOLTAGE 41.65f // CAN BE CHANGED TO TUNE THE OUTPUT
#define MAX_NOTE_LENGTH 32  // ALLOWS FOR 32nd notes.
#define NO_NOTE 0
#define REGULAR_NOTE 1
#define REMAIN_NOTE 2
#define REST_NOTE 3
#define NOTE_STACCATO 4
#define NEXT_NOTE 5
#define KEYBOARD_OFF 0
#define KEYBOARD_UNISON 1
#define KEYBOARD_POLY 2

typedef struct
{
    int8_t current_page;
    note note_value[PAGESIZE][STEPSIZE * MAX_NOTE_LENGTH]; // GIVES A "NORMAL" STEP SEQUENCE LENGTH OF STEPSIZE * MAX_NOTE_LENGTH / 4
    // uint8_t cv_value[STEPSIZE*MAX_NOTE_LENGTH];        UNUSED AT THIS POINT
    uint16_t selected_step;
    uint8_t CV_CHANNEL;
    uint8_t GATE_CHANNEL;
    uint8_t lowest_octave;
    uint8_t time_signature[2];
    int8_t modulation;
    uint16_t last_step;
    uint8_t id;
    int8_t harmonize;     //select which channel to harmonize with 0 for off
    int8_t harmony;       //Select which interval to harmonize
    uint16_t total_notes; //counting the number of REGULAR_NOTE for the generator
    bool active;
    uint16_t regular_note_position[];
} sequence_t;

typedef struct
{
    sequence_t sequencers[SEQUENCER_AMOUNT];
    bool playing;
    uint16_t end_of_sequence;
    uint16_t current_step;
    key current_key;
    int8_t key_select;
    uint16_t tempo;
    int8_t active_sequence;
    int8_t scale_select;
    scale current_scale;
    uint8_t keyboard_mode;
    uint8_t keyboard_octave;
    repeating_timer_t *timer;
} sequencer_t;

void init_sequencer(sequencer_t *self, sequence_t *s1, sequence_t *s2, repeating_timer_t *timer);

void clear_sequence(sequence_t  *seq);

void set_note(sequencer_t *seq, sequence_t *self, uint16_t step, note value);

void select_step(sequence_t *self, int8_t direction);

void edit_step(sequence_t *self, uint16_t step, note note);

uint8_t get_note_value(sequencer_t *self, uint16_t step, uint8_t sequence);

uint8_t get_note_type(sequence_t *self, uint16_t step);

void set_current_page(sequence_t *self, uint8_t page);

bool next_step(struct repeating_timer *t);

void play_step(sequencer_t *self);

void set_current_step(sequencer_t *self, uint16_t step);

void set_playing(sequencer_t *self);

void set_scale(sequencer_t *self, uint8_t scale);

void set_time_signature(sequence_t *self, uint8_t over, uint8_t under);

void set_key(sequencer_t *self, key key);

void select_key(sequencer_t *self, int8_t direction);

void select_scale(sequencer_t *self, int8_t dir);

void select_active_sequencer(sequencer_t *self, int8_t select);

void sequence_on_off(sequencer_t *self);

void select_page(sequencer_t *self, int8_t direction);
/**
 *	@brief gets the name of a note
 *
 *	@param[in] note : the selected note
 *
 * 	@return the name as a char array.
 */
char *note_to_string(sequencer_t *seq, uint16_t note_step);

/**
 *	@brief sets the output value corresponding to the selected note
 *
 *	@param[in] note : selected note to play
 *	@param[in] channel : selects output channel 1-6
 *
 * 	@return void.
 */
void play_note(sequencer_t *self, note note, uint8_t channel);

void set_tempo(sequencer_t *self, int16_t set);

void free_play_pressed(sequencer_t *self, uint16_t value);

void free_play_released(sequencer_t *self, uint16_t value);

void sequencer_pause_play(sequencer_t *self);

void step_reverse(sequencer_t *self);

void step_forward(sequencer_t *self);

void reset_notes();

// EDIT

void edit_step_value(sequencer_t *self, int8_t value);

void edit_step_type(sequence_t *self, uint8_t type);

void set_rest_step(sequencer_t *self, uint8_t length);

void add_note(sequencer_t *self, note note);

void keyboard_step_value(note *self, int16_t value);