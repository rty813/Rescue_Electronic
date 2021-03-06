#include "include.h"
double  speed_target[4]    ={0,0,0,0};
double  speed_target_Prv[4]={0,0,0,0};
double  speed_target_1,speed_target_2,speed_target_3,speed_target_4;

PID pid;
extern PID vPID[4]; 

u8 tmp_buf[30]={0},t;

u8 Speed_Flag =0;
double pwm2=100;

extern int update_count[4];//记录溢出次数
extern int firstcount[4];//第一次在中断中读取的编码器计数器的值
extern int secondcount[4];//第二次在中断中读取的编码器计数器的值
extern int speed_pulse[4];
extern double  speed[4];
extern  double  pwmA[4];//PWMA值		 PWM占空比
extern  double  pwmB[4];//PWMB值


int send_can_data(float target_data1,float target_data2)
{
	u8 canbuf[8];
	u8 res;
	
		canbuf[0] = *((u8*)(&target_data1));
		canbuf[1] = *((u8*)(&target_data1) + 1);
		canbuf[2] = *((u8*)(&target_data1) + 2);
		canbuf[3] = *((u8*)(&target_data1) + 3);
		canbuf[4] = *((u8*)(&target_data2));
		canbuf[5] = *((u8*)(&target_data2) + 1);
		canbuf[6] = *((u8*)(&target_data2) + 2);
		canbuf[7] = *((u8*)(&target_data2) + 3);
		res = Can_Send_Msg(canbuf, 8,(0x08e0>>5));
		if (res)
		{
			printf("CAN发送失败！！！");
			return 0;
		}
		else
		{
			return -1;
		}
}

u8 rnf_data[6]={0};
u8 lost_rc_flag = 0;

 int main(void) 
 {		
	 u8 i = 0;
	//delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(1000000);	 //串口初始化
	LED_Init();	 
	cycleCounterInit();				// Init cycle counter
	SysTick_Config(SystemCoreClock / 1000);
	 
	TIM_PWM_Init(999,3); 		//PWM
	Encoder_Init();	 		//捕获脉冲
	CAN_Config();
	SPI1_Init();
	NRF24L01_Init();
//	while(NRF24L01_Check());
	 steer_reset();
	 LED1=1;
 while(1)
	{
		if(!(NRF24L01_RxPacket(rnf_data)))//接收到返回0
		{
			speed_target[0] = rnf_data[0];
			speed_target[1] = rnf_data[1];
		}
			if(Speed_Flag)
			{
				TaskMotorSpeedAdj();
				steer_move();
				Speed_Flag = 0;
			}
			
	  	i++;
			if(i>=150)
			{
				i=0;
	//		send_can_data();
			}
	}
	
}
