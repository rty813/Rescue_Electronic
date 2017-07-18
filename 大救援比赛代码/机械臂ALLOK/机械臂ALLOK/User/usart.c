#include "usart.h"
#include "stdlib.h"
Queue USART1_RX_Queue , USART1_TX_Queue;
Queue USART2_RX_Queue , USART2_TX_Queue;
Queue USART3_RX_Queue , USART3_TX_Queue;
Queue UART4_RX_Queue , UART4_TX_Queue;

extern volatile unsigned char gbpRxInterruptBuffer[];
extern volatile unsigned char gbRxBufferWritePointer;

/**
  * @brief  配置USART
	* @param  串口号，波特率
  * @retval None
  */
void USART_Cofig(USART_TypeDef* USARTx , u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	if(USARTx == USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO , ENABLE);
		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA , &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA , &GPIO_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	}

	
	if(USARTx == USART2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA , &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA , &GPIO_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	}
	
	
	if(USARTx == USART3)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);

		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB , &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOB , &GPIO_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	}


	if(USARTx == UART4)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);
		
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC , &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC , &GPIO_InitStructure);

		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	}
	
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx , &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
	USART_ITConfig(USARTx , USART_IT_RXNE, ENABLE);
	USART_Cmd(USARTx , ENABLE);
}


/**
  * @brief  串口中断函数
  */
void USART1_IRQHandler(void) 
{
	uint8_t Re_Data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Re_Data = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if (!Full_Queue(USART1_RX_Queue))  //队列是否已满
		{
			Push_Queue(&USART1_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}

void USART2_IRQHandler(void) 
{
	uint8_t Re_Data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		
		Re_Data = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if (!Full_Queue(USART2_RX_Queue))  //队列是否已满
		{
			Push_Queue(&USART2_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}

void USART3_IRQHandler(void) 
{												   		 //串口3用来直接与舵机通信，该中断用来读取舵机信息
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) 
	{
		u8 dat;	
		USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清除中断标志.	    
		dat=USART_ReceiveData(USART3);
		gbpRxInterruptBuffer[(gbRxBufferWritePointer++)] = dat;
				
	}

}


void UART4_IRQHandler(void)  
{
	uint8_t Re_Data;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		Re_Data = USART_ReceiveData(UART4);//(UART4->DR);	//读取接收到的数据
		if (!Full_Queue(UART4_RX_Queue))  //队列是否已满
		{
			Push_Queue(&UART4_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}


/**
  * @brief  printf()函数重定向
  */
int fputc(int ch , FILE *f)
{
	USART_SendData(USE_USART , (uint8_t) ch);
	while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);//while(!(USE_USART->SR & USART_FLAG_TXE));
	return ch;
}

/**
  * @brief  队列操作函数
  */
uint8_t Top_Queue(Queue *pQueue)
{
	uint8_t data;
	data = pQueue->Q_data[pQueue->Q_Head];
	pQueue->Q_Head = (pQueue->Q_Head + 1) % QUEUE_SIZE;
	pQueue->Q_Mum --;
	return data;
}

void Push_Queue(Queue *pQueue, uint8_t ch)
{
	pQueue->Q_data[pQueue->Q_Tail] = ch;
	pQueue->Q_Tail = (pQueue->Q_Tail + 1) % QUEUE_SIZE;
	pQueue->Q_Mum ++;
}

void Init_Queue(Queue *pQueue)
{
	pQueue->Q_Head = 0;
	pQueue->Q_Tail = 0;
	pQueue->Q_Mum = 0;
}

void Prin_Queue(Queue *pQueue , USART_TypeDef* USARTx)
{
	while(!Empty_Queue(*pQueue))
	{
		USART_SendData(USARTx , Top_Queue(pQueue));
		while(USART_GetFlagStatus(USARTx , USART_FLAG_TXE) == RESET);
	}
}

/**
  * @brief  字符串转换浮点型
  */
float Str_to_Float( Queue *pQueue)
{
	float flo_num;
	uint8_t str[Str_to_Float_MAX_SIZE] = {0};
	uint8_t str_count=0;
	uint8_t char_temp;
	while(!Empty_Queue(*pQueue))
	{
		char_temp = Top_Queue(pQueue);
		if(char_temp != ' ')
		{
			str[str_count] = char_temp;
			str_count++;
		}
		else break;
	}
	flo_num = atof(str);
	
	return flo_num;
}


