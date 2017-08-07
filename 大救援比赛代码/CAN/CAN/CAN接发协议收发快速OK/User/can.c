/**
  ******************************************************************************
  * @date   	2014-11-16
  * @brief  	can模块驱动
  ******************************************************************************
  * @attention
	*						PB8-CAN-RX
  *						PB9-CAN-TX
  * 
  ******************************************************************************
	*/

#include "P_bsp.h"

CanTxMsg TxMessage;
CanRxMsg RxMessage;
CAN_STA sta = CAN_START;
extern uint8_t IRQ_CAN;

/**
  * @brief 	设置报文信息
  * @param  None
  * @retval None
  */
uint8_t CAN_SetMsg(void)
{
	uint32_t	count_temp , count_temp2 , char_temp;
	while(1)
	{
		//printf("2\n");
		if(Empty_Queue(USART_RX_Queue)) return ERROR;
		
		
	//	Prin_Queue(&USART_RX_Queue);
		
		
		
		switch(sta)
		{	
			case CAN_START:	char_temp = Top_Queue(&USART_RX_Queue);
											TxMessage.DLC = char_temp&0x0f;
											if(TxMessage.DLC > 8 )
											{
												sta = CAN_START;
												//Init_Queue(&USART_RX_Queue);
												//return ERROR;
											}
											switch(char_temp&0xf0)
											{
//												case 0x80:TxMessage.RTR = CAN_RTR_DATA;
//																	TxMessage.IDE = CAN_ID_EXT;
//																	sta = CAN_DATAEXT;
//													break;
//												case 0xc0:TxMessage.RTR = CAN_RTR_REMOTE;
//																	TxMessage.IDE = CAN_ID_EXT;
//																	sta = CAN_REMOTEEXT;
//													break;
//												case 0x40:TxMessage.RTR = CAN_RTR_REMOTE;
//																	TxMessage.IDE = CAN_ID_STD;
//																	sta = CAN_REMOTESTD;
//													break;
												case 0x00:TxMessage.IDE = CAN_ID_STD;
																	TxMessage.RTR = CAN_RTR_DATA;
																	sta = CAN_DATASTD;
													break;
												default :  sta = CAN_START;
																	//Init_Queue(&USART_RX_Queue);
																	//return ERROR;
												
											}
											break;
			case CAN_DATAEXT:	
												char_temp = Top_Queue(&USART_RX_Queue);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												TxMessage.ExtId = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												if(TxMessage.DLC > USART_RX_Queue.Q_Mum)
												{
														sta = CAN_START;
														Init_Queue(&USART_RX_Queue);
														return ERROR;
												}
												for(count_temp = 0 ; count_temp < TxMessage.DLC ; count_temp++)
													TxMessage.Data[count_temp] = Top_Queue(&USART_RX_Queue);
											sta = CAN_START;
											Init_Queue(&USART_RX_Queue);
												return SUCCESS;
												break;
														
			case CAN_REMOTEEXT:	char_temp = Top_Queue(&USART_RX_Queue);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													TxMessage.ExtId = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
											sta = CAN_START;
											Init_Queue(&USART_RX_Queue);
												return SUCCESS;
											break;
														
			case CAN_DATASTD:	char_temp = Top_Queue(&USART_RX_Queue);
												char_temp = (uint32_t)Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												TxMessage.StdId =	char_temp>>5;
												if(TxMessage.DLC > USART_RX_Queue.Q_Mum)
												{
												sta = CAN_START;
												//Init_Queue(&USART_RX_Queue);
												return ERROR;
												}
												for(count_temp = 0 ; count_temp < TxMessage.DLC ; count_temp++)
													TxMessage.Data[count_temp] = Top_Queue(&USART_RX_Queue);
												sta = CAN_START;
												//Init_Queue(&USART_RX_Queue);
												return SUCCESS;
												break;
														
			case CAN_REMOTESTD:	char_temp = Top_Queue(&USART_RX_Queue);
													TxMessage.StdId = (uint32_t)Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													sta = CAN_START;
													Init_Queue(&USART_RX_Queue);
													return SUCCESS;
														break;
													
			}
	}
	return SUCCESS;
}

/**
  * @brief  配置CAN
  * @param  None
  * @retval None
  */
void CAN_Config(void)
{
	CAN_InitTypeDef		CAN_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
/*	
	//CAN_GPIO_Config
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1 , ENABLE );
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB , &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB , &GPIO_InitStructure);
	
	*/
	///
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//初始化IO
///
	
	//CAN_NVIC_Config
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	//CAN_Mode_Config
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
  CAN_InitStructure.CAN_ABOM = ENABLE;
  CAN_InitStructure.CAN_AWUM = ENABLE;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler = 4;
	CAN_Init(CAN1 , &CAN_InitStructure);
	
	
	//CAN_Filter_Config

	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	
	
/****************ID设置*******************/
					//拓展ID填充	//	CAN_FilterInitStructure.CAN_FilterIdHigh = Set_ID >>13 & 0xffff;
												//	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)(Set_ID << 3) | CAN_RTR_DATA | CAN_ID_STD;
	CAN_FilterInitStructure.CAN_FilterIdHigh = Set_ID << 5 & 0xffff;
	CAN_FilterInitStructure.CAN_FilterIdLow = CAN_RTR_DATA | CAN_ID_STD;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x00;//0xffc0;//接受固定标准ID，允许最后一位有不同
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x00;//0x0005;	//标准数据/远程帧
	
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	
	CAN_ITConfig(CAN1 , CAN_IT_FMP0 , ENABLE);
}

void TaskCanCommadDeal(void)
{
	uint32_t id_temp,count_temp;
	id_temp = RxMessage.StdId << 5;
	//printf("RE");
	USART_SendData(USE_USART , 0x08);
	while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
	USART_SendData(USE_USART , id_temp/0x100);
	while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
	USART_SendData(USE_USART , (id_temp&0xff));
	while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
	
	for( count_temp = 0; count_temp < 8 ; count_temp++)
	{
		USART_SendData(USE_USART , RxMessage.Data[count_temp]);
		while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
	}
}

/**
  * @brief  This function handles CAN RX interrupt request.
  * @param  None
  * @retval None
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(CAN_GetITStatus(CAN1,CAN_IT_FMP0))
	{
//		TaskCanCommadDeal();
		CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
		IRQ_CAN = 1;
	}
//	CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
//	printf("%x\n%c\n",RxMessage.ExtId,RxMessage.Data[0]);
}
