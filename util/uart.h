#ifndef __UART_H__
#define __UART_H__

#include <stm32f10x_lib.h>

void UART_CNF();
void UART_SendData(USART_TypeDef* USARTx, u16 Data);

#endif
