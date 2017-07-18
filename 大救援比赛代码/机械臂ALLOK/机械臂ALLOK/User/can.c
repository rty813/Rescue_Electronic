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

#include "can.h"
#include "usart.h"
#include "AX.h"

extern uint8_t AX_Mode;

extern volatile float Pos_Target[5];

extern uint8_t IRQ;

CanTxMsg 	TxMessage;
CanRxMsg	RxMessage;
TEST_CAN_STA test_sta = CAN_TX_READY;
SER_CAN_STA ser_sta= CAN_START;

/**
  * @brief 	手动串口发送CAN报文
  * @param  需发送报文
  * @retval ERROR 或 SUCCESS
  */
uint8_t TEST_CAN_SetMsg(CanTxMsg *SetMsg)
{
	uint32_t	count_temp , count_temp2 , char_temp;
	while(1)
	{
		//printf("2\n");
		if(Empty_Queue(USART_RX_Queue)) break;
		switch(test_sta)
		{	
			case CAN_TX_READY:	//printf("3\n");
														char_temp = Top_Queue(&USART_RX_Queue);
														if(char_temp == 0x66)
														{
															test_sta = CAN_TX_RTR;
														}
														break;
														
			case CAN_TX_RTR:		//printf("4\n");
														char_temp = Top_Queue(&USART_RX_Queue);
														if(char_temp == 0x00)
														{
															SetMsg->RTR = CAN_RTR_DATA;
														}
														else if(char_temp == 0xff)
														{
															SetMsg->RTR = CAN_RTR_REMOTE;
														}
														else
														{
															//Init_Queue(&USART_RX_Queue);
															printf("CAN_RTR ERROR!\n");
															test_sta = CAN_TX_READY;
															break;
														}
														test_sta = CAN_TX_ID;
														break;
														
			case CAN_TX_ID:			//printf("5\n");
														char_temp = Top_Queue(&USART_RX_Queue);
														if(char_temp == 0x00 && USART_RX_Queue.Q_Mum > 2)
														{
															SetMsg->IDE = CAN_ID_STD;
															char_temp = Top_Queue(&USART_RX_Queue);
															SetMsg->StdId =  Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
															test_sta = CAN_TX_DATA;
															break;
														}
														else if(char_temp == 0xff && USART_RX_Queue.Q_Mum > 4)
														{
															SetMsg->IDE = CAN_ID_EXT;
															char_temp = Top_Queue(&USART_RX_Queue);
															char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
															char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
															SetMsg->ExtId = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
															test_sta = CAN_TX_DATA;
															break;
														}
														else
														{
															//Init_Queue(&USART_RX_Queue);
															printf("CAN_IDE ERROR!\n");
															test_sta = CAN_TX_READY;
															break;
														}
														
			case CAN_TX_DATA:	//printf("6\n");
													if(SetMsg->RTR == CAN_RTR_DATA)
													{
														count_temp = Top_Queue(&USART_RX_Queue);
														if(count_temp > 8 || count_temp > USART_RX_Queue.Q_Mum-1)//此处修改过
														{
															//Init_Queue(&USART_RX_Queue);
															printf("CAN_DATA TOO LONG!\n");
															test_sta = CAN_TX_READY;
															break;
														}
														else
														{
															SetMsg->DLC = count_temp;
															for(count_temp2 = 0 ; count_temp2 < count_temp ; count_temp2++)
															{
																SetMsg->Data[count_temp2] = Top_Queue(&USART_RX_Queue);
															}
															test_sta = CAN_TX_CHECK;
															break;
														}
													}
													else test_sta = CAN_TX_CHECK;
													break;
													
			case CAN_TX_CHECK://printf("7\n");
													char_temp = Top_Queue(&USART_RX_Queue);
													test_sta = CAN_TX_READY;
													if(char_temp == 0xee)
													{
														printf("CAN_TX SUCESS!\n");
														printf("%x,%x...\n",SetMsg->StdId , SetMsg->Data[0]);
														return SUCCESS;
													}
													else
													{
														//Init_Queue(&USART_RX_Queue);
														printf("CAN_CHECK ERROR!\n");
														return ERROR;
													}
			}
	}
	return ERROR;
}


/**
  * @brief 	设置报文信息
  * @param  None
  * @retval None
  */
uint8_t SER_CAN_SetMsg(CanTxMsg SetMsg)
{
	uint32_t	count_temp , count_temp2 , char_temp;
	while(1)
	{
		//printf("2\n");
		if(Empty_Queue(USART_RX_Queue)) break;
		
	//	Prin_Queue(&USART_RX_Queue);
		
		switch(ser_sta)
		{	
			case CAN_START:	char_temp = Top_Queue(&USART_RX_Queue);
											SetMsg.DLC = char_temp&0x0f;
											if(SetMsg.DLC > 8 )
											{
												break;
												//ser_sta = CAN_START;
												//Init_Queue(&USART_RX_Queue);
												//return ERROR;
											}
											switch(char_temp&0xf0)
											{
												case 0x80:SetMsg.RTR = CAN_RTR_DATA;
																	SetMsg.IDE = CAN_ID_EXT;
																	ser_sta = CAN_DATAEXT;
													break;
												case 0xc0:SetMsg.RTR = CAN_RTR_REMOTE;
																	SetMsg.IDE = CAN_ID_EXT;
																	ser_sta = CAN_REMOTEEXT;
													break;
												case 0x40:SetMsg.RTR = CAN_RTR_REMOTE;
																	SetMsg.IDE = CAN_ID_STD;
																	ser_sta = CAN_REMOTESTD;
													break;
												case 0x00:SetMsg.IDE = CAN_ID_STD;
																	SetMsg.RTR = CAN_RTR_DATA;
																	ser_sta = CAN_DATASTD;
													break;
												default :  //ser_sta = CAN_START;
																	//Init_Queue(&USART_RX_Queue);
																	//return ERROR;
																	break;
												
											}
											break;
			case CAN_DATAEXT:	
												char_temp = Top_Queue(&USART_RX_Queue);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												#ifdef LEFT
													char_temp = char_temp << 3;
												#endif
												SetMsg.ExtId = char_temp;	
												if(SetMsg.DLC > USART_RX_Queue.Q_Mum)
												{
														ser_sta = CAN_START;
														//Init_Queue(&USART_RX_Queue);
														//return ERROR;
												}
												for(count_temp = 0 ; count_temp < SetMsg.DLC ; count_temp++)
													SetMsg.Data[count_temp] = Top_Queue(&USART_RX_Queue);
												CAN_Transmit(CAN1,&SetMsg);
												ser_sta = CAN_START;
												//Init_Queue(&USART_RX_Queue);
												break;
														
			case CAN_REMOTEEXT:	char_temp = Top_Queue(&USART_RX_Queue);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													#ifdef LEFT
														char_temp = char_temp << 3;
													#endif
													SetMsg.ExtId = char_temp;
													CAN_Transmit(CAN1,&SetMsg);
													ser_sta = CAN_START;
													//Init_Queue(&USART_RX_Queue);
													break;
														
			case CAN_DATASTD:	char_temp = Top_Queue(&USART_RX_Queue);
												char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
												#ifdef LEFT
													char_temp = char_temp << 5;
												#endif
												SetMsg.StdId = char_temp;
												if(SetMsg.DLC > USART_RX_Queue.Q_Mum)
												{
												ser_sta = CAN_START;
												Init_Queue(&USART_RX_Queue);
												return ERROR;
												}
												for(count_temp = 0 ; count_temp < SetMsg.DLC ; count_temp++)
													SetMsg.Data[count_temp] = Top_Queue(&USART_RX_Queue);
												CAN_Transmit(CAN1,&SetMsg);
												ser_sta = CAN_START;
												Init_Queue(&USART_RX_Queue);
												break;
														
			case CAN_REMOTESTD:	char_temp = Top_Queue(&USART_RX_Queue);
													char_temp = Top_Queue(&USART_RX_Queue) + (char_temp<< 8);
													#ifdef LEFT
														char_temp = char_temp << 5;
													#endif
													SetMsg.StdId = char_temp;
													CAN_Transmit(CAN1,&SetMsg);
													ser_sta = CAN_START;
													//Init_Queue(&USART_RX_Queue);
													break;
													
			}
	}
	return ser_sta;
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
//  CAN_InitStructure.CAN_SJW = CAN_SJW_4tq;
//  CAN_InitStructure.CAN_BS1 = CAN_BS1_15tq;
//  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
//  CAN_InitStructure.CAN_Prescaler =6;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
  CAN_InitStructure.CAN_Prescaler =4;
	CAN_Init(CAN1 , &CAN_InitStructure);
	
	
	//CAN_Filter_Config
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	
	
#ifdef CAN_ID	//拓展ID填充	
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x04e0;//Set_ID >>13 & 0xffff;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x06e0;//(uint16_t)(Set_ID << 3) | CAN_RTR_DATA | CAN_ID_EXT;

#else					//标准ID填充
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x04e0//Set_ID << 5 & 0xffff;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x06e0//CAN_RTR_DATA | CAN_ID_STD;
#endif	
	
	
	
	/*0xffffffff:	STID11位 + EXID18位 + IDE1位 + RTR1位 + 预留位*/
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x05e0;//0xffc0;//接受固定标准ID，允许最后一位有不同
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x08e0;//0x0005;	//标准数据/远程帧
	
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1 , CAN_IT_FMP0 , ENABLE);
}

void TaskCanCommadDeal(void)
{
	float an;
	//CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
	//printf("SD");
	//printf("\r\n%x",RxMessage.StdId>>3 );
	
//	TxMessage.StdId=RxMessage.StdId;					 //使用的扩展ID
//  TxMessage.IDE=CAN_ID_STD;					 //扩展模式
//  TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
//  TxMessage.DLC=2;							 //数据长度为2字节
//  TxMessage.Data[0]=0x11;
//  TxMessage.Data[1]=0x22;
	//TxMessage = *((CanTxMsg *)&RxMessage);
	//CAN_Transmit(CAN1,&TxMessage);

if(RxMessage.RTR == CAN_RTR_DATA)	//解析速度指令
	{
		switch(RxMessage.StdId)
		{
			case 0x04e0>>5:
				an = *((float*)RxMessage.Data);
				Pos_Target[0] = an*57.32484;
				//printf("1  %.1f\r\n",Pos_Target[0]);
				//printf("1");
				break;
			case 0x05e0>>5:
				an = *((float*)RxMessage.Data);
				Pos_Target[1] = an*57.32484;
				an = *(((float*)(&RxMessage.Data[4])));
				Pos_Target[2]	= an*57.32484;
				//printf("2  %.1f  %.1f\r\n",Pos_Target[1],Pos_Target[2]);
				//printf("2");
				break;
			case 0x06e0>>5:
				an = *((float*)RxMessage.Data);
				Pos_Target[3] = an*57.32484;
				an = *(((float*)(&RxMessage.Data[4])));
				Pos_Target[4]	= -an*57.32484;
				//printf("3  %.1f  %.1f\r\n",Pos_Target[3],Pos_Target[4]);
				//printf("3");
				break;
			case 0x08e0>>5:
				if(*((int*)RxMessage.Data) > 0)
					AX_Mode = Mode_AutoON;
				if(*((int*)RxMessage.Data) == 0)
					AX_Mode = Mode_AutoOFF;
			default : break;
		}
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
		CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
		TaskCanCommadDeal();
		CAN_GetITStatus(CAN1,CAN_IT_FMP0);
		
	}
//	CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
//	printf("%x\n%c\n",RxMessage.ExtId,RxMessage.Data[0]);
}
