#pragma once
#include "sequencer.h"
#include "ssd1306.h"
#include "../include/generate.h"


void setup_ui();

void UI_menu_open(uint8_t selected);

void UI_main_menu(sequencer_t *seq, uint8_t menu_state,bool menu_active, int8_t selection_state, note control_note, int8_t sub_select, bool dynamic);

void UI_generate_menu(uint8_t selection, uint8_t sub_selection);

void UI_draw_bar(sequencer_t *seq, uint8_t current_bar, int16_t current_step, uint8_t menu_state);

void UI_draw_sheet();

void UI_startup();


void UI_main_selection(uint8_t UI_State_selector);

void main_menu(sequencer_t *seq, bool dynamic);

void edit_menu(sequencer_t *seq, uint8_t current_bar, note *control_note);

