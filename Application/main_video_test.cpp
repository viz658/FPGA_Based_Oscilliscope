/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "xadc_core.h"
#include "ps2_core.h"
#include <cstdio>


// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));

XadcCore xadc(get_slot_addr(BRIDGE_BASE,S5_XDAC));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE,S11_PS2));
FrameCore frame(FRAME_BASE);
#define X_SIZE 640
#define Y_SIZE 480
#define V_BOX 60
#define BLUE 0x014 //blue
#define COLOR1 0x100 //red
#define COLOR2 511 //white
#define COLOR3 0x020 //light green
#define TOLERANCE 15

//int map_y(int x1, int x2, int y1, int y2, int x_pos)
//{
//    int slope = (y2 - y1) / (x2 - x1);
//    int y_pos = ((x_pos - x1) * slope) + y1;
//    return y_pos;
//}

int map_y(int y1, int y2, double y_pos)
{
    int y_pixel = (y2-y1) * y_pos;
    return y_pixel;
}

int map_x(int x1, int x2, int y1, int y2, int y_pos)
{
    int slope = (y2 - y1) / (x2 - x1);
    int x_pos = ((y_pos - y1) / slope);
    return x_pos;
}

void osd_write_voltage(int ypx, int channel, double vpp) {
  osd.set_color(0x0f0, 0x000); //green text
  uint8_t osd_x = (X_SIZE - V_BOX) / 8;
  uint8_t osd_y = ypx / 16;
  osd.wr_char(osd_x + 1, osd_y, 'V', 0);
  osd.wr_char(osd_x + 2, osd_y, ':', 0);

//  double adc_val = xadc.read_adc_in(channel);
  char voltage_str[8];
  // not sure about using cstdio, might use too much memory, works tho
  snprintf(voltage_str, sizeof(voltage_str), "%.2f", vpp);

  for (int i = 0; voltage_str[i] != '\0'; ++i) {
    osd.wr_char(osd_x + 4 + i, osd_y, voltage_str[i], 0);
  }
}

int main() {
	mouse.bypass(1);
	ghost.bypass(1);
	bar.bypass(1);
	gray.bypass(1);
	frame.bypass(1);
	osd.bypass(1);
	int prev = 0;
	int xsamples[] = {75,150,300,X_SIZE - V_BOX};
	int colors[] = {BLUE, COLOR1, COLOR2, COLOR3};
	double adc_val[4];
	bool enable[4] = {true};
	int val = 0;
	unsigned char input_kb;

	int x_div = (X_SIZE - V_BOX) / xsamples[val];
	int num_selected = 0;
	int y_div = Y_SIZE / num_selected;

    int current_x = 0;
    int current_y = 0;
    int y1 = current_y;
    int y2 = current_y + y_div;
    int x1 = current_x;
    int x2 = current_x + x_div;
    double min = 0;
    double max = 0;

   while (1) {

//	   frame_check(&frame);
	   frame.bypass(0);
	   osd.bypass(0);
//	   frame.clr_screen(COLOR);

	ps2.get_kb_ch((char*) &input_kb);

    switch(input_kb)
    {
        case '-':
            if(val != 0)
			    --val;
            break;

        case '+':
            if(val != 3)
                ++val;
            break;

        case '1':
            enable[0] ^= 1;
            frame.clr_screen(0,X_SIZE,0,Y_SIZE,0x008);
            osd.clr_screen();
            break;

        case '2':
            enable[1] ^= 1;
            // just eliminate all of 1 color and it should work
            frame.clr_screen(0,X_SIZE,0,Y_SIZE,0x008);
            osd.clr_screen();
            break;

        case '3':
            enable[2] ^= 1;
            frame.clr_screen(0,X_SIZE,0,Y_SIZE,0x008);
            osd.clr_screen();
            break;

        case '4':
            enable[3] ^= 1;
            frame.clr_screen(0,X_SIZE,0,Y_SIZE,0x008);
            osd.clr_screen();
            break;

        case 'P':
        case 'p':
            input_kb = 0;
            while(1){
            	ps2.get_kb_ch((char*) &input_kb);
            	if(input_kb == 'p') {
            		break;
            	}
            }
//            while(input_kb != 'P' || input_kb != 'p') ps2.get_kb_ch((char*) &input_kb);
			break;

    }
    input_kb = 0;
    num_selected = 0;
	for(int i = 0; i < 4; ++i)
	{
		if(enable[i] == true)
			++num_selected;
		led.write(enable[i], i);
	}
	x_div = (X_SIZE - V_BOX) / xsamples[val];
	y_div = Y_SIZE / num_selected;

	current_y = 0;

	for(int i = 0; i < 4; i++){

		if(enable[i]) {
		y1 = current_y;
		y2 = current_y + y_div;
		current_y = y2;
		}
		else continue;
		uart.disp(current_y);
		uart.disp("\n\r");
		current_x = 0;

		if(current_y == 480)
			current_y = --y2;

		min = xadc.read_adc_in(i);
		max = min;
		while(current_x < X_SIZE - V_BOX)
        {
			x1 = current_x;
			x2 = current_x + x_div;
			current_x = x2;
			if(enable[i])
            {
				adc_val[i] = xadc.read_adc_in(i);
//				int ypos = map_y(x1,x2,0,1,adc_val[i]);
				int ypos = map_y(y1,y2,adc_val[i]);
				frame.clr_screen(x1,x2,y1,y2,0x008);
				frame.plot_line(x1,ypos +y1,x2, ypos + y1, colors[i]);

//				frame.plot_line(x2,prev + y1,x2, ypos + y1, colors[i]);
//                if(prev > (ypos - TOLERANCE) && prev < (ypos + TOLERANCE))
//                {
//                    frame.plot_line(x1,prev + y1,x2, ypos + y1, colors[i]);
//                }
//                else
//                {
//                    frame.plot_line(x1,prev + y1,x2, prev + y1, colors[i]);
//                }
                prev = ypos;

                if(adc_val[i] > max)
                	max = adc_val[i];
                if(adc_val[i] < min)
                	min = adc_val[i];
                double vpp = max - min;
                osd_write_voltage(y1, i, vpp);
			}
		}


	}
   } // while
} //main
