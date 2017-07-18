#include "include.h"

double  speed_target[2]    ={0,0};
double  speed_target_Prv[2]={0,0};
double  speed_target_1,speed_target_2;  //无线接收速度

PID pid;
extern PID vPID[2]; 

u8 tmp_buf[33]={0},t;

u8 Speed_Flag =0;
double pwm2=100;

extern int update_count[2];//记录溢出次数
extern int firstcount[2]; //第一次在中断中读取的编码器计数器的值
extern int secondcount[2];//第二次在中断中读取的编码器计数器的值
extern int speed_pulse[2];
extern double  speed[2];
extern  double  pwmA[2];//PWMA值		 PWM占空比
extern  double  pwmB[2];//PWMB值


//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	USART_SendData(USART1,c);  
} 
//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}	

//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
}  
void msg_check_2()
{
	char a[7][40]={0};
	int i=0,t=0,u=0,num=0;
	while(USART_RX_BUF[t] != NULL)
			{		
					if(USART_RX_BUF[t] != 32)
					{
						a[u][num]=USART_RX_BUF[t];
						num++;
						t++;
					}
					while(USART_RX_BUF[t] == 32)
					{
						t++;
						if(USART_RX_BUF[t] != 32)
						{
							a[u][num]='\0';
							u++;
							num=0;
							break;
						}

					}
					
			}
	speed_target_1 = atof(a[0]);
	speed_target_2 = atof(a[1]);	
			
	for(i=0;i<u+1;i++)
	{
		for(num = 0;num< 20;num++)a[i][num] = NULL;
	}		
	for(i=0;i<20;i++)
	{
		USART_RX_BUF[i] = NULL;
	}	
	i=0;
	t=0;
	u=0;
	num=0;
}


u8 num_1a[11] =  {0xFF,0xFF,0x0a,0x07,0x03,0x1E, 0xdd,0x00, 0x03,0x00  ,0x00};  //旋转上限
u8 num_1b[11] =  {0xFF,0xFF,0x0a,0x07,0x03,0x1E, 0xa8,0x00, 0x03,0x00  ,0x00};  //旋转下限
u8 num_2a[11] =  {0xFF,0xFF,0x06,0x07,0x03,0x1E, 0x9b,0x01, 0x03,0x00  ,0x00};  //旋转上限
u8 num_2b[11] =  {0xFF,0xFF,0x06,0x07,0x03,0x1E, 0x22,0x02, 0x03,0x00  ,0x00};  //旋转下限

u8 Stop = 0;
int main(void) 
 {
	char Command_mark = 0;
	float speed_set = 11;    //速度设定, 可修改
	u8  Command = '6';           //串口传输的命令
	 
	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	GPIO_Direction_init();
	GPIO_AX12_init();
	
	NRF24L01_Init();    
	TIM_PWM_Init(999,3); 	//PWM
	Encoder_Init();	 		//捕获脉冲
	CAN_Config();
	 
	 uart_init(115200);	     //串口初始化为115200
	// TIM3_Int_Init(9999, 7199); //溢出时为一秒
	 uart_debug_init(1000000);     //数字舵的控制串口 
//	 while(NRF24L01_Check())
//	{
//		delay_ms(200);
//		printf("Not found\r\n");
// 		delay_ms(200);
//	}	
	 delay_ms (500);
	 NRF24L01_RX_Mode();     //无线到接收模式
	 
	for(t=2;t<10;t++)              //计算校验位
	{
		num_1a[10]+=num_1a[t];
		num_1b[10]+=num_1b[t];	
		num_2a[10]+=num_2a[t];
		num_2b[10]+=num_2b[t];			
	}

	num_1a[10]=~num_1a[10];
	num_1b[10]=~num_1b[10];
	num_2a[10]=~num_2a[10];
	num_2b[10]=~num_2b[10];
	
	AX12_send_data(num_1a,11);
	AX12_send_data(num_2a,11);

	while(1)
	{
		//无线接收
		if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息
			{
				tmp_buf[32]=0;//加入字符串结束符  
//				printf("receive succeed :\r\n" );
				Command = tmp_buf[0]+48;
//				printf("%c  \r\n",Command);
				Command_mark = 1;
			}
		//串口接收
		if(USART_RX_STA&0x8000)
		{					   

			USART_RX_STA=0;
			Command = USART_RX_BUF[0];
			Command_mark = 1;
	
		}
		
		if(Command_mark==1)
		{
			Command_mark = 0;
			switch(Command)
			{
				if(Stop == 1)
				{
					if(Command=='1')
					{
					break;
					}
					else Stop = 0;
				}
				
				
				case '1':            //前进 
					speed_target[0] = speed_set;
					speed_target[1] = speed_set;
				break;
				case '2':            //左转
					speed_target[0] = speed_set;
					speed_target[1] = -speed_set;
				break;
				case '3':            //右转
					speed_target[0] = -speed_set;
					speed_target[1] = speed_set;
				break;
				case '4':            //左偏
					speed_target[0] = speed_set+5;
					speed_target[1] = speed_set;
				break;
				case '5':            //右偏
					speed_target[0] = speed_set;
					speed_target[1] = speed_set+5;
				break;
				case '6':            //超声停止
					speed_target[0] = 0;
					speed_target[1] = 0;
					Stop = 1;
//					time = update_cnt+TIM_GetCounter(TIM3)/10000.0;
//					printf("Turn time = %.3fs\r\n", time);
//					TIM_Cmd(TIM3, DISABLE);
//					CAN_ITConfig(CAN1 , CAN_IT_FMP0 , DISABLE);
//					speed_target[0] = 0;
//					speed_target[1] = 0;		
//					i=0;
				break;
				case '7':            //停止
					speed_target[0] = 0;
					speed_target[1] = 0;
				break;
				case '8':            //后退
					speed_target[0] = -speed_set;
					speed_target[1] = -speed_set;
				break;
				default :     
					Command = '7';     //设定速度后置停止状态
				break;
			}
		}
				

//		printf("speed_target[0] = %f, speed_target[1] = %f, command = %d, speed_set = %.2f\r\n",speed_target[0], speed_target[1], Command, speed_set);           
//	    speed_target[0] = -6;
//			speed_target[1] = 6;		
		if(Speed_Flag)
			{
				
				TaskMotorSpeedAdj();
				Speed_Flag = 0;
				
			}
//		mpu6050_send_data((short)(speed_target[0]), ((short)speed[0]),(short)speed_target[1],(short)speed[1],0,0);
//		printf("%f,  %f\r\n", speed[0], speed[1]);	

	}
	
}

/*------------------------------------*/
//	if(USART_RX_STA&0x8000)
//		{					   

//			USART_RX_STA=0;
//			UART_Speed = 1;
//			
//		}
//		if(UART_Speed)
//		{
//			Speed_Flag = 0;
//				msg_check_2();  //接收速度
// 				printf("Speed_Target======*  %f\r\n",speed_target_1);
// 				printf("pid.Proportion====*  %f\r\n",pid.Proportion);
// 				printf("pid.Integral======*  %f\r\n",pid.Integral);
// 				printf("pid.Derivative====*  %f\r\n",pid.Derivative);

//							speed_target[0]  = speed_target_1;
//							speed_target[1]  = speed_target_2;
//							speed_target[2]  = speed_target_3;
//							speed_target[3]  = speed_target_4;

// 			for(i=0;i<4;i++)
// 					{		

// 							vPID[2].Proportion = pid.Proportion;
// 							vPID[2].Integral  = pid.Integral;
// 							vPID[2].Derivative  = pid.Derivative;
// 					}
//		
//				UART_Speed = 0;
//		}   







/*---------------------------------------------------------------------------------*/			
//			mpu6050_send_data((short)(speed_target[0]), ((short)speed[0]),(short)speed_target[1],(short)speed[1],0,0);
		//	printf("\r\nspeed_target = %f,speed = %f\r\n", speed_targ
		//	et[0],speed[0]);
		//	printf("\r\nspeed_target = %f,speed = %f\r\n", speed_target[1],speed[1]);
		//	printf("\r\n");
//			delay_ms(10);
			
// 			TaskCanCommadDeal();
			
			
// 			printf("%.1lf %.1lf %.1lf %.1lf \r\n ",speed[0],vPID[0].SetPoint,speed[1],vPID[1].SetPoint);
// 			printf("%.1lf %.1lf %.1lf %.1lf\r\n ",vPID[0].SetPoint,vPID[1].SetPoint,vPID[2].SetPoint,vPID[3].SetPoint);

// 		printf(" 0--> %lf %lf %lf %lf\r\n ",speed[0],vPID[0].SetPoint,pwmA[0],pwmB[0]);
//  	printf(" 1--> %lf %lf %lf %lf",speed[1],vPID[1].SetPoint,pwmA[1],pwmB[1]);
	
// 		printf("%.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf %.1lf \r\n",speed[0],speed[1],speed[2],speed[3],vPID[0].SetPoint,vPID[1].SetPoint,vPID[2].SetPoint,vPID[3].SetPoint);
// 		printf("%.1lf %.1lf %.1lf %.1lf\r\n",speed[0],speed[1],speed[2],speed[3]);
// 	  printf("%.1lf \r\n",speed[2]);
