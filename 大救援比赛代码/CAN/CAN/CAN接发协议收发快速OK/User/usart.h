#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "queue.h"
#include "stdio.h"

#define SET_BAUDRATE	115200	// ≤®Ãÿ¬ …Ë÷√


#define USE_USART		USART1
#define USE_USART1
//#define USE_USART2		
//#define USE_UART4		

#ifdef USE_USART1
	void USART1_IRQHandler(void);
#endif

#ifdef USE_USART2
	void USART2_IRQHandler(void);
#endif

#ifdef USE_UART4
	void UART4_IRQHandler(void);
#endif


int fputc(int ch , FILE *f);
void USART_Cofig(void);

extern Queue USART_RX_Queue , USART_TX_Queue;
#endif /* __USART_H */
