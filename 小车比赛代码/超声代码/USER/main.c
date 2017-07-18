#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include <math.h>
#include <string.h>
#include "GY953.h"
#include "US-015.h"

int main(void)
{	
	float dis2;
	uart_init(115200);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();	    //延时函数初始化	  
	TIM_Int_Init(0xFFFF,72-1);
	US015Init();
	printf("Hello World!\r\n");
	while(1)
	{
		dis2 = GetDis2();		//front

	//	printf("F:%f\n",dis2);
	//	delay_ms(50);
	//	continue;
		
		if (dis2<20)
			printf("6\r\n");
		delay_ms(100);
		
	}
}


