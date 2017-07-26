#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "gy521.h"	 

 
/************************************************
 ALIENTEK战舰STM32开发板实验23
 IIC 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

 				 	
//要写入到24c02的字符串数组
const u8 TEXT_Buffer[]={"WarShipSTM32 IIC TEST"};
#define SIZE sizeof(TEXT_Buffer)	
	
int GetData(u16 REG_Address)
{
	unsigned char H,L;
	H=GY521_ReadOneByte(REG_Address);
	L=GY521_ReadOneByte(REG_Address+1);
	return (H<<8)+L;   //合成数据
}

int main(void)
{	 
	u8 key;
	u16 i=0;
	u8 datatemp[SIZE];
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	LED_Init();		  		//初始化与LED连接的硬件接口
	KEY_Init();				//按键初始化		 	 	
	GY521_Init();			//IIC初始化 
	while(1){
		printf("%f ", (GetData(ACCEL_XOUT_H) * 9.8) / 65536);
		printf("%f ", (GetData(ACCEL_YOUT_H) * 9.8) / 65536);
		printf("%f\t", (GetData(ACCEL_ZOUT_H) * 9.8) / 65536);

		printf("%d ", GetData(GYRO_XOUT_H));
		printf("%d ", GetData(GYRO_YOUT_H));
		printf("%d \r\n", GetData(GYRO_ZOUT_H));


		
		delay_ms(100);
	}
}

