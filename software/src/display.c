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

#include "../includes/display.h"

const uint8_t font_8x5[] =
{
			8, 5, //height, width
			0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x5F, 0x00, 0x00,
			0x00, 0x07, 0x00, 0x07, 0x00,
			0x14, 0x7F, 0x14, 0x7F, 0x14,
			0x24, 0x2A, 0x7F, 0x2A, 0x12,
			0x23, 0x13, 0x08, 0x64, 0x62,
			0x36, 0x49, 0x56, 0x20, 0x50,
			0x00, 0x08, 0x07, 0x03, 0x00,
			0x00, 0x1C, 0x22, 0x41, 0x00,
			0x00, 0x41, 0x22, 0x1C, 0x00,
			0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
			0x08, 0x08, 0x3E, 0x08, 0x08,
			0x00, 0x80, 0x70, 0x30, 0x00,
			0x08, 0x08, 0x08, 0x08, 0x08,
			0x00, 0x00, 0x60, 0x60, 0x00,
			0x20, 0x10, 0x08, 0x04, 0x02,
			0x3E, 0x51, 0x49, 0x45, 0x3E,
			0x00, 0x42, 0x7F, 0x40, 0x00,
			0x72, 0x49, 0x49, 0x49, 0x46,
			0x21, 0x41, 0x49, 0x4D, 0x33,
			0x18, 0x14, 0x12, 0x7F, 0x10,
			0x27, 0x45, 0x45, 0x45, 0x39,
			0x3C, 0x4A, 0x49, 0x49, 0x31,
			0x41, 0x21, 0x11, 0x09, 0x07,
			0x36, 0x49, 0x49, 0x49, 0x36,
			0x46, 0x49, 0x49, 0x29, 0x1E,
			0x00, 0x00, 0x14, 0x00, 0x00,
			0x00, 0x40, 0x34, 0x00, 0x00,
			0x00, 0x08, 0x14, 0x22, 0x41,
			0x14, 0x14, 0x14, 0x14, 0x14,
			0x00, 0x41, 0x22, 0x14, 0x08,
			0x02, 0x01, 0x59, 0x09, 0x06,
			0x3E, 0x41, 0x5D, 0x59, 0x4E,
			0x7C, 0x12, 0x11, 0x12, 0x7C,
			0x7F, 0x49, 0x49, 0x49, 0x36,
			0x3E, 0x41, 0x41, 0x41, 0x22,
			0x7F, 0x41, 0x41, 0x41, 0x3E,
			0x7F, 0x49, 0x49, 0x49, 0x41,
			0x7F, 0x09, 0x09, 0x09, 0x01,
			0x3E, 0x41, 0x41, 0x51, 0x73,
			0x7F, 0x08, 0x08, 0x08, 0x7F,
			0x00, 0x41, 0x7F, 0x41, 0x00,
			0x20, 0x40, 0x41, 0x3F, 0x01,
			0x7F, 0x08, 0x14, 0x22, 0x41,
			0x7F, 0x40, 0x40, 0x40, 0x40,
			0x7F, 0x02, 0x1C, 0x02, 0x7F,
			0x7F, 0x04, 0x08, 0x10, 0x7F,
			0x3E, 0x41, 0x41, 0x41, 0x3E,
			0x7F, 0x09, 0x09, 0x09, 0x06,
			0x3E, 0x41, 0x51, 0x21, 0x5E,
			0x7F, 0x09, 0x19, 0x29, 0x46,
			0x26, 0x49, 0x49, 0x49, 0x32,
			0x03, 0x01, 0x7F, 0x01, 0x03,
			0x3F, 0x40, 0x40, 0x40, 0x3F,
			0x1F, 0x20, 0x40, 0x20, 0x1F,
			0x3F, 0x40, 0x38, 0x40, 0x3F,
			0x63, 0x14, 0x08, 0x14, 0x63,
			0x03, 0x04, 0x78, 0x04, 0x03,
			0x61, 0x59, 0x49, 0x4D, 0x43,
			0x00, 0x7F, 0x41, 0x41, 0x41,
			0x02, 0x04, 0x08, 0x10, 0x20,
			0x00, 0x41, 0x41, 0x41, 0x7F,
			0x04, 0x02, 0x01, 0x02, 0x04,
			0x40, 0x40, 0x40, 0x40, 0x40,
			0x00, 0x03, 0x07, 0x08, 0x00,
			0x20, 0x54, 0x54, 0x78, 0x40,
			0x7F, 0x28, 0x44, 0x44, 0x38,
			0x38, 0x44, 0x44, 0x44, 0x28,
			0x38, 0x44, 0x44, 0x28, 0x7F,
			0x38, 0x54, 0x54, 0x54, 0x18,
			0x00, 0x08, 0x7E, 0x09, 0x02,
			0x18, 0xA4, 0xA4, 0x9C, 0x78,
			0x7F, 0x08, 0x04, 0x04, 0x78,
			0x00, 0x44, 0x7D, 0x40, 0x00,
			0x20, 0x40, 0x40, 0x3D, 0x00,
			0x7F, 0x10, 0x28, 0x44, 0x00,
			0x00, 0x41, 0x7F, 0x40, 0x00,
			0x7C, 0x04, 0x78, 0x04, 0x78,
			0x7C, 0x08, 0x04, 0x04, 0x78,
			0x38, 0x44, 0x44, 0x44, 0x38,
			0xFC, 0x18, 0x24, 0x24, 0x18,
			0x18, 0x24, 0x24, 0x18, 0xFC,
			0x7C, 0x08, 0x04, 0x04, 0x08,
			0x48, 0x54, 0x54, 0x54, 0x24,
			0x04, 0x04, 0x3F, 0x44, 0x24,
			0x3C, 0x40, 0x40, 0x20, 0x7C,
			0x1C, 0x20, 0x40, 0x20, 0x1C,
			0x3C, 0x40, 0x30, 0x40, 0x3C,
			0x44, 0x28, 0x10, 0x28, 0x44,
			0x4C, 0x90, 0x90, 0x90, 0x7C,
			0x44, 0x64, 0x54, 0x4C, 0x44,
			0x00, 0x08, 0x36, 0x41, 0x00,
			0x00, 0x00, 0x77, 0x00, 0x00,
			0x00, 0x41, 0x36, 0x08, 0x00,
			0x02, 0x01, 0x02, 0x04, 0x02,
};


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

void display_box(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    display_line(p, x, y, x+width, y);
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
    if(c > '~')
        return;

    for(uint8_t i=0; i<font[1]; ++i) {
        uint8_t line=(uint8_t)(font[(c-0x20)*font[1]+i+2]);

        for(int8_t j=0; j<font[0]; ++j, line>>=1) {
            if(line & 1 ==1)
                display_square(p, x+i*scale, y+j*scale, scale, scale);
        }
    }
}

void display_string(ssd1306_t *p, uint32_t x, uint32_t y, uint32_t scale, char *s) {
    const uint8_t *font = font_8x5;
    for(int32_t x_n=x; *s; x_n+=font[0]*scale) {
        display_char_font(p, x_n, y, scale, font, *(s++));
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
    if(size<54) 
        return;

    const uint32_t bfOffBits=ssd1306_bmp_get_val(data, 10, 4);
    const uint32_t biSize=ssd1306_bmp_get_val(data, 14, 4);
    const int32_t biWidth=(int32_t) ssd1306_bmp_get_val(data, 18, 4);
    const int32_t biHeight=(int32_t) ssd1306_bmp_get_val(data, 22, 4);
    const uint16_t biBitCount=(uint16_t) ssd1306_bmp_get_val(data, 28, 2);
    const uint32_t biCompression=ssd1306_bmp_get_val(data, 30, 4);

    if(biBitCount!=1) 
        return;

    if(biCompression!=0) 
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
}
