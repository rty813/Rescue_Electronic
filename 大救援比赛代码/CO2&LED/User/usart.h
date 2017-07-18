#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

#define QUEUE_SIZE  64    				//缓冲大小
#define Str_to_Float_MAX_SIZE 20	//字符串转化浮点数最大长度
typedef struct{
	uint8_t Q_data[QUEUE_SIZE];
	uint8_t Q_Head, Q_Tail, Q_Mum;
}Queue;															//接受队列结构体


#define Empty_Queue(Queue_Check)	((Queue_Check).Q_Mum == 0)						//检测空队列
#define Full_Queue(Queue_Check)		((Queue_Check).Q_Mum == QUEUE_SIZE)		//检测满队列
uint8_t Top_Queue(Queue * pQueue);						//取出队头
void Push_Queue(Queue *pQueue, uint8_t ch);		//填入队尾
void Prin_Queue(Queue *pQueue , USART_TypeDef* USARTx);	//打印队列元素
void Init_Queue(Queue *pQueue);								//初始化队列

int fputc(int ch , FILE *f);	//重定向
void USART_Cofig(USART_TypeDef* USARTx , u32 baudrate);				//串口配置

extern Queue USART1_RX_Queue , USART1_TX_Queue;
extern Queue USART2_RX_Queue , USART2_TX_Queue;
//extern Queue USART3_RX_Queue , USART3_TX_Queue;
extern Queue UART4_RX_Queue , UART4_TX_Queue;

#define USE_USART		USART2
#define USART_RX_Queue USART2_RX_Queue
#define USART_TX_Queue USART2_TX_Queue

#endif /* __USART_H */
