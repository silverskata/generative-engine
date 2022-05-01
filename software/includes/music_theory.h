#include <stdio.h>
#include <stdint.h>


//Notes and scales
#define IONIAN 0
#define DORIAN 1
#define PHRYGIAN 2
#define LYDIAN 3
#define MIXOLYDIAN 4
#define AEOLIAN 5
#define LOCRIAN 6
#define CHROMATIC 7
#define USER1 8
#define USER2 9


typedef enum{
    C = 0,
    D, 
    E, 
    F, 
    G,
    A,
    B,
    C_SHARP, 
    D_SHARP,
    F_SHARP = 10,
    G_SHARP,
    D_FLAT,
    E_FLAT,
    A_FLAT = 16,
    B_FLAT,
}key_signatures;

uint8_t key_selector[] = {D_FLAT,A_FLAT,E_FLAT,B_FLAT,F,C,G,D,A,E,B,F_SHARP};

typedef struct{
        uint16_t hex;
        char *name[16];
        int8_t tonal [12];
    }scale;


typedef struct{
char name[5][5];
}note;

typedef struct{
char name[3];
uint8_t number;
int8_t modulation[7];
}key;


