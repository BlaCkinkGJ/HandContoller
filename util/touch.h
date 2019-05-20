#ifndef TOUCH_H
#define TOUCH_H

#include <cpu.h>
#include <stm32f10x_lib.h>

extern float xfac;
extern float yfac;
extern short xoff;
extern short yoff;

#define T_DCLK_L GPIOC->BRR = 1 << 10
#define T_DCLK_H GPIOC->BSRR = 1 << 10

#define T_CS_L GPIOC->BRR = 1 << 8
#define T_CS_H GPIOC->BSRR = 1 << 8

#define T_DOUT_L GPIOC->BRR = 1 << 12
#define T_DOUT_H GPIOC->BSRR = 1 << 12

#define T_DIN (1 & ((GPIOC->IDR) >> 11))

#define T_INT (1 & ((GPIOC->IDR) >> 5))

void ADS_Write_Byte(CPU_INT08U num);
CPU_INT16U ADS_Read_AD(CPU_INT08U CMD);
void Touch_Configuration(void);
void Draw_Big_Point(CPU_INT16U x, CPU_INT16U y);
CPU_INT08U Touch_GexX(CPU_INT16U* y, CPU_INT08U ext);
CPU_INT08U Touch_GexY(CPU_INT16U* y, CPU_INT08U ext);
void Touch_GetXY(CPU_INT16U* x, CPU_INT16U* y, CPU_INT08U ext);
void Touch_Adjust(void);
void Convert_Pos(CPU_INT16U x_in, CPU_INT16U y_in, CPU_INT16U* x_out, CPU_INT16U* y_out);

#endif
