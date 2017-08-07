#include "include.h"


volatile u8 Roll[11] ={0xFF,0xFF,0x01,0x07,0x03,0x1E, 0xff,0x01, 0x3f,0x00, 0x00 };//0x01
volatile u8 Pitch[11]={0xFF,0xFF,0x02,0x07,0x03,0x1E, 0x3c,0x00, 0x3f,0x00, 0x00 };//0x02
extern double pos_change_target[2];

double Roll_Angle=0,Pitch_Angle=0, Yaw_Angle = 0, claw_Angle=0;
double Roll_Angle_var=0,Pitch_Angle_var=0, Yaw_Angle_var = 0, claw_Angle_var = 0;

void AX_pos_angle_transform(void)
{

	Roll_Angle = 511+( pos_change_target[0])/300.0*1024;
	Pitch_Angle = 201+( pos_change_target[1])/300.0*1024;
	/*    限制舵机位置    */
	if(Roll_Angle<63) Roll_Angle=63; 		if(Roll_Angle>805) Roll_Angle=805;        
	if(Pitch_Angle<201) Pitch_Angle=201;  	if(Pitch_Angle>816) Pitch_Angle=816;
	else ;
	//printf("roll  %.1f  pitch %.1f  yaw %.1f \r\n",Roll_Angle, Pitch_Angle, Yaw_Angle);
}

void AX_move(void)
{
	u8 sum=0;
	u8 t;
	Roll[6] =(int) Roll_Angle % 256;
	Roll[7] =(int) Roll_Angle / 256;
//	
	Pitch[6] = (int) Pitch_Angle %256;
	Pitch[7] = (int) Pitch_Angle /256;

	sum = 0;
	for(t=2;t<=9;t++)sum = sum + Roll[t];Roll[t]=(uint8_t)(~(sum));t=0;sum=0;Roll_Angle = 0;
	for(t=0;t<11;t++)
	{	
		USART_SendData(USART2, Roll[t]);                      //AX12_1   0x01
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束	
	}		
	
	sum = 0;
	for(t=2;t<=9;t++)sum = sum + Pitch[t];Pitch[t]=(uint8_t)(~(sum));t=0;sum=0;Pitch_Angle = 0;
	for(t=0;t<11;t++)
	{	
		USART_SendData(USART2, Pitch[t]);                      //AX12_2  0x02
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束	
	}
	
}


void steer_move(void)
{
	AX_pos_angle_transform();
	AX_move();
}

void steer_reset(void)
{
	pos_change_target[0]=0;
	pos_change_target[1]=0;
	AX_pos_angle_transform();
	AX_move();
}
