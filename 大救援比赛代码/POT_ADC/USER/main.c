#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "adc.h"
#include "timer.h"
#include "can.h"

/************************************************
 ALIENTEK ս��STM32F103������ʵ��17
 ADC ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

 
 int main(void)
 {	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	CAN_Config();
 	LED_Init();			     //LED�˿ڳ�ʼ��	 	
 	Adc_Init();		  		//ADC��ʼ��    
	TIM3_Int_Init(7199,200); //50ms
	 LED1=0;
	while(1)
	{
		;
	}
 }

