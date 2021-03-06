#ifndef UART_H
#define UART_H

#include <bsp.h>
#include <stm32f10x_lib.h>
#include <string.h>
#include <stdbool.h>

#define UART_STR_BUF 1024

void UART_NVIC_Init();
void UART_CNF();
void UART_SendStr(USART_TypeDef* UART, const char* data);

void USART1_IRQHandler();
void USART2_IRQHandler();

void scanSetup();
#endif
