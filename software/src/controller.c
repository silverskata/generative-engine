#include "../include/controller.h"

uint16_t button_press = 0;
int16_t keyboard_press = -1;
int8_t encoder_direction = 0;

void control_task(controller_t *controller)
{
    if (!queue_is_empty(&controller->buttons->controlqueue))
    {
        button_press = read_control_queue(controller);
        if (!update_shift(controller, button_press));
    }
    else
        button_press = 0;
            if(controller->keyboard_active == 3 && !queue_is_empty(&controller->buttons->keyboardqueue)){
                keyboard_press = read_keyboard_queue(controller);
            }
            else keyboard_press = -1;
    if (multicore_fifo_rvalid())
        encoder_direction = multicore_fifo_pop_blocking();
    else
        encoder_direction = 0;

    switch (controller->menu_state)
    {
    case MAIN_PLAYING:
        update_state_playing(controller, button_press, keyboard_press, encoder_direction);
        break;
    case MENU_CLOSED:
        update_state_main(controller, button_press, keyboard_press, encoder_direction);
        break;
    case MENU_OPEN:

        break;
    case EDIT_MENU:
        update_state_edit(controller, button_press, keyboard_press, encoder_direction);
        break;
    case GENERATE_MENU:
        update_state_generation(controller, button_press, encoder_direction);
        break;
    }
}

void init_controller(controller_t *control, button_control_t *buttons, sequencer_t *seq, generator_t *gen)
{
    control->sequencer = seq;
    control->gen = gen;
    control->buttons = buttons;
    control->shift = false;
    control->keyboard_active = 3;
    control->note.length = QUARTERNOTE;
    control->note.legato = QUARTERNOTE - 1;
    control->note.value = 0;
    control->note.octave = 0;
    control->note.modulation = 0;
    control->note.type = REGULAR_NOTE;
};

uint16_t read_control_queue(controller_t *self)
{
    uint16_t val;
    queue_remove_blocking(&self->buttons->controlqueue, &val);
    return val;
}

uint16_t read_keyboard_queue(controller_t *self)
{
    uint16_t val;
    queue_remove_blocking(&self->buttons->keyboardqueue, &val);
    return val;
}

bool is_input(uint16_t val)
{
    return (val & 0x01);
}

bool update_shift(controller_t *self, uint16_t button_press)
{
    if (button_press >> 1 != BUTTON_SHIFT)
        return false;
    if (is_input(button_press))
        self->shift = true;
    else
        self->shift = false;
    return true;
}
// 0 for off and 1 for on
void set_keyboard(controller_t *self, uint8_t onoff)
{
    if (onoff != 0)
        self->keyboard_active = 3;
    else
        self->keyboard_active = 1;
}
void select_note_length(controller_t *self, int8_t direction)
{
    self->note.length += direction;
    if (self->note.length > MAX_NOTE_LENGTH)
        self->note.length = MAX_NOTE_LENGTH;
    else if (self->note.length < 1)
        self->note.length = 1;
}

void update_state_playing(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction)
{
    if (self->shift)
        set_tempo(self->sequencer, encoder_direction * 5);
    else
        set_tempo(self->sequencer, encoder_direction);

    if (button_press != 0 && !is_input(button_press))
        switch (button_press >> 1)
        {
        case BUTTON_UP:
            self->sequencer->keyboard_octave++;
            if (self->sequencer->keyboard_octave>6)
            self->sequencer->keyboard_octave = 6;
        break;
        case BUTTON_DOWN:
            self->sequencer->keyboard_octave--;
            if (self->sequencer->keyboard_octave>7)
            self->sequencer->keyboard_octave = 0;
        break;
        case BUTTON_LEFT:
            if (!self->sequencer->playing)
                self->sequencer->current_step = 0;
            break;
        case (BUTTON_RIGHT):
            sequencer_pause_play(self->sequencer);
            break;
        case (BUTTON_OK):
            self->menu_state = MENU_CLOSED;
            break;
        }
        if (keyboard_press > -1){
            if(is_input(keyboard_press))
                free_play_pressed(self->sequencer,keyboard_press);
            else
                free_play_released(self->sequencer,keyboard_press);
        }

}

uint8_t note_length_map[] = {THIRTYSECONDNOTE, SIXTEENTHNOTE, DOTTEDSIXTEENTH, EIGHTNOTE, DOTTEDEIGHT, QUARTERNOTE, DOTTEDQUARTER, SEMINOTE, DOTTEDSEMI, WHOLENOTE};
uint8_t staccato = 1;
void update_state_edit(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction)
{
    if(is_input(keyboard_press))
        keyboard_press = keyboard_press >> 1;
    else keyboard_press = -1;

    int8_t selection_direction = 0;
    if (button_press != 0 && !is_input(button_press))
    {
        switch (button_press >> 1)
        {
        case BUTTON_LEFT:
            if (self->sequencer->sequencers[self->sequencer->active_sequence].selected_step > 0)
                step_reverse(self->sequencer);
            break;
        case (BUTTON_RIGHT):
            if (self->shift)
                sequencer_pause_play(self->sequencer);
            else
            {
                step_forward(self->sequencer);
            }
            break;
        case (BUTTON_UP):
            // SHIFT UP raised the selected note value;
            if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                        .type != NO_NOTE &&
                self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                        .type != REST_NOTE &&
                self->shift)
                edit_step_value(self->sequencer, 1);
            else if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                    [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                         .type != REST_NOTE)
            {
                self->note.octave++;
                if (self->note.octave > 5)
                    self->note.octave = 5;
            }
            break;
        case (BUTTON_DOWN):
            // SHIFT DOWN lowers the selected note value;
            if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                        .type != NO_NOTE &&
                self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                        .type != REST_NOTE &&
                self->shift)
                edit_step_value(self->sequencer, -1);
            else if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                    [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                         .type != REST_NOTE)
            {
                self->note.octave--;
                if (self->note.octave > 5)
                    self->note.octave = 0;
            }
            break;
        case (BUTTON_OK):
            if (self->shift)
                self->menu_state = MAIN_PLAYING;
            else
               self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step].protected = 
                !self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step].protected;

            break;
        }
    }

    if (keyboard_press > -1)
    {
        switch (keyboard_press)
        {
        case 1: /*** TODO  ADD FUNCTIONS TO THE BLACK KEYS**/
            selection_direction--;
            break;
        case 3:
            selection_direction++;
            break;
        case 6:
            self->note.legato--;
            if (self->note.legato < 1)
                self->note.legato = 1;
            break;
        case 8:
            self->note.legato++;
            if (self->note.legato > self->note.length)
                self->note.legato = self->note.length;
            break;
        case 10:
            set_rest_step(self->sequencer, self->note.length);
            step_forward(self->sequencer);
            break;
        case 12:
            self->note.octave++;
            keyboard_press = 0;
            if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                    .type != NEXT_NOTE)
            {
                keyboard_step_value(&self->note, keyboard_press);
                set_note(self->sequencer, &self->sequencer->sequencers[self->sequencer->active_sequence], self->sequencer->sequencers[self->sequencer->active_sequence].selected_step, self->note);
            }
            else
            {
                keyboard_step_value(&self->note, keyboard_press);
                add_note(self->sequencer, self->note);
            }
            self->note.octave--;
            break;
        default:
            if (self->sequencer->sequencers[self->sequencer->active_sequence].note_value[self->sequencer->sequencers
                [self->sequencer->active_sequence].current_page][self->sequencer->sequencers[self->sequencer->active_sequence].selected_step]
                    .type != NEXT_NOTE)
            {
                keyboard_step_value(&self->note, keyboard_press);
                set_note(self->sequencer, &self->sequencer->sequencers[self->sequencer->active_sequence], self->sequencer->sequencers[self->sequencer->active_sequence].selected_step, self->note);
            }
            else
            {
                keyboard_step_value(&self->note, keyboard_press);
                add_note(self->sequencer, self->note);
            }
        }
    }

    if (selection_direction != 0)
    {
        self->selection_state += selection_direction;
        if (self->selection_state > 9)
            self->selection_state = 0;
        else if (self->selection_state < 0)
            self->selection_state = 9;
        self->note.length = note_length_map[self->selection_state];
        self->note.legato = self->note.length - staccato;
    }
}

void update_state_main(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction)
{
    int8_t selection_direction = 0;
    if (button_press != 0 && !is_input(button_press))
    {
        switch (button_press >> 1)
        {
        case BUTTON_LEFT:
            self->selection_state--;
            if (self->selection_state < 0)
                self->selection_state = 6;
            break;
        case (BUTTON_RIGHT):
            if (self->shift)
                sequencer_pause_play(self->sequencer);
            else
            {
                self->selection_state++;
                if (self->selection_state > 6)
                    self->selection_state = 0;
            }
            break;
        case (BUTTON_UP):
            selection_direction = 1;
            break;
        case (BUTTON_DOWN):
            selection_direction = -1;
            break;
        case (BUTTON_OK):
            if (self->shift)
                self->menu_state = MAIN_PLAYING;
            else
                switch (self->selection_state)
                {
                case 0: // Open the main menu
                    self->selection_state = 0;
                    self->menu_state = MENU_OPEN;
                    break;
                case 3: // notes selected
                    go_to_edit(self);
                    break;
                case 5:
                    sequence_on_off(self->sequencer);
                    break;
                default:
                    break;
                }
            break;
        }
    }
    if (encoder_direction != 0)
        selection_direction = encoder_direction;
    if (selection_direction != 0)
        switch (self->selection_state)
        {
        case 0: // MENU
            /*** TODO ROLL OUT THE MENU AND SELECT THE FIRST ITEM ***/
            break;
        case 1: // KEY
            select_key(self->sequencer, selection_direction);
            break;
        case 2: // SCALE
            select_scale(self->sequencer, selection_direction);
            break;
        case 3: // NOTES nop
            break;
        case 4: // PAGE
            select_page(self->sequencer, selection_direction);
            break;
        case 5: // SEQUENCER
            select_active_sequencer(self->sequencer, selection_direction);
            break;
        case 6: // TEMPO
            if (self->shift)
                set_tempo(self->sequencer, selection_direction * 5);
            else
                set_tempo(self->sequencer, selection_direction);
            break;
        }
}

void update_state_generation(controller_t *self, uint16_t button_press, int8_t encoder_direction)
{
    if (button_press != 0)
        switch (button_press >> 1)
        {
        case BUTTON_LEFT:
            // SHIFT LEFT BUTTON ACTS AS A BACK BUTTON, WILL BE ADDED IN NEXT ITERATION OF THE HARDWARE
            if (self->shift && self->selection_state == 0)
                self->menu_state = MAIN_PLAYING;
            else if (self->shift && self->selection_state != 0)
                self->selection_state = 0;
            break;
        case (BUTTON_RIGHT):
            break;
        case (BUTTON_OK):
            break;
        }
}
/*** TODO ***/
void update_state_song(controller_t *self, uint16_t button_press)
{
    switch (button_press >> 1)
    {
    case BUTTON_LEFT:
        // SHIFT LEFT BUTTON ACTS AS A BACK BUTTON, WILL BE ADDED IN NEXT ITERATION OF THE HARDWARE
        if (self->shift && self->selection_state == 0)
            self->menu_state = MAIN_PLAYING;
        else if (self->shift && self->selection_state != 0)
            self->selection_state = 0;
        break;
    case (BUTTON_RIGHT):
        break;
    case (BUTTON_OK):
        break;
    }
}
/*** TODO ***/
void update_state_sequencers(controller_t *self, uint16_t button_press)
{
    switch (button_press >> 1)
    {
    case BUTTON_LEFT:
        // SHIFT LEFT BUTTON ACTS AS A BACK BUTTON, WILL BE ADDED IN NEXT ITERATION OF THE HARDWARE
        if (self->shift && self->selection_state == 0)
            self->menu_state = MAIN_PLAYING;
        else if (self->shift && self->selection_state != 0)
            self->selection_state = 0;
        break;
    case (BUTTON_RIGHT):
        break;
    case (BUTTON_OK):
        break;
    }
}
/*** TODO ***/
void update_state_settings(controller_t *self, uint16_t button_press)
{
    switch (button_press >> 1)
    {
    case BUTTON_LEFT:
        // SHIFT LEFT BUTTON ACTS AS A BACK BUTTON, WILL BE ADDED IN NEXT ITERATION OF THE HARDWARE
        if (self->shift && self->selection_state == 0)
            self->menu_state = MAIN_PLAYING;
        else if (self->shift && self->selection_state != 0)
            self->selection_state = 0;
        break;
    case (BUTTON_RIGHT):
        break;
    case (BUTTON_OK):
        break;
    }
}

/*** TODO ***/
void update_state_save(controller_t *self, uint16_t button_press)
{
    switch (button_press >> 1)
    {
    case BUTTON_LEFT:
        // SHIFT LEFT BUTTON ACTS AS A BACK BUTTON, WILL BE ADDED IN NEXT ITERATION OF THE HARDWARE
        if (self->shift && self->selection_state == 0)
            self->menu_state = MAIN_PLAYING;
        else if (self->shift && self->selection_state != 0)
            self->selection_state = 0;
        break;
    case (BUTTON_RIGHT):

        break;
    case (BUTTON_OK):
        break;
    }
}

void go_to_edit(controller_t *self)
{
    self->menu_state = EDIT_MENU; // EDIT NOTES MENU
    self->selection_state = 5;
    self->keyboard_active = 3;
}