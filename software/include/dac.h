#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

/*******************           DAC            **********************/

// Registers
#define REG_AB 0x8000
#define REG_GAIN 0x4000
#define REG_SHUTDOWN 0x1000
#define REG_DATA 0x0FFF

// Channels
#define DAC_PITCH1  1
#define DAC_PITCH2  2
#define DAC_PITCH3  3
#define DAC_CV1     4
#define DAC_CV2     5
#define DAC_CV3     6
//Chip select
#define CS0 22
#define CS1 20
#define CS2 21

//Used SPI instance

/**
*	@brief initialize dac
*/
void dac_init();

/**
*	@brief write to the MCP4822 register.
*
*	@param[in] spi : pointer to the spi_port
*	@param[in] cs : width of display
*	@param[in] control : 4 control bits of the dac
*	@param[in] data : 12 bits containing the value to be output
*	
* 	@return void.
*/
void reg_write(spi_inst_t *spi, const uint8_t cs, const uint8_t control, const uint16_t data);

/**
*	@brief sets the voltage of a channel
*
*	@param[in] channel : selects output channel 1-6
*	@param[in] value : sets the value from 0 to 3300 with 2x gain
*	
* 	@return void.
*/
void set_voltage(uint8_t channel, uint16_t value);
