#ifndef __ST7789_H
#define __ST7789_H


#include "stc8g.h"

sbit TFT_BL = P3^4;   // 液晶屏背光灯控制引脚
sbit LCD_CS = P5^0;   // 片选引脚
sbit LCD_RS = P5^1;   // 
sbit LCD_WR = P4^2; 
sbit LCD_RD = P4^4;

#define     RED          0XF800	  //红色
#define     GREEN        0X07E0	  //绿色
#define     YELLOW       0XFFE0   //黄色
#define     BLUE         0X001F	  //蓝色
#define     WHITE        0XFFFF	  //白色
#define     BLACK        0X0000   //黑色


void LCD_Init(void);
void LCD_Clear(unsigned int color);
void LCD_DrawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_DrawRectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
//void LCD_ShowFont(unsigned int x, unsigned int y, unsigned char num);
void LCD_Fill(unsigned int stx, unsigned int sty, unsigned int endx, unsigned int endy, unsigned int color);

#endif



