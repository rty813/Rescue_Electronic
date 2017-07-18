/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.1
  * @date    12-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * 帧长度任意，无补位字节
  ******************************************************************************
  */  

#include "P_bsp.h"
uint8_t IRQ_CAN = 0;
u8 last_number = 1;
u8 data = 0;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
	
int main(void)
{
	float x;
	u8 temp;
	uint8_t str[]="CCC";
	uint32_t id_temp,count_temp;
	Bsp_init();
	//printf("start\n");
	while (1)
	{
		//Prin_Queue(&USART_RX_Queue);
		//delay_ms(4);
		if(data>0)
		{//printf("%d",USART_RX_Queue.Q_Mum);
			printf("1\n");
			//delay_ms(1);
			if(CAN_SetMsg() == SUCCESS)
			{	
				CAN_Transmit(CAN1,&TxMessage);
				printf("a");
//				x = *(float*)&TxMessage.Data[0];
//				printf("%f,",x);
//				x = *(float*)&TxMessage.Data[4];
//				printf("  %f\n",x);
				//printf("%x,%x,%x,%x,%d,,,,,",TxMessage.StdId,TxMessage.ExtId,TxMessage.IDE,TxMessage.RTR,TxMessage.DLC);
	//printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",TxMessage.StdId,TxMessage.Data[0],TxMessage.Data[1],TxMessage.Data[2],TxMessage.Data[3],TxMessage.Data[4],TxMessage.Data[5],TxMessage.Data[6],TxMessage.Data[7]);
			data--;
			}
		}//delay_ms(100);
		
		if(IRQ_CAN>0)
		{
			//CAN_Receive(CAN1 , CAN_FIFO0 , &RxMessage);
			//id_temp = RxMessage.StdId << 5;
			//printf("RE");
			temp = (RxMessage.IDE + RxMessage.RTR) << 5;
			switch(temp)
			{
				case 0x80:
					USART_SendData(USE_USART, temp + RxMessage.DLC);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART, RxMessage.ExtId >> 24);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);

					USART_SendData(USE_USART, RxMessage.ExtId >> 16);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART, RxMessage.ExtId >> 8);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART, RxMessage.ExtId);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					for( count_temp = 0; count_temp < RxMessage.DLC; count_temp++)
					{
						USART_SendData(USE_USART , RxMessage.Data[count_temp]);
						while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
					}
					
					break;
					
				case 0xc0:
					USART_SendData(USE_USART, temp);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);				

					USART_SendData(USE_USART, RxMessage.ExtId >> 24);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);

					USART_SendData(USE_USART, RxMessage.ExtId >> 16);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART, RxMessage.ExtId >> 8);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART, RxMessage.ExtId);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
					
					break;
				
				case 0x40:
					USART_SendData(USE_USART, temp);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);	
				
					USART_SendData(USE_USART , RxMessage.StdId >> 3);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART , RxMessage.StdId << 5);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);

					break;
				
				case 0x00:
					USART_SendData(USE_USART, temp + RxMessage.DLC);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART , RxMessage.StdId >> 3);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
				
					USART_SendData(USE_USART , RxMessage.StdId << 5);
					while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);

					for( count_temp = 0; count_temp < RxMessage.DLC; count_temp++)
					{
						USART_SendData(USE_USART , RxMessage.Data[count_temp]);
						while(USART_GetFlagStatus(USE_USART , USART_FLAG_TXE) == RESET);
					}
					
					break;					
			}
			
			IRQ_CAN = 0;
		}
		
/*		if(IRQ_CAN > 0)
		{
			TxMessage.DLC = RxMessage.DLC;
			TxMessage.RTR = CAN_RTR_DATA;
		  TxMessage.IDE = CAN_ID_EXT;
			TxMessage.ExtId = RxMessage.ExtId;
			for(count_temp = 0 ; count_temp < TxMessage.DLC ; count_temp++)
				TxMessage.Data[count_temp] = RxMessage.Data[count_temp];
			CAN_Transmit(CAN1,&TxMessage);
			IRQ_CAN = 0;
		}*/
	}
}

/************************END OF FILE********************************************/
