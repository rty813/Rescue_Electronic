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

#include "include.h"

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
	float x,x1,x2;
	u8 i,j;
// 	if(	CAN_EN == 1)	
// 	{
//	CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);	
//	CAN_Transmit(CAN1,(CanTxMsg *)&RxMessage);
//	printf("%x,%x,%x,%x,%d...\n",RxMessage.StdId,RxMessage.ExtId,RxMessage.IDE,RxMessage.RTR,RxMessage.DLC);
//	printf("RE%x,%x,%x,%x,%x,%x,%x,%x,%x...\n",RxMessage.StdId,RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2],RxMessage.Data[3],RxMessage.Data[4],RxMessage.Data[5],RxMessage.Data[6],RxMessage.Data[7]);
	
	if(RxMessage.RTR == CAN_RTR_DATA)	//解析速度指令
	{
		switch(RxMessage.StdId)
		{
			case (0x01e0>>5):
					x1 = *((float*)(&RxMessage.Data[0]));  //主1
					x2 = *((float*)(&RxMessage.Data[4]));  
				//	x = (x1+x2)/2.0;
					//printf("1 %.1f\r\n",x);
					speed_target[0] = 80*x1  ;
			    speed_target[3] = 80*x2;
					timeflag = 0;
					break;			
			case (0x02e0>>5):
					x1 = *((float*)(&RxMessage.Data[0]));  //主2
					x2 = *((float*)(&RxMessage.Data[4]));  
				//	x = (x1+x2)/2.0;
					//printf("1 %.1f\r\n",x);
					speed_target[1] = -80*x1 ;
			speed_target[2] = -80*x2;
					timeflag = 0;
					break;
			case (0x03e0>>5):
//					x1 = *((float*)(&RxMessage.Data[0]));  //副1
//				  x2 = *((float*)(&RxMessage.Data[4]));
//			    x = (x1+x2)/2.0;
//					//printf("1\r\n");
//			
//					
//					speed_target[2] = x * -80;
//			     timeflag=0;
//					//printf("3\r\n");
//					//	break;
			case (0x04e0>>5):
				
//			x1 = *((float*)(&RxMessage.Data[0]));  //副1
//				  x2 = *((float*)(&RxMessage.Data[4]));
//			    x = (x1+x2)/2.0;
//			speed_target[2] = x * 80;
//			     timeflag=0;
//					//speed_target[0] = *(float*)&RxMessage.Data[0];
//					//speed_target[1] = *(float*)&RxMessage.Data[4];
//					//printf("4\r\n");
//					//break;
			case (0x1a):
				speed_target[0]=(RxMessage.Data[1])/10.0;
				speed_target[1]=(RxMessage.Data[3])/10.0;
				speed_target[2]=(RxMessage.Data[5])/10.0;
				speed_target[3]=(RxMessage.Data[7])/10.0;
				
			//	printf("%f %f %f %f\r\n",speed_target[0],speed_target[1],speed_target[2],speed_target[3]);
				for(i=0,j=0;i<4;i++)
				{
					if(RxMessage.Data[j]==1)
					speed_target[i]=-speed_target[i];
					j+=2;
				}
				for(i=0;i<4;i++)
				{
					printf("%f ",speed_target[i]);
				}
				printf("\r\n");
			default: break;
			
		}
	}
	
// 		}	
// 	CAN_EN = 0;
//	else if(RxMessage.RTR == CAN_RTR_Remote)//回传信息
//	{
//		switch(RxMessage.StdId)
//		{
//			case Main_Track:
//			
//				break;
//			case Side_Track:
//				
//				break;
//			default: break;
//		}
//	}


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
// 		CAN_EN = 1;
	//	printf("can_receive");
		LED0=!LED0;
		TaskCanCommadDeal();
		CAN_GetITStatus(CAN1,CAN_IT_FMP0);	
	}
}
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}