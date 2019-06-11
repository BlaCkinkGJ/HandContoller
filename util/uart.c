/**
 * @file uart.c
 * @author BlaCkinkGJ (ss5kijun@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-05-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <uart.h>

static BSP_OS_SEM BSP_SerLock;

void USART1_IRQHandler()
{
    unsigned char recvBuf = '\0';

    // you have to change this section
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        ;
    recvBuf = (unsigned char)USART_ReceiveData(USART1);
    USART_SendData(USART1, recvBuf);
    USART_SendData(USART2, recvBuf);

    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

void USART2_IRQHandler()
{
    unsigned char recvBuf = '\0';

    // you have to change this section
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        ;
    recvBuf = (unsigned char)USART_ReceiveData(USART2);
    USART_SendData(USART1, recvBuf);
      
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}

void UART_NVIC_Init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // USART1 IRQ channel open
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // Don't care about priority in this time
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // USART2 IRQ channel open
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief UART configuration function
 * 
 */
void UART_CNF()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
            | RCC_APB2Periph_AFIO
            | RCC_APB2Periph_USART1,
        ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    // USART 1 Setting
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);

    // USART 2 Setting
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; // Tx
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    BSP_OS_SemCreate(&BSP_SerLock, 1, "Serial Lock");
}

void UART_SendStr(USART_TypeDef* UART, const char* data)
{
    OS_ERR err;
    u32 len = strlen(data);
    /**
     * Send data to bluetooth module
     * FB755AC Module uses AT command.
     * So if data meet the <CR>.
     * It means end of stream
     */
    BSP_OS_SemWait(&BSP_SerLock, 0);
    for (int idx = 0; idx < len; idx++) {
      while(1) {
        if (USART_GetFlagStatus(UART, USART_FLAG_TXE) != RESET){
          USART_SendData(UART, data[idx]);
          break;
        } else {
          OSTimeDlyHMSM(0, 0, 0, 1,
            OS_OPT_TIME_HMSM_STRICT,
            &err);
        }
      }
    } // end for
    BSP_OS_SemPost(&BSP_SerLock);
}

void scanSetup(){
  USART_SendData(USART2, 'A');
  USART_SendData(USART2, 'T');
  USART_SendData(USART2, '+');
  USART_SendData(USART2, 'B');
  USART_SendData(USART2, 'T');
  USART_SendData(USART2, 'S');
  USART_SendData(USART2, 'C');
  USART_SendData(USART2, 'A');
  USART_SendData(USART2, 'N');
}
