#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"

u16 speed[4] = {890, 890, 890, 890};
u8 state = 0;

int main(void)
{
	u16 led0pwmval = 0;
	u8 dir = 1;
	u16 len;
	u16 t;
	delay_init();									//��ʱ������ʼ��	  0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart_init(115200); //���ڳ�ʼ��Ϊ115200
	uart3_init(115200);
	LED_Init();			   //LED�˿ڳ�ʼ��
	TIM5_PWM_Init(899, 0); //����Ƶ��PWMƵ��=72000000/900=80Khz
	TIM3_Int_Init(7199, 499);

	LED1 = 1;
	LED2 = 1;
	LED3 = 1;
	LED4 = 1;
	printf("����");
	while (1)
	{
		delay_ms(10);
		TIM_SetCompare1(TIM5, speed[0]);
		TIM_SetCompare2(TIM5, speed[1]);
		TIM_SetCompare3(TIM5, speed[2]);
		TIM_SetCompare4(TIM5, speed[3]);
		
		if (USART3_RX_STA & 0x8000)
		{
			LED1 = !LED1;
			if (USART3_RX_BUF[0] == '1'){
				LED1 = 0;
				LED3 = 0;
				LED2 = 1;
				LED4 = 1;
				state = 1;
			}
			if (USART3_RX_BUF[0] == '4'){
				state = 2;
				LED2 = 0;
				LED4 = 0;
				LED1 = 1;
				LED3 = 1;
			}
			if (USART3_RX_BUF[0] == '0'){
				state = 0;
				LED1 = 1;
				LED2 = 1;
				LED3 = 1;
				LED4 = 1;
			}
			USART3_RX_STA = 0;
		}

		/*if (USART_RX_STA & 0x8000)
		{
			//LED2 = !LED2;
			len = USART_RX_STA & 0x3fff;
			for (t = 0; t < len + 2; t++)
			{
				if (t == len)
				{
					USART_SendData(USART3, 0x0D);
				} else
				if (t == len + 1)
				{
					USART_SendData(USART3, 0x0A);
				} else{
					USART_SendData(USART3, USART_RX_BUF[t]);
				}
				while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET);
			}
			//printf("�������\r\n");
			USART_RX_STA = 0;
		}

		if (USART3_RX_STA & 0x8000)
		{
			LED1 = !LED1;
			len = USART3_RX_STA & 0x3fff;
			for (t = 0; t < len + 2; t++)
			{
				if (t == len)
				{
					USART_SendData(USART1, 0x0D);
				} else
				if (t == len + 1)
				{
					USART_SendData(USART1, 0x0A);
				} else{
					USART_SendData(USART1, USART3_RX_BUF[t]);
				}
				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
			}
			USART3_RX_STA = 0;
		}*/
	}
}
