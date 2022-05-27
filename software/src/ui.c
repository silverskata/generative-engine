#include "../include/ui.h"
#include "../include/controller.h"

// UI
char menu_main[][12] = {"Play", "Main", "Edit", "Generate","Song", "Save/Load","Settings"};
char menu_sequencers[][12] = {"Sequence 1", "Sequence 2", "Sequence 3"};
char menu_edit[][12] = {"note", "length", ""};
char menu_generate[][12] = {"Create", "Complete", "Dynamic"};
char generate_create[][12] = {"Keys", "Scale", "Tempo", "Parts"};
char generate_dynamic[][12] = {"Keys", "Scales", "Tempo"}; // CHECK WHATS NEEDED

uint8_t note_y[] = {49, 47, 44, 42, 39, 37, 34, 32, 29, 27, 24, 22, 19, 17, 14};

ssd1306_t disp;

void setup_ui()
{
    disp.external_vcc = false;
    display_setup(i2c0);
    display_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);
}

void UI_Test(int val1, int val2, int val3, controller_t *control){
    char buf[20];
    char buf2[20];
    char buf3[20];
    sprintf(buf,"key1: %d",val1);
    sprintf(buf2,"key2: %d",val2);
    sprintf(buf3,"%d",val3);
    ssd1306_clear(&disp);
    display_write_tight(&disp,5,2,2,buf);
    display_write_tight(&disp,5,12,2,buf2);
    display_write_tight(&disp,5,22,2,buf3);
    ssd1306_show(&disp);
}


void UI_generate_menu(generator_t *gen, sequence_t * s)
{
    ssd1306_clear(&disp);
    char test[] = "generating random.";
    display_write_tight(&disp, 5, 10, 2, test);
    char buff[30];
    sprintf(buff, "%d%d%d%d%d%d%d%d", s->note_value[0][0].value,s->note_value[0][1].value,s->note_value[0][2].value,s->note_value[0][3].value,s->note_value[0][4].value,s->note_value[0][5].value,s->note_value[0][6].value,s->note_value[0][7].value);
    display_write_tight(&disp, 5, 30, 2, buff);
        char buff3[30];
    sprintf(buff3, "%d%d%d%d%d%d%d%d", s->note_value[0][0].octave,s->note_value[0][1].octave,s->note_value[0][2].octave,s->note_value[0][3].octave,s->note_value[0][4].octave,s->note_value[0][5].octave,s->note_value[0][6].octave,s->note_value[0][7].octave);
    display_write_tight(&disp, 5, 40, 2, buff3);
    char buff2[30];
    sprintf(buff2, "Gen c %d ", gen->cycles);
    display_write_tight(&disp, 5, 50, 2, buff2);
    
    ssd1306_show(&disp);
}

void UI_main_menu(sequencer_t *seq, uint8_t menu_state,bool menu_active, int8_t selection_state, note control_note)
{
    if(menu_active)UI_menu_open(selection_state);
      else{
    switch (menu_state)
    {
    case 0: // PLAYING
        ssd1306_clear(&disp);
        UI_draw_sheet();
        UI_draw_bar(seq, (int)(seq->current_step / 32), seq->current_step, menu_state);
        main_menu(seq);
        break;
    case 1: // Menu Closed
        ssd1306_clear(&disp);
        UI_draw_sheet();
        UI_draw_bar(seq, (int)(seq->current_step / 32), seq->current_step, menu_state);
        UI_main_selection(selection_state);
        main_menu(seq);
        break;
    case 3: // Generate menu
        break;
    case 2: // Step editor
        ssd1306_clear(&disp);
        UI_draw_sheet();
        edit_menu(seq, (int)(seq->sequencers[seq->active_sequence].selected_step / 32), &control_note);
        UI_draw_bar(seq, (int)(seq->sequencers[seq->active_sequence].selected_step / 32), seq->sequencers[seq->active_sequence].selected_step + 1, menu_state);

        break;
    case 4: // song

        break;
    case 5: // save
        break;
    case 6: // settings
        break;
    }
    display_write_tight(&disp, 5, 0, 2, menu_main[menu_state]); // MAIN MENU LOCATION
    }
    ssd1306_show(&disp);
}

void edit_menu(sequencer_t *seq, uint8_t current_bar, note *control_note)
{
    char bar_buf[3];
    sprintf(bar_buf, "%d", current_bar + 1);
    display_write_tight(&disp, 0, 16, 2, bar_buf); // CURRENT BAR
    char octave_buf[5];
    sprintf(octave_buf, "%d", seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][seq->sequencers[seq->active_sequence].selected_step].octave);
    char plus[4] ="ADD";
    if(seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][seq->sequencers[seq->active_sequence].selected_step].type==NEXT_NOTE)
        display_write_tight(&disp, 111, 0, 2, plus); // NOTE LOCATION
    else{
        display_write_tight(&disp, 111, 0, 2, note_to_string(seq, (seq->sequencers[seq->active_sequence].selected_step))); // NOTE LOCATION
        display_write_tight(&disp, 123, 0, 2, octave_buf);
    }
    char note_buf[8];
    sprintf(note_buf, "s %d legato %d  octave %d", seq->active_sequence+1, control_note->legato, control_note->octave);
    display_write_tight(&disp, 2, 55, 3, note_buf);
    char note_word[] = "length:";
    display_write_tight(&disp, 40, 6, 3, note_word);
    uint8_t x = 83;
    switch (control_note->length)
    {
    case WHOLENOTE:
        draw_wholenote(&disp, x, 10);
        break;
    case DOTTEDSEMI:
        draw_dotted(&disp, x, 10);
    case SEMINOTE:
        draw_seminote(&disp, x, 10);
        break;
    case DOTTEDQUARTER:
        draw_dotted(&disp, x, 10);
    case QUARTERNOTE:
        draw_4_note(&disp, x, 10);
        break;
    case DOTTEDEIGHT:
        draw_dotted(&disp, x, 10);
    case EIGHTNOTE:
        draw_8_note(&disp, x, 10);
        break;
    case DOTTEDSIXTEENTH:
        draw_dotted(&disp, x, 10);
    case SIXTEENTHNOTE:
        draw_16_note(&disp, x, 10);
        break;
    case THIRTYSECONDNOTE:
        draw_32_note(&disp, x, 10);
        break;
    }
}
void UI_menu_open(uint8_t selected){
    display_black_square(&disp,1,0,58,65);
    display_box_no_top(&disp,0,0,59,66);
    for(uint8_t i = 0; i<7;i++)
    display_write_tight(&disp, 5,1+i*9,3,menu_main[i]);

    display_box(&disp,5,selected*9,49,9);
}
void main_menu(sequencer_t *seq)
{
    char temp_buf[7];
    if (seq->tempo < 100)
        sprintf(temp_buf, " %dbpm", seq->tempo);
    else
        sprintf(temp_buf, "%dbpm", seq->tempo);
    display_write_tight(&disp, 85, 55, 2, temp_buf); // TEMPO
    char bar_buf[2];
    sprintf(bar_buf, "%d", (int)(seq->current_step / 32) + 1);
    display_write_tight(&disp, 0, 16, 2, bar_buf); // CURRENT BAR
    char seq_buf[10];
    sprintf(seq_buf, "se %d", seq->active_sequence + 1);
    display_write_tight(&disp, 44, 55, 2, seq_buf); // ACTIVE SEQUENCER
    if (seq->sequencers[seq->active_sequence].active)
        display_fill_circle(&disp, 73, 58, 2);
    else
        display_circle(&disp, 73, 58, 2);
    char page_buf[2];
    sprintf(seq_buf, "page %d", seq->sequencers[seq->active_sequence].current_page + 1);
    display_write_tight(&disp, 5, 55, 2, seq_buf);
    display_write_tight(&disp, 42, 0, 2, seq->current_key.name);   // KEY LOCATION
    display_write_tight(&disp, 62, 0, 2, seq->current_scale.name); // SCALE LOCATION
}

void UI_main_selection(uint8_t UI_State_selector)
{
    switch (UI_State_selector)
    {
    case 0:
        display_box_no_top(&disp, 1, -3, 30, 12);
        break;
    case 1:
        display_box_no_top(&disp, 39, -3, 16, 12);
        break;
    case 2:
        display_box_no_top(&disp, 59, -3, 65, 12);
        break;
    case 3:
        display_box(&disp, 0, 14, 127, 38);
        break;
    case 4:
        display_box(&disp, 1, 52, 41, 12);
        break;
    case 5:
        display_box(&disp, 41, 52, 36, 12);
        break;
    case 6:
        display_box(&disp, 83, 52, 39, 12);
        break;
    }
}

void UI_startup()
{
    draw_logo(&disp);
    sleep_ms(300);
}

uint8_t key_value_map[] = {KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_A, KEY_B, 0, 0, 3, 0, 1, 2, 5, 6, 0};

uint16_t blink_helper = 0;

void UI_draw_bar(sequencer_t *seq, uint8_t current_bar, int16_t current_step, uint8_t menu_state)
{

    for (uint16_t i = 0; i < 32; i++)
    {
        uint16_t x = 12 + 7 * i / 2;
        if (seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].type == REGULAR_NOTE)
        {
            int16_t height = seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].value +
                             key_value_map[seq->current_key.number] + seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].octave * 7;
            uint16_t y;
            height = height - seq->sequencers[seq->active_sequence].lowest_octave * 7;
            uint8_t count_above = 0;
            uint8_t count_below = 0;

            while (height > 14)
            {
                height = height - 7;
                count_above++;
            }
            while(height<0){
                height = height +7;
                count_below++;
            }
            y = note_y[height];
            if (count_above > 0)
            draw_up_octave(&disp, x, y, count_above);
            if(count_below >0)
            draw_down_octave(&disp, x, y, count_below);

            switch (seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].length)
            {
            case WHOLENOTE:
                    draw_wholenote(&disp, x, y);
                if (current_step + 1 - WHOLENOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            case DOTTEDSEMI:
                draw_dotted(&disp, x, y);
            case SEMINOTE:
                if (height > 11)
                    draw_down_seminote(&disp, x, y);
                else
                    draw_seminote(&disp, x, y);
                if (current_step + 1 - SEMINOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            case DOTTEDQUARTER:
                draw_dotted(&disp, x, y);
            case QUARTERNOTE:
                if (height > 11)
                    draw_down_4_note(&disp, x, y);
                else
                    draw_4_note(&disp, x, y);
                if (current_step + 1 - QUARTERNOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            case DOTTEDEIGHT:
                draw_dotted(&disp, x, y);
            case EIGHTNOTE:
                if (height > 11)
                    draw_down_8_note(&disp, x, y);
                else
                    draw_8_note(&disp, x, y);
                if (current_step - EIGHTNOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            case DOTTEDSIXTEENTH:
                draw_dotted(&disp, x, y);
            case SIXTEENTHNOTE:
                if (height > 11)
                    draw_down_16_note(&disp, x, y);
                else
                    draw_16_note(&disp, x, y);
                if (current_step - SIXTEENTHNOTE < current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            case THIRTYSECONDNOTE:
                draw_32_note(&disp, x, y);
                if (current_step - THIRTYSECONDNOTE < current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, y, 4);
                break;
            }
        }
        else if (seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].type == REST_NOTE)
        {
            switch (seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].length)
            {
            case WHOLENOTE:
                draw_wholerest(&disp, x);
                break;
            case DOTTEDSEMI:
                draw_dotted(&disp, x + 8, 35);
            case SEMINOTE:
                draw_semirest(&disp, x);
                if (current_step + 1 - SEMINOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, 37, 4);
                break;
            case DOTTEDQUARTER:
                draw_dotted(&disp, x + 2, 35);
            case QUARTERNOTE:
                draw_4_rest(&disp, x);
                if (current_step + 1 - QUARTERNOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, 37, 4);
                break;
            case DOTTEDEIGHT:
                draw_dotted(&disp, x + 3, 35);
            case EIGHTNOTE:
                draw_8_rest(&disp, x);
                if (current_step - EIGHTNOTE <= current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, 37, 4);
                break;
            case DOTTEDSIXTEENTH:
                draw_dotted(&disp, x + 3, 35);
            case SIXTEENTHNOTE:
                draw_16_rest(&disp, x);
                if (current_step - SIXTEENTHNOTE < current_bar * 32 + i && current_step >= current_bar * 32 + i + 1)
                    display_circle(&disp, x, 37, 4);
                break;
            case THIRTYSECONDNOTE:
                break;
            }
        }
        else if (seq->sequencers[seq->active_sequence].note_value[seq->sequencers[seq->active_sequence].current_page][current_bar * 32 + i].type == NEXT_NOTE && menu_state == EDIT_MENU)
        {
            display_circle(&disp, x, 33, 4);
            if (blink_helper > 50)
                display_fill_circle(&disp, x, 33, 2);
            blink_helper++;
            if (blink_helper > 80)
                blink_helper = 0;
        }
    }
}

void UI_draw_sheet()
{
    for (int i = 0; i < 5; i++)
        display_line(&disp, 0, (i * 5) + 24, 127, (i * 5) + 24);
    display_line(&disp, 0, 24, 0, 43);
    display_line(&disp, 127, 24, 127, 43);
}