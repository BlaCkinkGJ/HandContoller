#ifndef __UART_H__
#define __UART_H__

#include <includes.h>

void UART_CNF();
void UART_SendData(USART_TypeDef* USARTx, u16 Data);

#endif
