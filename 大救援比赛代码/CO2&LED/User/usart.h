#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"

#define QUEUE_SIZE  64    				//�����С
#define Str_to_Float_MAX_SIZE 20	//�ַ���ת����������󳤶�
typedef struct{
	uint8_t Q_data[QUEUE_SIZE];
	uint8_t Q_Head, Q_Tail, Q_Mum;
}Queue;															//���ܶ��нṹ��


#define Empty_Queue(Queue_Check)	((Queue_Check).Q_Mum == 0)						//���ն���
#define Full_Queue(Queue_Check)		((Queue_Check).Q_Mum == QUEUE_SIZE)		//���������
uint8_t Top_Queue(Queue * pQueue);						//ȡ����ͷ
void Push_Queue(Queue *pQueue, uint8_t ch);		//�����β
void Prin_Queue(Queue *pQueue , USART_TypeDef* USARTx);	//��ӡ����Ԫ��
void Init_Queue(Queue *pQueue);								//��ʼ������

int fputc(int ch , FILE *f);	//�ض���
void USART_Cofig(USART_TypeDef* USARTx , u32 baudrate);				//��������

extern Queue USART1_RX_Queue , USART1_TX_Queue;
extern Queue USART2_RX_Queue , USART2_TX_Queue;
//extern Queue USART3_RX_Queue , USART3_TX_Queue;
extern Queue UART4_RX_Queue , UART4_TX_Queue;

#define USE_USART		USART2
#define USART_RX_Queue USART2_RX_Queue
#define USART_TX_Queue USART2_TX_Queue

#endif /* __USART_H */
