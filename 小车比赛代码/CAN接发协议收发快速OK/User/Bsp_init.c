#include "P_bsp.h"

/**
  * @brief  初始函数
  * @param  None
  * @retval None
  */
void Bsp_init(void)
{
	delay_init();
	NVIC_Group_Init();
	USART_Cofig();
	CAN_Config();
	
}

/**
  * @brief 	中断组初始函数
  * @param  None
  * @retval None
  */
void NVIC_Group_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
}
