/*

MIT License

Copyright (c) 2021 David Schramm

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../include/ssd1306.h"
#include "../include/font.h"

char left_bracket[]={"["};
char right_bracket[]={"]"};
char quarterrest1[] ={(char)129};
char quarterrest2[] ={(char)130};
char eightrest[] ={(char)128};
char sixteenthrest[] ={(char)131};
char up_arrow[] ={(char)182};
char raised_symbol[]={183};
char lowered_symbol[]={184};


char logo_row1[]={132,133,134,135,136,137,138,139,140,141,0};
char logo_row2[]={142,143,144,145,146,147,148,149,150,151,0};
char logo_row3[]={152,153,154,155,156,157,158,159,160,161,0};
char logo_row4[]={162,163,164,165,166,167,168,169,170,171,0};
char logo_row5[]={172,173,174,175,176,177,178,179,180,181,0};

inline static void swap(uint32_t *a, uint32_t *b) {
    uint32_t *t=a;
    *a=*b;
    *b=*t;
}

inline static void private_write(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, char *name) {
    switch(i2c_write_blocking(i2c, addr, src, len, false)) {
    case PICO_ERROR_GENERIC:
        printf("[%s] addr not acknowledged!\n", name);
        break;
    case PICO_ERROR_TIMEOUT:
        printf("[%s] timeout!\n", name);
        break;
    default:
        break;
    }
}

void display_setup(i2c_inst_t *i2c){
    i2c_init(i2c, 400000);
    gpio_set_function(9, GPIO_FUNC_I2C);
    gpio_set_function(8, GPIO_FUNC_I2C);
    gpio_pull_up(8);
    gpio_pull_up(9);
}

inline static void display_write(ssd1306_t *p, uint8_t val) {
    uint8_t d[2]= {0x00, val};
    private_write(p->i2c_i, p->address, d, 2, "ssd1306_write");
}

bool display_init(ssd1306_t *p, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c_instance) {
    p->width=width;
    p->height=height;
    p->pages=height/8;
    p->address=address;

    p->i2c_i=i2c_instance;


    p->bufsize=(p->pages)*(p->width);
    if((p->buffer=malloc(p->bufsize+1))==NULL) {
        p->bufsize=0;
        return false;
    }

    ++(p->buffer);

    // from https://github.com/makerportal/rpi-pico-ssd1306
    int8_t cmds[]= {
        SET_DISP | 0x00,  // off
        // address setting
        SET_MEM_ADDR,
        0x00,  // horizontal
        // resolution and layout
        SET_DISP_START_LINE | 0x00,
        SET_SEG_REMAP | 0x01,  // column addr 127 mapped to SEG0
        SET_MUX_RATIO,
        height - 1,
        SET_COM_OUT_DIR | 0x08,  // scan from COM[N] to COM0
        SET_DISP_OFFSET,
        0x00,
        SET_COM_PIN_CFG,
        width>2*height?0x02:0x12,
        // timing and driving scheme
        SET_DISP_CLK_DIV,
        0x80,
        SET_PRECHARGE,
        p->external_vcc?0x22:0xF1,
        SET_VCOM_DESEL,
        0x30,  // 0.83*Vcc
        // display
        SET_CONTRAST,
        0xFF,  // maximum
        SET_ENTIRE_ON,  // output follows RAM contents
        SET_NORM_INV,  // not inverted
        // charge pump
        SET_CHARGE_PUMP,
        p->external_vcc?0x10:0x14,
        SET_DISP | 0x01
    };

    for(size_t i=0; i<sizeof(cmds); ++i)
        display_write(p, cmds[i]);

    return true;
}


inline void ssd1306_deinit(ssd1306_t *p) {
    free(p->buffer-1);
}

inline void ssd1306_poweroff(ssd1306_t *p) {
    display_write(p, SET_DISP|0x00);
}

inline void ssd1306_poweron(ssd1306_t *p) {
    display_write(p, SET_DISP|0x01);
}

inline void ssd1306_contrast(ssd1306_t *p, uint8_t val) {
    display_write(p, SET_CONTRAST);
    display_write(p, val);
}

inline void ssd1306_invert(ssd1306_t *p, uint8_t inv) {
    display_write(p, SET_NORM_INV | (inv & 1));
}

inline void ssd1306_clear(ssd1306_t *p) {
    memset(p->buffer, 0, p->bufsize);
}

void display_pixel(ssd1306_t *p, uint32_t x, uint32_t y) {
    if(x>=p->width || y>=p->height) return;

    p->buffer[x+p->width*(y>>3)]|=0x1<<(y&0x07); // y>>3==y/8 && y&0x7==y%8
}


void display_line(ssd1306_t *p, int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    if(x1>x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    if(x1==x2) {
        if(y1>y2)
            swap(&y1, &y2);
        for(int32_t i=y1; i<=y2; ++i)
            display_pixel(p, x1, i);
        return;
    }

    float m=(float) (y2-y1) / (float) (x2-x1);

    for(int32_t i=x1; i<=x2; ++i) {
        float y=m*(float) (i-x1)+(float) y1;
        display_pixel(p, i, (uint32_t) y);
    }
}

void display_square(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for(uint32_t i=0; i<width; ++i)
        for(uint32_t j=0; j<height; ++j)
            display_pixel(p, x+i, y+j);

}
void display_black_square(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for(uint32_t i=0; i<width; ++i)
        for(uint32_t j=0; j<height; ++j)
            display_pixel2(p, x+i, y+j);

}
void display_box(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    display_line(p, x, y, x+width, y);
    display_line(p, x, y+height, x+width, y+height);
    display_line(p, x, y, x, y+height);
    display_line(p, x+width, y, x+width, y+height);
}

void display_box_no_top(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    display_line(p, x, y+height, x+width, y+height);
    display_line(p, x, y, x, y+height);
    display_line(p, x+width, y, x+width, y+height);
}


void display_circle(ssd1306_t *p, uint8_t x0, uint8_t y0, uint8_t radius){
  int8_t x = 0, y = radius;
	int8_t dp = 1 - radius;
	do {
		if (dp < 0)
			dp = dp + (x++) * 2 + 3;
		else
			dp = dp + (x++) * 2 - (y--) * 2 + 5;

		display_pixel(p, x0 + x, y0 + y);  
		display_pixel(p, x0 - x, y0 + y);
		display_pixel(p, x0 + x, y0 - y);
		display_pixel(p, x0 - x, y0 - y);
		display_pixel(p, x0 + y, y0 + x);
		display_pixel(p, x0 - y, y0 + x);
		display_pixel(p, x0 + y, y0 - x);
		display_pixel(p, x0 - y, y0 - x);

	} while (x < y);

  display_pixel(p, x0 + radius, y0);
  display_pixel(p, x0, y0 + radius);
  display_pixel(p, x0 - radius, y0);
  display_pixel(p, x0, y0 - radius);
}

void display_fill_circle(ssd1306_t *p, int8_t x0, int8_t y0, int8_t radius) {
  int8_t x = 0, y = radius;
	int8_t dp = 1 - radius;
	do {
		if (dp < 0)
      dp = dp + (x++) * 2 + 3;
    else
      dp = dp + (x++) * 2 - (y--) * 2 + 5;

    display_line(p, x0 - x, y0 - y, (x0 - x) + 2*x, y0 - y);
    display_line(p, x0 - x, y0 + y, (x0 - x) + 2*x, y0 + y);   
    display_line(p, x0 - y, y0 - x, (x0 - y) + 2*y, y0 - x);
    display_line(p, x0 - y, y0 + x, (x0 - y) + 2*y, y0 + x);


	} while (x < y);
 display_line(p, x0 - radius, y0, x0 - radius + 2 * radius, y0);

}

void display_char_font(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const uint8_t *font, char c) {
    if(c > 189)
        return;

    for(uint8_t i=0; i<font[1]; ++i) {
        uint8_t line=(uint8_t)(font[(c-0x20)*font[1]+i+2]);

        for(int8_t j=0; j<8; ++j, line>>=1) {
            if(line & 1 ==1)
                display_square(p, x+i*scale, y+j*scale, scale, scale);
        }
    }
}

void display_char_font2(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, const uint8_t *font, char c) {
    if(c > 181)
        return;

    for(uint8_t i=0; i<font[1]; ++i) {
        uint8_t line=(uint8_t)(font[(c-0x20)*font[1]+i+2]);

        for(int8_t j=0; j<8; ++j, line>>=1) {
            if(line & 1 ==1)
                display_black_square(p, x+i*scale, y+j*scale, scale, scale);
        }
    }
}

void display_pixel2(ssd1306_t *p, uint32_t x, uint32_t y) {
    if(x>=p->width || y>=p->height) return;

   p->buffer[x+p->width*(y>>3)]=0; // y>>3==y/8 && y&0x7==y%8
}

void display_string_test(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, char *s) {
    const uint8_t *font = font_8x5;
    for(int32_t x_n=x; *s; x_n+=8*scale) {
        display_char_font2(p, x_n, y, scale, font, *(s++));
    }
}

void display_string(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, char *s) {
    const uint8_t *font = font_8x5;
    for(int32_t x_n=x; *s; x_n+=8*scale) {
        display_char_font(p, x_n, y, scale, font, *(s++));
    }
}

void display_write_tight(ssd1306_t *p, uint32_t x, uint32_t y, int32_t scale, char *s) {
    const uint8_t *font = font_8x5;
    for(int32_t x_n=x; *s; x_n+=font[0]*1-scale) {
        display_char_font(p, x_n, y, 1, font, *(s++));
    }
}

static inline uint32_t ssd1306_bmp_get_val(const uint8_t *data, const size_t offset, uint8_t size) {
    switch(size) {
    case 1:
        return data[offset];
    case 2:
        return data[offset]|(data[offset+1]<<8);
    case 4:
        return data[offset]|(data[offset+1]<<8)|(data[offset+2]<<16)|(data[offset+3]<<24);
    default:
        __builtin_unreachable();
    }
    __builtin_unreachable();
}

void ssd1306_bmp_show_image_with_offset(ssd1306_t *p, const uint8_t *data, const long size, uint32_t x_offset, uint32_t y_offset) {
    if(size<54) // data smaller than header
        return;

    const uint32_t bfOffBits=ssd1306_bmp_get_val(data, 10, 4);
    const uint32_t biSize=ssd1306_bmp_get_val(data, 14, 4);
    const int32_t biWidth=(int32_t) ssd1306_bmp_get_val(data, 18, 4);
    const int32_t biHeight=(int32_t) ssd1306_bmp_get_val(data, 22, 4);
    const uint16_t biBitCount=(uint16_t) ssd1306_bmp_get_val(data, 28, 2);
    const uint32_t biCompression=ssd1306_bmp_get_val(data, 30, 4);

    if(biBitCount!=1) // image not monochrome
        return;

    if(biCompression!=0) // image compressed
        return;

    const int table_start=14+biSize;
    uint8_t color_val;

    for(uint8_t i=0; i<2; ++i) {
        if(!((data[table_start+i*4]<<16)|(data[table_start+i*4+1]<<8)|data[table_start+i*4+2])) {
            color_val=i;
            break;
        }
    }

    uint32_t bytes_per_line=(biWidth/8)+(biWidth&7?1:0);
    if(bytes_per_line&3)
        bytes_per_line=(bytes_per_line^(bytes_per_line&3))+4;

    const uint8_t *img_data=data+bfOffBits;

    int step=biHeight>0?-1:1;
    int border=biHeight>0?-1:biHeight;
    for(uint32_t y=biHeight>0?biHeight-1:0; y!=border; y+=step) {
        for(uint32_t x=0; x<biWidth; ++x) {
            if(((img_data[x>>3]>>(7-(x&7)))&1)==color_val)
                display_pixel(p, x_offset+x, y_offset+y);
        }
        img_data+=bytes_per_line;
    }
}

inline void ssd1306_bmp_show_image(ssd1306_t *p, const uint8_t *data, const long size) {
    ssd1306_bmp_show_image_with_offset(p, data, size, 0, 0);
}

void ssd1306_show(ssd1306_t *p) {
    uint8_t payload[]= {SET_COL_ADDR, 0, p->width-1, SET_PAGE_ADDR, 0, p->pages-1};
    if(p->width==64) {
        payload[1]+=32;
        payload[2]+=32;
    }

    for(size_t i=0; i<sizeof(payload); ++i)
        display_write(p, payload[i]);

    *(p->buffer-1)=0x40;

    private_write(p->i2c_i, p->address, p->buffer-1, p->bufsize+1, "ssd1306_show");
}


void draw_selector(ssd1306_t *disp, int16_t x,int16_t y,int16_t space){
    display_string(disp,x-2,y,2, left_bracket);
    display_string(disp,x-2+space,y,2, right_bracket); 
}

//MUSICAL NOTES
void draw_4_note(ssd1306_t *disp, int16_t x,int16_t y){
        display_fill_circle(disp,x,y,2);
        display_line(disp,x+2,y-7,x+2,y-2);
    }
void draw_down_4_note(ssd1306_t *disp, int16_t x,int16_t y){
        display_fill_circle(disp,x,y,2);
        display_line(disp, x-2, y+1, x-2 , y+9);
    }
void draw_8_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_4_note(disp,x,y);
        display_line(disp,x+2,y-8,x+4,y-6);
}

void draw_down_8_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_down_4_note(disp, x, y);
        display_line(disp,x-5,y+8,x-2,y+10);
}

void draw_16_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_8_note(disp,x,y);
        display_line(disp,x+2,y-5,x+4,y-3);
}

void draw_32_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_16_note(disp,x,y);
        display_line(disp,x+2,y-2,x+4,y-1);
}
void draw_down_32_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_down_16_note(disp,x,y);
        display_line(disp,x-5,y+2,x-2,y+4);
}
void draw_down_16_note(ssd1306_t *disp, int16_t x,int16_t y){
        draw_down_8_note(disp,x,y);
        display_line(disp,x-5,y+5,x-2,y+7);
}
void draw_seminote(ssd1306_t *disp, int16_t x,int16_t y){
        display_circle(disp,x,y,2);
        display_line(disp,x+2,y-7,x+2,y-2);
    }
void draw_down_seminote(ssd1306_t *disp, int16_t x,int16_t y){
        display_circle(disp,x,y,2);
        display_line(disp, x-2, y+1, x-2 , y+9);
    }
void draw_wholenote(ssd1306_t *disp, int16_t x,int16_t y){
        display_circle(disp,x,y,2);
}
void draw_dotted(ssd1306_t *disp, int16_t x,int16_t y){
    display_fill_circle(disp, x+5, y+1,1);
}

void draw_up_octave(ssd1306_t *disp, int16_t x,int16_t y, uint8_t amount){
    for(uint8_t i = 0; i < amount;i++){
    display_string(disp,x-3,y-12-4*i,1,up_arrow);
    }
}

void draw_raised(ssd1306_t *disp, int16_t x,int16_t y){

    display_string(disp,x,y,1,raised_symbol);
}

void draw_lowered(ssd1306_t *disp, int16_t x,int16_t y){

    display_string(disp,x,y,1,lowered_symbol);
}



//RESTS

void draw_wholerest(ssd1306_t *disp, int16_t x){
    uint8_t y= 33;
    display_line(disp,x-2,y-1,x+8,y-1);

        display_square(disp,x,y,7,4);
}

void draw_semirest(ssd1306_t *disp, int16_t x){
    uint8_t y= 32;
        display_line(disp,x-2,y+4,x+8,y+4);
        display_square(disp,x,y,7,4);
}

void draw_4_rest(ssd1306_t *disp, int16_t x){
        display_string(disp,x,27,1,quarterrest1);
        display_string(disp,x,27+8,1,quarterrest2);
}

void draw_8_rest(ssd1306_t *disp, int16_t x){
        display_string(disp,x,31,1,eightrest);
}

void draw_16_rest(ssd1306_t *disp, int16_t x){
        display_string(disp,x,31,1,sixteenthrest);
}


void draw_logo(ssd1306_t *disp){
            for (uint8_t i = 0; i < 14; i++)
        {    
            ssd1306_clear(disp);
                display_write_tight(disp,15-13+i,0+i*2-10,-10+i,logo_row1);
                display_write_tight(disp,15-26+2*i,21+i-10,-10+i,logo_row2);
                display_write_tight(disp,15-6+i/2,48-i/2-10,-10+i,logo_row3);
                display_write_tight(disp,15-13+i,63-i-10,-10+i,logo_row4);
                display_write_tight(disp,15,63-i-2,-10+i,logo_row5);
            ssd1306_show(disp);
            sleep_ms(40-i);
        }
        sleep_ms(50);
        char bird[]={"bird electronics"};
        char gen[]={"GENERATIVE"};
        char en[]={"ENGINE"};
        char ver[]={"version 0.1"};
        display_write_tight(disp,5,0,1,gen);
                 ssd1306_show(disp);
        sleep_ms(400);
        display_write_tight(disp,78,6,1,en);
                 ssd1306_show(disp);
        sleep_ms(200);
  display_write_tight(disp,65,47,3,ver);
                 ssd1306_show(disp);
        sleep_ms(600);
        
        display_write_tight(disp,10,57,1,bird);
                     ssd1306_show(disp);
                 sleep_ms(500);
}