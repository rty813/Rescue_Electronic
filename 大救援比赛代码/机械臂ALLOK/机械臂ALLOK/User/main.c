/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */  

#include "stm32f10x.h"
#include "usart.h"
#include "can.h"
#include "bitband.h"
#include "delay.h"
#include "math.h"
#include "AX.h"

#define USART_AX USART3

//#define  AX_TXD  {GPIO_SetBits(GPIOB,GPIO_Pin_12); GPIO_ResetBits(GPIOB,GPIO_Pin_13);}
//#define  AX_RXD  {GPIO_SetBits(GPIOB,GPIO_Pin_13); GPIO_ResetBits(GPIOB,GPIO_Pin_12);}

typedef enum { POS_RESET_OFF , POS_RESET_P_I , POS_RESET_P_II , POS_RESET_P_III } POS_RESET_STA; 
//                                 							ID                   position  	speed      check
/*const*/ uint8_t AX_Init_1[11] =  {0xFF,0xFF,0x0A,0x07,0x03,0x1E, 0xFF,0x01, 0xFF,0x00, 0xD7 };//初值
/*const*/ uint8_t AX_Init_2[11] =  {0xFF,0xFF,0x06,0x07,0x03,0x1E, 0xFF,0x01, 0xFF,0x00, 0x0A };
volatile uint8_t Roll[11]  =  {0xFF,0xFF,0x0A,0x07,0x03,0x1E, 0x00,0x00, 0x5F,0x00, 0x00 };//0x0A
volatile uint8_t Pitch[11] =  {0xFF,0xFF,0x06,0x07,0x03,0x1E, 0x00,0x00, 0x5F,0x00, 0x00 };//0x06	
//地址0X1A开始 为柔性边距 防止舵机烧坏
const uint8_t AX_Remove_1[11]  =  {0xFF,0XFF,0X0A,0X07,0X03,0X1A,0X5F,0X5F,0X5F,0X5F,0X55}; 
const uint8_t AX_Remove_2[11]  =  {0xFF,0XFF,0X06,0X07,0X03,0X1A,0X5F,0X5F,0X5F,0X5F,0X59};
const	float Gyro_Max[5] = { 0.5 , 0.15 , 0.25 , 10 , 10};//每次角度调整最大角度改变量
const float Angle_Init[5] = {90 , 70 , 0 , 150 , 70};//70
const uint32_t Angle_Limit[4] = {0 , 60 , 0 , 150};
const float Position_I[5] = {90 , 150 , 150 , 150 , 150};
const float Position_II[5] = {90 , 150 , 150 , 150 , 150};
const float Position_III[5] = {90 , 150 , 150 , 150 , 150};
volatile float Pos_Cur[5] = {0 , 0 , 0 , 0 , 0};//当前位置
volatile float Pos_Target[5] = {0 , 0 , 20 , 0 , 0};//目标位置
volatile POS_RESET_STA Reset_Sta = POS_RESET_OFF; //机械臂复位
uint8_t Angle_Up_Data_flag = 0;
uint8_t AX_Mode = Mode_AutoON;
uint8_t Time_IT = 0;

///*const*/ uint8_t AX_Init_1[11] =  {0xFF,0xFF,0x01,0x07,0x03,0x1E, 0xFF,0x01, 0xFF,0x00, 0xD7 };//初值
///*const*/ uint8_t AX_Init_2[11] =  {0xFF,0xFF,0x02,0x07,0x03,0x1E, 0xFF,0x01, 0xFF,0x00, 0x0A };
//volatile uint8_t Roll[11]  =  {0xFF,0xFF,0x01,0x07,0x03,0x1E, 0x00,0x00, 0xFF,0x00, 0x00 };//0x0A
//volatile uint8_t Pitch[11] =  {0xFF,0xFF,0x02,0x07,0x03,0x1E, 0x00,0x00, 0xFF,0x00, 0x00 };//0x06	
////地址0X1A开始 为柔性边距 防止舵机烧坏
//const uint8_t AX_Remove_1[11]  =  {0xFF,0XFF,0X01,0X07,0X03,0X1A,0X5F,0X5F,0X5F,0X5F,0X55}; 
//const uint8_t AX_Remove_2[11]  =  {0xFF,0XFF,0X02,0X07,0X03,0X1A,0X5F,0X5F,0X5F,0X5F,0X59};

void Set_Roll_Angle(float target_angle);
void Set_Pitch_Angle(float target_angle);
void Up_Data_TIM_Config(uint32_t times);
void TIM2_PWM_Init(void);
void Up_Data_Angle(void);
void AX_Config(void);
void Pos_Reset(void);
void AX_Auto_TIM_Cofing(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	float flo_temp;
	uint8_t char_temp;
	uint32_t u32_temp;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	CAN_Config();
	USART_Cofig(USART1 , 115200);
//	USART_Cofig(USART2 , 115200);
//	USART_Cofig(USART3 , 115200);
	USART_Cofig(UART4 , 115200);
	AX_Config();//2*数字舵串口3初始化及初始命令写入
	TIM2_PWM_Init();//3*模拟舵机PWM初始化
	Up_Data_TIM_Config(20);//20ms中断一次
	AX_Auto_TIM_Cofing();
	printf("start\n");
	while (1)
	{
		if(USART_RX_Queue.Q_Mum > 1)
		{
			delay_ms(10);
			for(char_temp = 0 ; char_temp<5 ; char_temp++)
			{
				flo_temp = Str_to_Float(&USART_RX_Queue);
				Pos_Target[char_temp] = flo_temp;
				printf("%.2f    ",flo_temp);
			}
			//Pos_Target[1] = -Pos_Target[1];
			Pos_Target[4] = -Pos_Target[4];
			printf("\r\n");
		}
		if(Angle_Up_Data_flag > 0)
		{
			Up_Data_Angle();
			
//			u32_temp = PWM_INIT_1+Pos_Cur[0]/180*2000;//180*2000;
//			TIM_SetCompare1(TIM2,u32_temp);
//			u32_temp = PWM_INIT_2+Pos_Cur[1]/300*2000;
//			TIM_SetCompare2(TIM2,u32_temp);
//			u32_temp = PWM_INIT_3+Pos_Cur[2]/300*2000;
//			TIM_SetCompare3(TIM2,u32_temp);
			
			if(Pos_Cur[1] < Angle_Limit[0])//限制角度
				Pos_Cur[1] = Angle_Limit[0];
			else if(Pos_Cur[1] > Angle_Limit[1])
				Pos_Cur[1] = Angle_Limit[1];
			//printf("%.1f\r\n",Pos_Cur[1]);
			
			if(Pos_Cur[2] < Angle_Limit[2])
				Pos_Cur[2] = Angle_Limit[2];
			else if(Pos_Cur[2] > Angle_Limit[3])
				Pos_Cur[2] = Angle_Limit[3];
			
			u32_temp = 500+(Angle_Init[0]+Pos_Cur[0])/180*2000;//180*2000;//写入角度
			TIM_SetCompare1(TIM2,u32_temp);
			u32_temp = 500+(Angle_Init[1]-Pos_Cur[1])/300*2000;
			TIM_SetCompare2(TIM2,u32_temp);
			u32_temp = 500+(Angle_Init[2]+Pos_Cur[2])/300*2000;	
			TIM_SetCompare3(TIM2,u32_temp);
			
			switch(AX_Mode)
			{
				case Mode_AutoON:
					Set_Roll_Angle(90);
					TIM_SetCounter(TIM7,0);
					TIM_Cmd(TIM7,ENABLE);
					AX_Mode = Mode_Auto;
					break;
				case Mode_Auto:
					break;
				case Mode_AutoOFF:
					TIM_Cmd(TIM7,DISABLE);
					gbpParameter[0] = 0x24;
					gbpParameter[1] = 0x02;
					TxPacket(0x0A,INST_READ,2);
					RxPacket(8);
					gbpParameter[0] = 0x1E;
					gbpParameter[1] = gbpRxBuffer[5];
					gbpParameter[2] = gbpRxBuffer[6];

				  TxPacket(0x0A,INST_WRITE,3);
					Pos_Cur[3] = (gbpRxBuffer[5] + gbpRxBuffer[6]*256)/1024.0*300 - Angle_Init[3];
					Pos_Target[3] = Pos_Cur[3];
					AX_Mode = Mode_Normal;
					break;
				case Mode_Normal:
					Set_Roll_Angle(Pos_Cur[3]);
					break;
			}
			
			Set_Pitch_Angle(Pos_Cur[4]);
			
			Angle_Up_Data_flag = 0;
		}
	}
}

void Set_Roll_Angle(float target_angle)
{
	uint32_t Roll_Angle , sum;
	uint8_t temp;
	
	AX_TXD
	
	Roll_Angle = ( target_angle + Angle_Init[3] )/300.0*1024;  //300度分成1024等分 2f 03 -- cc 00

	if(Roll_Angle >= 0x032f)Roll_Angle = 0x032f;
	if(Roll_Angle <= 0x00cc)Roll_Angle = 0x00cc;
			
	Roll[6] =(int) Roll_Angle % 256;
	Roll[7] =(int) Roll_Angle / 256;
	for(sum = 0, temp=2; temp <= 9 ; temp++)
		sum += Roll[temp];
	Roll[temp]=(uint8_t)(~(sum));
	
	for(temp = 0 ; temp < 11 ; temp++)
	{	
		USART_SendData(USART_AX, Roll[temp]);
		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);//等待发送结束	
	}
}

void Set_Pitch_Angle(float target_angle)
{
	uint32_t Pitch_Angle , sum;
	uint8_t temp;
	
	Pitch_Angle = ( target_angle + Angle_Init[4] )/300.0*1024;  //300度分成1024等分 2f 03 -- cc 00

	if(Pitch_Angle >= 0x032f)Pitch_Angle = 0x032f;
	if(Pitch_Angle <= 0x00cc)Pitch_Angle = 0x00cc;
			
	Pitch[6] =(int) Pitch_Angle % 256;
	Pitch[7] =(int) Pitch_Angle / 256;
	for(sum = 0, temp=2; temp <= 9 ; temp++)
		sum += Pitch[temp];
	Pitch[temp]=(uint8_t)(~(sum));
	
	for(temp = 0 ; temp < 11 ; temp++)
	{	
		USART_SendData(USART_AX, Pitch[temp]);
		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);//等待发送结束	
	}
}

void AX_Config(void)
{
//	uint8_t temp;
//	uint8_t	t;
//	uint32_t sum;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB , &GPIO_InitStructure);
	
	USART_Cofig(USART3 , 1000000);
	
	AX_TXD
	
//	PBout(12) = 1;
//	PBout(13) = 0;
	
	delay_ms(100);//必须有

//	for(t=2 ,sum=0;t<=9;t++)
//		sum = sum + AX_Init_1[t];
//	AX_Init_1[t]=(uint8_t)(~(sum));
//	
//	for(t=2 ,sum=0;t<=9;t++)
//		sum = sum + AX_Init_2[t];
//	AX_Init_2[t]=(uint8_t)(~(sum));

//	for(temp = 0 ; temp < 11 ; temp++)
//	{	
//		USART_SendData(USART_AX, AX_Init_1[temp]);
//		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);//等待发送结束	
//	}
//	
//	for(temp = 0 ; temp < 11 ; temp++)
//	{	
//		USART_SendData(USART_AX, AX_Init_2[temp]);
//		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);	
//	}
	
//	for(temp = 0 ; temp < 11 ; temp++)
//	{	
//		USART_SendData(USART_AX, AX_Remove_1[temp]);
//		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);	
//	}
//	
//	for(temp = 0 ; temp < 11 ; temp++)
//	{	
//		USART_SendData(USART_AX, AX_Remove_2[temp]);
//		while(USART_GetFlagStatus(USART_AX,USART_FLAG_TC)!=SET);
//	}
	printf("AX\r\n");
}

void TIM2_PWM_Init(void)
{  
	uint32_t PWM_INIT_1,PWM_INIT_2,PWM_INIT_3;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |	RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2; //TIM_CH1,2,3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO  
 
	TIM_TimeBaseStructure.TIM_Period = 19999;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2 , &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	PWM_INIT_2 = 500+(Angle_Init[1]+Pos_Cur[1])/300*2000;	
	PWM_INIT_3 = 500+(Angle_Init[2] + Pos_Cur[2])/300*2000;

	TIM_OCInitStructure.TIM_Pulse = 1500;
	TIM_OC1Init(TIM2 ,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2 , TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = PWM_INIT_2;
	TIM_OC2Init(TIM2 ,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM2 , TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse = PWM_INIT_3;
	TIM_OC3Init(TIM2 ,&TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2 , TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM2 , ENABLE);
	
	TIM_Cmd(TIM2 , ENABLE);
}

/**
  * @brief 	更新当前角度周期(避免角速度过大)
	* @param  None
  * @retval None
  */
void Up_Data_TIM_Config(uint32_t times)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = times*10-1;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
}

void AX_Auto_TIM_Cofing(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);	
	
	TIM_TimeBaseStructure.TIM_Period = 9999;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM7,ENABLE);	
}

void Up_Data_Angle(void)
{
	uint8_t count_temp;
	for(count_temp = 0 ; count_temp < 5 ;count_temp++)
	{
		if(fabs(Pos_Cur[count_temp] - Pos_Target[count_temp]) < Gyro_Max[count_temp])
			Pos_Cur[count_temp] = Pos_Target[count_temp];
		else if(Pos_Cur[count_temp] > Pos_Target[count_temp])
			Pos_Cur[count_temp] -= Gyro_Max[count_temp];
		else Pos_Cur[count_temp] += Gyro_Max[count_temp];
					//printf("%.1f\r\n",Pos_Cur[count_temp]);
	}//printf("\r\n\r\n");
}

void Pos_Reset(void)
{
	uint8_t count_temp;
	switch( Reset_Sta )
	{
		case POS_RESET_OFF:
			break;
		
		case POS_RESET_P_I:
			for(count_temp = 0 ; count_temp<5 ; count_temp++)
			{
				Pos_Target[count_temp] = Position_I[count_temp];
			}
			for(count_temp = 0 ; count_temp<5	&& Pos_Target[count_temp] == Pos_Cur[count_temp]; count_temp++);
			if(count_temp == 5)
				Reset_Sta = POS_RESET_P_II;
			break;
			
		case POS_RESET_P_II:
			for(count_temp = 0 ; count_temp<5 ; count_temp++)
			{
				Pos_Target[count_temp] = Position_II[count_temp];
			}
			for(count_temp = 0 ; count_temp<5	&& Pos_Target[count_temp] == Pos_Cur[count_temp]; count_temp++);
			if(count_temp == 5)
				Reset_Sta = POS_RESET_P_III;
			break;
			
		case POS_RESET_P_III:
			for(count_temp = 0 ; count_temp<5 ; count_temp++)
			{
				Pos_Target[count_temp] = Position_III[count_temp];
			}
			for(count_temp = 0 ; count_temp<5	&& Pos_Target[count_temp] == Pos_Cur[count_temp]; count_temp++);
			if(count_temp == 5)
				Reset_Sta = POS_RESET_OFF;
			break;
	}
	
}

void TIM6_IRQHandler(void) 
{
	Angle_Up_Data_flag = 1;
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}

void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) //检查 TIM3 更新中断发生与否
	{
	TIM_ClearITPendingBit(TIM7, TIM_IT_Update ); //清除 TIM3 更新中断标志
		Time_IT++;
		if(Time_IT == 10)
			Set_Roll_Angle(-90);
		if(Time_IT == 20)
			Set_Roll_Angle(90);
		Time_IT %= 20;
	}
}

/************************END OF FILE********************************************/
