#include "../include/buttons.h"

const uint8_t COl_MAP[] = {COL0, COL1, COL2, COL3};

const uint16_t keyboard_map[] = {BUTTON_C, BUTTON_C_SHARP, BUTTON_D, BUTTON_D_SHARP, BUTTON_E, BUTTON_F, BUTTON_F_SHARP, BUTTON_G, BUTTON_G_SHARP,
                                 BUTTON_A, BUTTON_B_FLAT, BUTTON_B, BUTTON_REST};

const uint16_t button_map[] = {LEFT, RIGHT, UP, DOWN, OK, SHIFT};

const uint16_t keyboard_value_map[] = {KEYBOARD_ONE, KEYBOARD_TWO, KEYBOARD_THREE, KEYBOARD_FOUR, KEYBOARD_FIVE, KEYBOARD_SIX, KEYBOARD_SEVEN,
                                       KEYBOARD_EIGHT, KEYBOARD_NINE, KEYBOARD_TEN, KEYBOARD_ELEVEN, KEYBOARD_TVELVE, KEYBOARD_THIRTEEN};

uint16_t queue_control_p[6];
uint16_t queue_keyboard_p[2];
uint8_t keys_pressed = 0;

void button_init(button_control_t *self)
{
    // MENU BUTTONS
    gpio_set_function(RIGHT, GPIO_FUNC_NULL);
    gpio_set_dir(RIGHT, false);
    gpio_pull_up(RIGHT);
    gpio_set_function(LEFT, GPIO_FUNC_NULL);
    gpio_set_dir(LEFT, false);
    gpio_pull_up(LEFT);
    gpio_set_function(DOWN, GPIO_FUNC_NULL);
    gpio_set_dir(DOWN, false);
    gpio_set_dir(UP, false);
    gpio_pull_up(UP);
    gpio_pull_up(DOWN);
    gpio_set_dir(OK, false);
    gpio_pull_up(OK);
    gpio_set_dir(SHIFT, false);
    gpio_pull_up(SHIFT);
    // KEYBOARD MATRIX
    gpio_set_dir(ROW0, false);
    gpio_set_dir(ROW1, false);
    gpio_set_dir(ROW2, false);
    gpio_set_dir(ROW3, false);
    gpio_init(COL0);
    gpio_set_dir(COL0, false);
    gpio_init(COL1);
    gpio_set_dir(COL1, false);
    gpio_init(COL2);
    gpio_set_dir(COL2, false);
    gpio_init(COL3);
    gpio_set_dir(COL3, false);

    queue_init(&self->controlqueue, sizeof(uint16_t), 10);
    queue_init(&self->keyboardqueue, sizeof(uint16_t), 6);

    for (uint8_t i = 0; i < 6; i++)
    {
        self->control_buttons[i] = false;
        self->control_buttons_flag[i] = false;
        self->keyboard_buttons[i / 3] = -1;
        self->keyboard_buttons_flag[i / 3] = false;
    }
}

void read_control_released(button_control_t *buttons)
{
    for (uint8_t i = 0; i < 6; i++)
    {
        if (buttons->control_buttons_flag[i] && !button_poll(i))
        {
            busy_wait_ms(2);
            if (buttons->control_buttons_flag[i] && !button_poll(i))
            {
                queue_control_p[i] = ((1 << i) << 1);
                queue_add_blocking(&buttons->controlqueue, &queue_control_p[i]);
                buttons->control_buttons_flag[i] = false;
            }
        }
    }
}

void read_keyboard_released(button_control_t *buttons)
{
    int16_t keys = matrix_poll();
    uint16_t keyhits[] = {0, 0};
    if (keys == -1)
    {
        for (uint8_t i = 0; i < 2; i++)
        {
            if (buttons->keyboard_buttons_flag[i])
            {
                buttons->keyboard_buttons_flag[i] = false;
                queue_keyboard_p[i] = (buttons->keyboard_buttons[i] << 1);
                buttons->keyboard_buttons[i] = -1;
                queue_add_blocking(&buttons->keyboardqueue, &queue_keyboard_p[i]);
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < 13; i++)
        {
            if ((keys >> i) & 1)
            {
                if (buttons->keyboard_buttons[0] == i)
                    keyhits[0] = 1;
                else if (buttons->keyboard_buttons[1] == i)
                    keyhits[1] = 1;
                else if (buttons->keyboard_buttons[2] == i)
                    keyhits[2] = 1;
            }
        }
        for (uint8_t i = 0; i < 2; i++)
        {
            if (!keyhits[i] && buttons->keyboard_buttons_flag[i])
            {
                buttons->keyboard_buttons_flag[i] = false;
                queue_keyboard_p[i] = (buttons->keyboard_buttons[i] << 1);
                buttons->keyboard_buttons[i] = -1;
                queue_add_blocking(&buttons->keyboardqueue, &queue_keyboard_p[i]);
            }
        }
    }
}

void read_control_buttons(button_control_t *buttons)
{
    if (queue_is_full(&buttons->controlqueue))
        return;
    bool check_buttons[6];
    for (uint8_t i = 0; i < 6; i++)
    {
        check_buttons[i] = button_poll(i);
    }
    uint8_t count = 0;
    for (uint8_t i = 0; i < 6; i++)
    {
        if (check_buttons[i])
            count++;
    }
    if (count == 0)
        return;
    busy_wait_ms(2);
    for (uint8_t i = 0; i < 6; i++)
    {
        if (check_buttons[i] == button_poll(i))
        {
            buttons->control_buttons[i] = check_buttons[i];
            if (!buttons->control_buttons_flag[i] && button_poll(i))
            {
                queue_control_p[i] = ((1 << i) << 1 | 1);
                queue_add_blocking(&buttons->controlqueue, &queue_control_p[i]);
                buttons->control_buttons_flag[i] = true;
            }
        }
        else
            buttons->control_buttons[i] = false;
    }
}

void read_keyboard(button_control_t *buttons)
{
    if (queue_is_full(&buttons->keyboardqueue))
        return;
    int8_t keypresses[2] = {-1, -1};
    uint8_t keycount = 0;
    int16_t keys = matrix_poll();
    if (keys == -1)
    {
        return;
    }
    for (uint8_t i = 0; i < 13; i++)
        if ((keys >> i) & 1)
        {
            keypresses[keycount] = i;
            keycount++;
            if (keycount > 1)
                break;
        }
    switch (keycount)
    {
    case 2:
        if (!buttons->keyboard_buttons_flag[1] && buttons->keyboard_buttons[1] == -1 && keypresses[1] != -1)
        {
            buttons->keyboard_buttons_flag[1] = true;
            if (keypresses[1] == buttons->keyboard_buttons[0])
                buttons->keyboard_buttons[1] = keypresses[0];
            else
                buttons->keyboard_buttons[1] = keypresses[1];
            queue_keyboard_p[1] = buttons->keyboard_buttons[1] << 1 | 1;
            queue_add_blocking(&buttons->keyboardqueue, &queue_keyboard_p[1]);
        }
    case 1:
        if (!buttons->keyboard_buttons_flag[0] && buttons->keyboard_buttons[0] == -1 && keypresses[0] != -1)
        {
            buttons->keyboard_buttons_flag[0] = true;
            buttons->keyboard_buttons[0] = keypresses[0];
            queue_keyboard_p[0] = keypresses[0] << 1 | 1;
            queue_add_blocking(&buttons->keyboardqueue, &queue_keyboard_p[0]);
        }
    }
}

bool button_poll(uint8_t button)
{
    return !gpio_get(button_map[button]);
}

int16_t matrix_poll()
{
    int16_t result = 0;
    for (uint8_t col = 0; col < 4; col++)
    {
        gpio_set_dir(COl_MAP[col], true);
        gpio_put(COl_MAP[col], 1);
        for (uint8_t row = 0; row < 4; row++)
        {
            if (gpio_get(row))
            {
                sleep_ms(2);
                if (gpio_get(row))
                {
                    for (uint8_t i = 0; i < 13; i++)
                    {
                        if ((int)((1 << row) << 4 | (1 << col)) == keyboard_map[i])
                            result |= 1 << i;
                    }
                }
            }
        }
        gpio_set_dir(COl_MAP[col], false);
    }
    if (result == 0)
        return -1;
    else
        return result;
};