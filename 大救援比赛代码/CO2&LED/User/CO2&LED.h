#ifndef __CO2_LED_h
#define __CO2_LED_h

#include "stm32f10x.h"
#include "usart.h"

#define USART_CO2		USART1
#define CO2_RX_Queue	USART1_RX_Queue
#define CO2_TIM		TIM6
#define LED_RCC		RCC_APB2Periph_GPIOC		
#define LED_GPIO	GPIOC
#define LED_GPIO_PIN	GPIO_Pin_4

#define ON  1
#define OFF 0

#define LED(a)	if (a)	\
					GPIO_SetBits(LED_GPIO,LED_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED_GPIO,LED_GPIO_PIN)

typedef enum	{ Reply1 , Reply2 , Reply3 , num1 , num2}	CO2_Msg_STA;
					
void Cmd_Read_CO2(void);
void LED_Config(void);
void CO2_TIM_Config(void);
void CO2_Analysis(void);
void Set_CO2_CAN_Msg(void);

extern const uint8_t	Cmd_Read_CO2_Seq[8];
extern CanTxMsg 	CO2_TxMessage;
extern uint32_t CO2_level;
extern uint8_t CO2_flag;
#endif
