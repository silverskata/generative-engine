#pragma once
#include "sequencer.h"
#include "ssd1306.h"


void setup_ui();

void UI_main_menu(sequencer_t * seq, uint8_t menu_state, int8_t selection_state, note control_note);

void UI_draw_bar(sequencer_t * seq, uint8_t current_bar, int16_t current_step,uint8_t menu_state);

void UI_draw_sheet();

void UI_startup();

void UI_Test(int val1,int val2, int val3);

void UI_main_selection(uint8_t UI_State_selector);

void main_menu(sequencer_t * seq);

void edit_menu(sequencer_t *seq, uint8_t current_bar, note *control_note);