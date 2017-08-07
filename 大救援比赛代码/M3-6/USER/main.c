#include "include.h"
double  speed_target[4]    ={0,0,0,0};
double  speed_target_Prv[4]={0,0,0,0};
double  speed_target_1,speed_target_2,speed_target_3,speed_target_4;

PID pid;
extern PID vPID[4]; 

u8 tmp_buf[30]={0},t;

u8 Speed_Flag =0;
double pwm2=100;

extern int update_count[4];//��¼�������
extern int firstcount[4];//��һ�����ж��ж�ȡ�ı�������������ֵ
extern int secondcount[4];//�ڶ������ж��ж�ȡ�ı�������������ֵ
extern int speed_pulse[4];
extern double  speed[4];
extern  double  pwmA[4];//PWMAֵ		 PWMռ�ձ�
extern  double  pwmB[4];//PWMBֵ


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
			printf("CAN����ʧ�ܣ�����");
			return 0;
		}
		else
		{
			return -1;
		}
}


 int main(void) 
 {		
	 u8 i = 0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��
	LED_Init();	 
	 
	TIM_PWM_Init(999,3); 		//PWM
	Encoder_Init();	 		//��������
	CAN_Config();
	 	LED1 = 0;
	 while(1)
	 {
		 	if(Speed_Flag)
			{
				TaskMotorSpeedAdj();
				Speed_Flag = 0;
			}
		 for(i=0;i<4;i++)
		 {
			angle_speed_transform(); //���Ĵ���λ 
		 }
		 if(speed_target[0]==0&&speed_target[1]==0&&speed_target[2]==0&&speed_target[3]==0)
			 break;
	 }
	 LED1 = 1;
 while(1)
 {
			if(Speed_Flag)
			{
				TaskMotorSpeedAdj();
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
