#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart3.h"
#include "stdlib.h"
#include "suanfa.h"
#include "timer.h"
#include "hmc.h"
#include "mpu6050.h"
#include "myiic.h"
#include "usart1.h"	 
//��ʼˮƽ״̬�µĳ�ʼλ��      ID  Length Write Reg  position   speed      check
const u8 num_1[11] =  {0xFF,0xFF,0x01,0x07,0x03,0x1E, 0xFF,0x01, 0xFF,0x00, 0xD7 };
const u8 num_2[11] =  {0xFF,0xFF,0x02,0x07,0x03,0x1E, 0xCC,0x00, 0xFF,0x00, 0x0A };

volatile u8 Roll[11] ={0xFF,0xFF,0x01,0x07,0x03,0x1E, 0x00,0x00, 0x64,0x00, 0x00 };//0x01
volatile u8 Pitch[11]={0xFF,0xFF,0x02,0x07,0x03,0x1E, 0x00,0x00, 0x64,0x00, 0x00 };//0x02

double Roll_Angle=0,Pitch_Angle=0;
volatile int mpu_flag = 0;
double errorax,erroray,erroraz;
extern double Gyro_correct[3];
int send_flag;
void Goto_Origin()	
{
	static u8 i=0;
	




}



int main(void)
 {		
 	u8 t,q;
	u16 sum=0;
	 GPIO_InitTypeDef  GPIO_InitStructure;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	uart3_init(1000000);	 
	IO_Init();
	 
	PB12 = 1;
	PB13 = 0; 
	AX_TXD	
	 

 	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				 //LED0-->PA.8 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8						 
	GPIO_ResetBits(GPIOB,GPIO_Pin_1); 
	
	 delay_ms(500);
	for(t=0;t<11;t++)
	{	
		USART_SendData(USART3, num_1[t]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
	}
		delay_ms(500);
	for(t=0;t<11;t++)
	{	
		USART_SendData(USART3, num_2[t]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
	} 
	uart2_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	
 	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600

	IIC_Init();	 
	InitMPU6050();

	delay_us(1000);delay_us(1000);delay_us(1000);
for(q=0;q<100;q++)
{
    errorax = (double)CombineData(GYRO_XOUT_H)/16.4+errorax;
	erroray = (double)CombineData(GYRO_YOUT_H)/16.4+erroray;
	erroraz= (double)CombineData(GYRO_ZOUT_H)/16.4+erroraz;

}	
Gyro_correct[0]=errorax/100;
Gyro_correct[1]=erroray/100;
Gyro_correct[2]=erroraz/100;
	TIM3_Int_Init(499,7199);
	TIM2_Int_Init(10000-1,7199);

 	while(1)
	{ 
		//GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		if(send_flag%2==0)
 				printf("%.2f %.2f\r\n",EA.Roll,EA.Pitch);
if(send_flag>30000)
	send_flag=0;

		if(mpu_flag ==1)
		{	
			
			Roll_Angle = ( Roll_Angle + 150 )/300.0*1024;
			Roll[6] =(int) Roll_Angle % 256;
			Roll[7] =(int) Roll_Angle / 256;
		  for(t=2;t<=9;t++)sum = sum + Roll[t];Roll[t]=(uint8_t)(~(sum));t=0;sum=0;Roll_Angle = 0;

			Pitch_Angle = ( Pitch_Angle + 60 )/300.0*1024;
			Pitch[6] =(int)  Pitch_Angle % 256;
			Pitch[7] =(int)  Pitch_Angle / 256;
		  for(t=2;t<=9;t++)sum = sum + Pitch[t];Pitch[t]=(uint8_t)(~(sum));t=0;sum=0;Pitch_Angle = 0;
			
			

		for(t=0;t<11;t++)
			{	
				USART_SendData(USART3, Pitch[t]);
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
			}		

			for(t=0;t<11;t++)
			{	
				USART_SendData(USART3, Roll[t]);
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���	
			}			
		
		Roll_Angle  = 0 ;
		Pitch_Angle = 0;
		mpu_flag = 0;
		
		}
	}	 
 }

 
 //��ʱ��3�жϷ������
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		
         
		suanfa();
// 		Roll_Angle = 0.9*Roll_Angle + 0.1*EA.Roll ;
// 		Pitch_Angle = 0.9*Pitch_Angle +0.1*EA.Pitch;
			
		Roll_Angle = -1*(0.2*Roll_Angle + 0.8*EA.Roll );
		Pitch_Angle = -1*(0.2*Pitch_Angle +0.8*EA.Pitch);
		
		mpu_flag = 1;	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 	
			send_flag++;			
		}
}