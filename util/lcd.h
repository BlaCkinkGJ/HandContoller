#ifndef __LCD_H__
#define __LCD_H__

//#include "hw_config.h"
#include <cpu.h>
#include <stdlib.h>
#include <stm32f10x_lib.h>

#define DataToWrite(DATA) GPIO_Write(GPIOE, DATA)
#define DataToRead() GPIO_ReadOutputData(GPIOE)

/*Private define-------------------------*/
#define POINT_COLOR BLACK
#define BACK_COLOR 0xFFFF
/*--------Color---------------*/
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define LGRAY 0XC618

#define __NOP() __asm volatile("NOP")

void LCD_Clear(CPU_INT16U Color);

void LCD_Fill(CPU_INT08U xsta, CPU_INT16U ysta, CPU_INT08U xend, CPU_INT16U yend, CPU_INT16U colour);
void LCD_DrawLine(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend);
void Draw_Circle(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U r);
void LCD_DrawRectangle(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend);
void LCD_DrawPoint(CPU_INT16U xsta, CPU_INT16U ysta);
void LCD_WindowMax(unsigned int x, unsigned int y, unsigned int x_end, unsigned int y_end);
void Delay_10ms(int time);
void LCD_ShowString(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U* pcStr, CPU_INT16U PenColor, CPU_INT16U BackColor);

void LCD_Configuration(void);
void LCD_FSMCConfiguration(void);
void LCD_Init(void);

#endif
