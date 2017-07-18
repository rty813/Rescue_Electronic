
#include "CO2&LED.h"
#include "can.h"

const uint8_t	Cmd_Read_CO2_Seq[8] = {0xFE,0x04,0x00,0x03,0x00,0x01,0xD5,0xC5};
CO2_Msg_STA CO2_sta = Reply1;
CanTxMsg 	CO2_TxMessage;
uint32_t CO2_level , CO2_temp;
uint8_t CO2_flag = 0;


/**
  * @brief 	配置LED
  * @param  None
  * @retval None
  */
void LED_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(LED_RCC , ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_GPIO , &GPIO_InitStructure);
	
	//GPIO_SetBits(LED_GPIO , LED_GPIO_PIN);
}


/**
  * @brief 	读取浓度周期(模块约1s更新一次)
  * @param  None
  * @retval None
  */
void CO2_TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 4999;	  //500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
}


/**
  * @brief 	发送读取指令
  * @param  None
  * @retval None
  */
void Cmd_Read_CO2(void)
{
	uint8_t	temp;
	for(temp = 0 ; temp < 8 ; temp++)
	{
		USART_SendData(USART_CO2 , Cmd_Read_CO2_Seq[temp]);
		while(USART_GetFlagStatus(USART_CO2 , USART_FLAG_TC)!=SET);//等待发送结束
	}
}

/**
  * @brief 	配置CO2 CAN报文
  * @param  None
  * @retval None
  */
void Set_CO2_CAN_Msg(void)
{
	CO2_TxMessage.DLC = 8;
	CO2_TxMessage.IDE = CAN_ID;
	CO2_TxMessage.RTR = CAN_RTR_DATA;
	if(CAN_ID)
		CO2_TxMessage.ExtId = Set_ID;
	else 
		CO2_TxMessage.StdId = ( 0x7e0>>5 );
}

/**
  * @brief 	解析CO2浓度
  * @param  None
  * @retval None
  */
void CO2_Analysis(void)
{
	uint8_t char_temp;
	while(!Empty_Queue(CO2_RX_Queue))
	{
		switch(CO2_sta)
		{
			case Reply1:char_temp = Top_Queue(&CO2_RX_Queue);
									if(char_temp == 0xfe)
										CO2_sta = Reply2;
									break;
									
			case Reply2:char_temp = Top_Queue(&CO2_RX_Queue);
									if(char_temp == 0x04)
										CO2_sta = Reply3;
									else
										CO2_sta = Reply1;
									break;
									
			case Reply3:char_temp = Top_Queue(&CO2_RX_Queue);
									if(char_temp == 0x02)
										CO2_sta = num1;
									else
										CO2_sta = Reply1;
									break;
			case num1:	CO2_temp = Top_Queue(&CO2_RX_Queue);
									CO2_sta = num2;
									break;
			case num2:	char_temp = Top_Queue(&CO2_RX_Queue);
									CO2_level = (CO2_temp << 8) + char_temp;
									CO2_sta = Reply1;
									break;
		}
	}	
}


/**
  * @brief CO2数据更新中断函数
  */
void TIM6_IRQHandler(void) 
{
	CO2_flag = 1;
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}

