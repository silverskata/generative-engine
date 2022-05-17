#include "../include/dac.h"

spi_inst_t *spi = spi0;

void dac_init(){
    //INIT AND SET ALL CS PINS TO HIGH
        gpio_init(CS0);
        gpio_set_dir(CS0, GPIO_OUT);
        gpio_put(CS0, 1);
        gpio_init(CS1);
        gpio_set_dir(CS1, GPIO_OUT);
        gpio_put(CS1, 1);
        gpio_init(CS2);
        gpio_set_dir(CS2, GPIO_OUT);
        gpio_put(CS2, 1);

    //INIT SPI port
    
    spi_init(spi, 10000000); //Transfer speed 20 MHz

    spi_set_format( spi0,   //Instance
                    8,     //Bits per transfer
                    0,      //Polarity
                    0,      //Phase
                    SPI_MSB_FIRST);

    //Set SPI Pins
    gpio_set_function(18, GPIO_FUNC_SPI);   //SCK
    gpio_set_function(19, GPIO_FUNC_SPI);   //MOSI
    }


void reg_write(spi_inst_t *spi, const uint8_t cs, const uint8_t control, const uint16_t data){
    uint8_t message[2]; // data buffer

    message[1] = data & 0x00FF;
    message[0] = control << 4 | ((data & 0x0F00) >> 8);
    
    gpio_put(cs,0);
    spi_write_blocking(spi, message ,2);
    //For some reason the spi_write does not wait for the clock to go low until proceeding, so we wait until it does
    while(gpio_get(18));    
    gpio_put(cs,1);
}

    void set_voltage(uint8_t channel, uint16_t value){
        switch(channel){
            case DAC_PITCH1:
                reg_write(spi, CS0, 0X1, value);
                break;
            case DAC_PITCH2:
                reg_write(spi, CS0, 0X9, value);
                break;
            case DAC_PITCH3:
                reg_write(spi, CS1, 0X1, value);
                break;            
            case DAC_CV1:
                reg_write(spi, CS1, 0X9, value);
                break;                                    
            case DAC_CV2:
                reg_write(spi, CS2, 0X1, value);
                break;                    
            case DAC_CV3:
                reg_write(spi, CS2, 0X9, value);
                break;                                    
        }
    }