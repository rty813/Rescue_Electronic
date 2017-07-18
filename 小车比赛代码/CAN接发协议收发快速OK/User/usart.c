#include "P_bsp.h"

Queue USART_RX_Queue , USART_TX_Queue;


/**
  * @brief  配置USART
  * @param  None
  * @retval None
  */
void USART_Cofig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef USE_USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO , ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
#endif

	
#ifdef USE_USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA , &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
#endif


#ifdef USE_UART4
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC , &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC , &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
#endif
	
	
	USART_InitStructure.USART_BaudRate = SET_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USE_USART , &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
 
	USART_ITConfig(USE_USART , USART_IT_RXNE, ENABLE);
	USART_Cmd(USE_USART , ENABLE);
}


/**
  * @brief  串口中断函数
  * @param  None
  * @retval None
  */
#ifdef USE_USART1
void USART1_IRQHandler(void) 
{
	uint8_t Re_Data;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Re_Data = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if (!Full_Queue(USART_RX_Queue))  //队列是否已满
		{
			Push_Queue(&USART_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}
#endif

#ifdef USE_USART2
void USART2_IRQHandler(void) 
{
	uint8_t Re_Data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Re_Data = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if (!Full_Queue(USART_RX_Queue))  //队列是否已满
		{
			Push_Queue(&USART_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}
#endif

#ifdef USE_UART4
void UART4_IRQHandler(void)  
{
	uint8_t Re_Data;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		Re_Data = USART_ReceiveData(UART4);//(UART4->DR);	//读取接收到的数据
		if (!Full_Queue(USART_RX_Queue))  //队列是否已满
		{
			Push_Queue(&USART_RX_Queue , Re_Data);   //  把串口数据塞进队列
		}
	}
}
#endif


/**
  * @brief  printf()函数重定向
  * @param  None
  * @retval None
  */
int fputc(int ch , FILE *f)
{
	USART_SendData(USE_USART , (uint8_t) ch);
	while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);//while(!(USE_USART->SR & USART_FLAG_TXE));
	return ch;
}

