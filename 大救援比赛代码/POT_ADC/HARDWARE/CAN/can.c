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
#include "stdio.h"

extern double speed_target[4];
extern double speed_target_1, speed_target_2;
u8 CAN_EN = 0;
CanTxMsg TxMessage;
CanRxMsg RxMessage;
CAN_TX sta=CAN_TX_READY;

extern uint32_t timeflag;
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
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;//32位屏蔽模式
	
	
/****************ID设置*******************/
		//拓展ID填充			
//	CAN_FilterInitStructure.CAN_FilterIdHigh = Set_ID >>13 & 0xffff;
//	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)(Set_ID << 3) | CAN_RTR_DATA | CAN_ID_STD;														
	CAN_FilterInitStructure.CAN_FilterIdHigh = Set_ID << 5 & 0xffff;
	CAN_FilterInitStructure.CAN_FilterIdLow = CAN_RTR_DATA | CAN_ID_STD;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0;//0xffc0;//接受固定标准ID，允许最后一位有不同
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0;//0x0005;	//标准数据/远程帧
	
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
		
	CAN_ITConfig(CAN1 , CAN_IT_FMP0 , ENABLE);
	
}

void TaskCanCommadDeal(void)
{
	float x1,x2;

	if(RxMessage.RTR == CAN_RTR_DATA)	//解析速度指令
	{
		switch(RxMessage.StdId)
		{
			case (0x01e0>>5):
					x1 = *((float*)(&RxMessage.Data[0]));  //主1
					x2 = *((float*)(&RxMessage.Data[4]));  
					break;		
			
			default: break;
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
}

u8 Can_Send_Msg(u8* msg,u8 len,uint32_t id)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
  TxMessage.StdId=id;	
  TxMessage.IDE=0;		 
  TxMessage.RTR=0;		 
	TxMessage.DLC=len;			
	for(i = 0; i < len; i++)
		TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF)){
		i++;	
	}
	if(i>=0XFFF)return 1;
	return 0;	 
}

int send_can_data(float target_data1,float target_data2,uint32_t id)
{
	u8 canbuf[8];
	u8 res;
	
		canbuf[0] = *((u8*)(&target_data1));
		canbuf[1] = *((u8*)(&target_data1) + 1);
		canbuf[2] = *((u8*)(&target_data1) + 2);
		canbuf[3] = *((u8*)(&target_data1) + 3);
		canbuf[4] = *((u8*)(&target_data2));
		canbuf[5] = *((u8*)(&target_data2) + 1);
		canbuf[6] = *((u8*)(&target_data2) + 2);
		canbuf[7] = *((u8*)(&target_data2) + 3);
		res = Can_Send_Msg(canbuf, 8,id);
		if (res)
		{
			printf("CAN发送失败！！！");
			return 0;
		}
		else
		{
			return -1;
		}
}
