/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  

#include "stm32f10x.h"
#include "can.h"
#include "usart.h"
#include "CO2&LED.h"

uint8_t LEDT = OFF;
uint8_t LEDC = OFF;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
	
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//抢占,响应各四个优先级
	CAN_Config();
	LED_Config();
	CO2_TIM_Config();
	Set_CO2_CAN_Msg();
	USART_Cofig(USART1 , 9600);//CO2模块
	USART_Cofig(USART2 , 115200);//调试用
	printf("start\n");
	CO2_level = 500;
	while (1)
	{	
		if(CO2_flag)
		{
			Cmd_Read_CO2();
			CO2_flag = 0;
		}
		
		if(CO2_RX_Queue.Q_Mum > 5)
		{
			CO2_Analysis();
			//CO2_level = 0x12345678;
			//printf("%x\r\n", CO2_level);
			//*((uint32_t *)CO2_TxMessage.Data) = CO2_level;
			//CAN_Transmit(CAN1,&CO2_TxMessage);
			//printf("------%d\r\n",*((uint32_t *)CO2_TxMessage.Data));
		}
	
		if(LEDT != LEDC)
		{
			LED(LEDT);
			LEDC = LEDT;
		}	
//		if(count<8)
//			LED(OFF);
//		else
//			LED(ON);
//		if(count > 15)
//			count = 0;
			
		
	}
}
/************************END OF FILE********************************************/
