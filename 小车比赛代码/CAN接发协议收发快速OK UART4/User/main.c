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

#include "P_bsp.h"
uint8_t IRQ_CAN = 0;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
	
int main(void)
{
	float x;
	uint8_t str[]="CCC";
	uint32_t id_temp,count_temp;
	Bsp_init();
	//printf("start\n");
	while (1)
	{
		//Prin_Queue(&USART_RX_Queue);
		//delay_ms(4);
		if(USART_RX_Queue.Q_Mum>12)
		{//printf("%d",USART_RX_Queue.Q_Mum);
			//printf("1\n");
			//delay_ms(1);
			if(CAN_SetMsg() == SUCCESS)
			{	
				CAN_Transmit(CAN1,&TxMessage);
//				x = *(float*)&TxMessage.Data[0];
//				printf("%f,",x);
//				x = *(float*)&TxMessage.Data[4];
//				printf("  %f\n",x);
				//printf("%x,%x,%x,%x,%d,,,,,",TxMessage.StdId,TxMessage.ExtId,TxMessage.IDE,TxMessage.RTR,TxMessage.DLC);
	//printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",TxMessage.StdId,TxMessage.Data[0],TxMessage.Data[1],TxMessage.Data[2],TxMessage.Data[3],TxMessage.Data[4],TxMessage.Data[5],TxMessage.Data[6],TxMessage.Data[7]);
			}
		}//delay_ms(100);
		
		if(IRQ_CAN>0)
		{
			//CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
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
			IRQ_CAN = 0;
		}
	}
}

/************************END OF FILE********************************************/
