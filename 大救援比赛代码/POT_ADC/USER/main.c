#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "adc.h"
#include "timer.h"
#include "can.h"

/************************************************
 ALIENTEK 战舰STM32F103开发板实验17
 ADC 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

 
 int main(void)
 {	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	CAN_Config();
 	LED_Init();			     //LED端口初始化	 	
 	Adc_Init();		  		//ADC初始化    
	TIM3_Int_Init(7199,200); //50ms
	 LED1=0;
	while(1)
	{
		;
	}
 }

