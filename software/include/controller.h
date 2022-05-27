#include "buttons.h"
#include "sequencer.h"
#include "encoder.h"
#include "ui.h"
#include <sys/queue.h>
#include "pico/util/queue.h"
#include "pico/multicore.h"

typedef enum
{
    MAIN_PLAYING = 0,
    MENU_CLOSED,
    EDIT_MENU,
    GENERATE_MENU,
    SONG_MENU,
    SAVE_MENU,
    SETTINGS_MENU,
} controller_states;

typedef enum
{
    MAIN_MENU_SELECTED,
    KEY_SELECTED,
    SCALE_SELECTED,
    NOTES_SELECTED,
    BAR_SELECTED,
    PAGE_SELECTED,
    SEQUENCER_SELECTED,
    TEMPO_SELECTED,
} main_selection;

typedef struct
{
    button_control_t *buttons;
    sequencer_t *sequencer;
    generator_t *gen_s1;
    generator_t *gen_s2;
    generator_t *gen_key;
    generator_t *gen_scale;
    generator_t *gen_harmony;
    bool menu_active;
    bool dynamic_generation;
    uint8_t menu_state;
    uint8_t old_state;
    int16_t selection_state;
    uint8_t keyboard_active; // 1 is off 3 is on.
    bool shift;
    note note;
} controller_t;

void control_task(controller_t *controller);

void init_controller(controller_t *control, button_control_t *buttons, sequencer_t *seq);

void dynamic_generation(controller_t * control);

void menu_active_state(controller_t *self, uint16_t button_press);

uint16_t read_control_queue(controller_t *self);

uint16_t read_keyboard_queue(controller_t *self);

bool is_input(uint16_t val);

void update_state(controller_t *self, uint16_t button_press);

bool update_shift(controller_t *self, uint16_t button_press);

void update_state_main(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction);

void update_state_playing(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction);

void update_state_edit(controller_t *self, uint16_t button_press, int16_t keyboard_press, int8_t encoder_direction);

void update_state_generation(controller_t *self, uint16_t button_press, int8_t encoder_direction);

void go_to_edit(controller_t *self);