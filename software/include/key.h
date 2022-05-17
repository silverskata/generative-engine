#include <stdio.h>
#include <stdint.h>

#define C_VAL 0
#define C_SHARP_VAL 1
#define D_FLAT_VAL 1
#define D_VAL 2
#define D_SHARP_VAL 3
#define E_FLAT_VAL 3
#define E_VAL 4
#define E_SHARP_VAL 5
#define F_FLAT_VAL 4
#define F_VAL 5
#define F_SHARP_VAL 6
#define G_FLAT_VAL 6
#define G_VAL 7
#define G_SHARP_VAL 8
#define A_FLAT_VAL 8
#define A_VAL 9
#define A_SHARP_VAL 10
#define B_FLAT_VAL 10
#define B_VAL 11

typedef struct{
char name[3];
uint8_t number;
uint8_t value;
int8_t modulation[7];
}key;

typedef enum{
    KEY_C = 0, KEY_D, KEY_E, KEY_F, KEY_G, KEY_A, KEY_B, KEY_C_SHARP, KEY_D_SHARP, KEY_F_SHARP, KEY_G_SHARP, KEY_D_FLAT, KEY_E_FLAT, KEY_A_FLAT, KEY_B_FLAT,
}key_signatures;

static const uint8_t key_selector[] = {KEY_D_FLAT,KEY_A_FLAT,KEY_E_FLAT,KEY_B_FLAT,KEY_F,KEY_C,KEY_G,KEY_D,KEY_A,KEY_E,KEY_B,KEY_F_SHARP};

static const key keys[] = {
    {.name = "C", .number = KEY_C, .value=C_VAL, .modulation = {0, 0, 0, 0, 0, 0, 0}},
    {.name = "D", .number = KEY_D, .value=D_VAL, .modulation = {0, 0, 1, 0, 0, 0, 1}},
    {.name = "E", .number = KEY_E, .value=E_VAL, .modulation = {0, 1, 1, 0, 0, 1, 1}},
    {.name = "F", .number = KEY_F, .value=F_VAL, .modulation = {0, 0, 0, -1, 0, 0, 0}},
    {.name = "G", .number = KEY_G, .value=G_VAL, .modulation = {0, 0, 0, 0, 0, 0, 1}},
    {.name = "A", .number = KEY_A, .value=A_VAL, .modulation = {0, 0, 1, 0, 0, 1, 1}},
    {.name = "B", .number = KEY_B, .value=B_VAL, .modulation = {0, 1, 1, 0, 1, 1, 1}},
    {.name = "C#", .number = KEY_C_SHARP, .value=C_SHARP_VAL, .modulation = {1, 1, 1, 1, 1, 1, 1}},
    {.name = "D#", .number = KEY_D_SHARP, .value=D_SHARP_VAL, .modulation = {1, 1, 2, 1, 1, 0, 0}},
    {.name = "F#", .number = KEY_F_SHARP, .value=F_SHARP_VAL, .modulation = {1, 1, 1, 0, 1, 1, 1}},
    {.name = "G#", .number = KEY_G_SHARP, .value=G_SHARP_VAL, .modulation = {1, 1, 0, 1, 1, 0, 0}},
    {.name = "Db", .number = KEY_D_FLAT, .value=D_FLAT_VAL, .modulation = {-1, -1, 0, -1, -1, -1, 0}},
    {.name = "Eb", .number = KEY_E_FLAT,.value=E_FLAT_VAL, .modulation = {-1, -1, 0, -1, -1, -1, 0}},
    {.name = "Ab", .number = KEY_A_FLAT, .value=A_FLAT_VAL, .modulation = {-1, -1, 0, -1, -1, -1, 0}},
    {.name = "Bb", .number = KEY_B_FLAT, .value=B_FLAT_VAL, .modulation = {-1, -1, 0, -1, -1, -1, 0}},
};

static char key_list[15][3] = {
    {"C"}, {"D"}, {"E"}, {"F"},
    {"G"}, {"A"}, {"B"}, {"C#"},
    {"D#"}, {"F#"}, {"G#"},{"Db"}, 
    {"Eb"}, {"Ab"}, {"Bb"}};