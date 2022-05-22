#include "stdint.h"

#define GATE1 15
#define GATE2 14
#define GATE3 13
#define GATE_ON 0
#define GATE_OFF 1

void output_setup();

void flash_led(uint8_t times, uint32_t duration_ms);