#ifndef UART_H
#define UART_H

#include <includes.h>

void UART_CNF();
void UART_SendData(USART_TypeDef* USARTx, u16 Data);

#endif
