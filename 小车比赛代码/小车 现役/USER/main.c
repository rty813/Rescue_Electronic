#include "include.h"
double  speed_target[4]    ={0,0,0,0};
double  speed_target_Prv[4]={0,0,0,0};
double  speed_target_1,speed_target_2,speed_target_3,speed_target_4;

PID pid;
extern PID vPID[4]; 


u8 Speed_Flag =1;
double pwm2=100;
u8 i=0;
extern int update_count[4];//记录溢出次数
extern int firstcount[4];//第一次在中断中读取的编码器计数器的值
extern int secondcount[4];//第二次在中断中读取的编码器计数器的值
extern int speed_pulse[4];
extern double  speed[4];
extern  double  pwmA[4];//PWMA值		 PWM占空比
extern  double  pwmB[4];//PWMB值
u8 num_1a[8] =  {0xFF,0xFF,0x00,0x04,0x03,0x18, 0x01,0xdf};  //旋转上限
u8 num_1b[11] =  {0xFF,0xFF,0x00,0x07,0x03,0x1E, 0x77,0x01, 0x07,0x00  ,0x58};  //旋转下限
u8 num_2a[8] =  {0xFF,0xFF,0x01,0x04,0x03,0x18, 0x01,0xde};  //旋转上限
u8 num_2b[11] =  {0xFF,0xFF,0x01,0x07,0x03,0x1E, 0x56,0x03, 0x07,0x00  ,0x79};  //旋转下限
u8 tmp_buf[33]={0},i,j;
u8 tmp_buf1[33]={0,1,2,3,4,5};
	 u8 controll;
//	 double speed_yaokong;
//u8 stop_1b[]={};
 int main(void) 
 {		
	//u8 UART_Speed = 0;
	 
//	}

	//u8 UART_Speed = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	//  can>计算速度>编码器>摄像头
	USART_Cofig(USART2,115200);
	USART_Cofig(UART4,1000000);             //控制数字舵
	USART_Cofig(USART3,115200);
	LED_Init();	 
	 
	TIM_PWM_Init(999,3); 		//PWM
	Encoder_Init();	 		//捕获脉冲
	CAN_Config();
//	NRF24L01_Init();
//  TIM6_camera_init(9999,7199);       //1s

	 delay_ms (500);

	 

	//NRF24L01_TX_Mode(); 	     //无线到接收模式
 while(1)
	{

//		if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
//			{
//				tmp_buf[32]=0;//加入字符串结束符
//	//			controll=1;
//				speed_target[0]=(double)(tmp_buf[1]+48)/10;
//				speed_target[1]=(double)(tmp_buf[3]+48)/10;
//				speed_target[2]=(double)(tmp_buf[5]+48)/10;
//				speed_target[3]=(double)(tmp_buf[7]+48)/10;
//				
//				printf("%f %f %f %f\r\n",speed_target[0],speed_target[1],speed_target[2],speed_target[3]);
//				for(i=0,j=0;i<4;i++)
//				{
//					if(tmp_buf[j]=='1')
//					speed_target[i]=-speed_target[i];
//					j=+2;
//				}

//			}
//			
			
			
			
			
		
		if(Speed_Flag)
			{
			LED1 =1;
 			TaskMotorSpeedAdj();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
			Speed_Flag = 0;
			}
		
			

	}
	
}
