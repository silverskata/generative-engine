#include <stdio.h>
#include <stdint.h>

/**
 *	@brief Modal scale enumberation
 */
typedef enum
{
   IONIAN = 0,
   DORIAN,
   PHRYGIAN,
   LYDIAN,
   MIXOLYDIAN,
   AEOLIAN,
   LOCRIAN,
   CHROMATIC,
   USER1,
   USER2,
} scale_modes;

/**
 *	@brief Holds scale information
 */
typedef struct
{
   uint16_t hex;
   char name[16];
   int8_t tonal[12];
} scale;

static scale scales[] = {
    {0x0AB5,
     {"Ionian"},
     {0, 0, 0, 0, 0, 0, 0}},
    {0x06AD,
     {"Dorian"},
     {0, 0, -1, 0, 0, 0, -1}},
    {0x05AB,
     {"Phrygian"},
     {0, -1, -1, 0, 0, -1, -1}},
    {0x0AD5,
     {"Lydian"},
     {0, 0, 0, 1, 0, 0, 0}},
    {0x06B5,
     {"Mixolydian"},
     {0, 0, 0, 0, 0, 0, -1}},
    {0x05AD,
     {"Aeolian"},
     {0, 0, -1, 0, 0, -1, -1}},
    {0x056B,
     {"Locrian"},
     {0, -1, -1, 0, -1, -1, -1}},
    {0x0FFF,
     {"Chromatic"},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
    {0x0FFF,

     {"User1"}, /*** TODO LATER ADDITION ***/
     {1, 1, 1, 1, 1, 1, 1}},
    {0x0FFF,
     {"User2"},
     {1, 1, 1, 1, 1, 1, 1}}};

typedef struct
{
   char name[5][4];
} scale_modulation;

static const uint8_t default_scale[] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21};
static const uint8_t default_chromatic[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
