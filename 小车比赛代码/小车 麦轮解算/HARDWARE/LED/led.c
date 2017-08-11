#include "led.h"


void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				 //LED0-->PB.0 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.1
 GPIO_ResetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1); 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  			
 GPIO_ResetBits(GPIOC,GPIO_Pin_5); 			
}
 
