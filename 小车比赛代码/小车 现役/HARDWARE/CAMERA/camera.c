#include "include.h"
u8 canbuf[8];
	float TO = 0;
	float TA = 0;
	u8 res;
void TIM6_camera_init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
	TIM_DeInit(TIM6);
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM6, ENABLE);  //使能TIMx			 
}

void AX12_send_data(u8 data[], u8 len )    //串口给数字舵发数据
{
	u8 i;
	for(i=0; i<len; i++)
	{
		USART_SendData(UART4, data[i]);//向串口4发送数据
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//等待发送结束
	}
}


unsigned int update_cnt = 0;
u8 up_dowm_mark = 0;              //改变舵机上下转的标志
extern u8 num_1a[], num_2a[];

//定时器6中断服务程序
void TIM6_IRQHandler(void)   //TIM6中断
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM6更新中断发生与否
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志 
			update_cnt++;
			if(update_cnt>=12)
			{
				update_cnt = 0;
				up_dowm_mark = ~up_dowm_mark;
				if(up_dowm_mark)
				{
				  AX12_send_data(num_1a,11);
					AX12_send_data(num_2a,11);
				}
				else
				{
	//				AX12_send_data(num_1b,11);
		//		AX12_send_data(num_2b,11);
				}
			}
		}
		
//		if(USART_RX_STA &&0x8000==0x8000)
//		{
//		TO = ((USART_RX_BUF[4] << 8) | USART_RX_BUF[5]) / 100.0;
//		TA = ((USART_RX_BUF[6] << 8) | USART_RX_BUF[7]) / 100.0;
//		printf("目标温度：%.1f\t环境温度：%.1f\n",TO, TA);
//		
//		canbuf[0] = *((u8*)(&TO));
//		canbuf[1] = *((u8*)(&TO) + 1);
//		canbuf[2] = *((u8*)(&TO) + 2);
//		canbuf[3] = *((u8*)(&TO) + 3);
//		canbuf[4] = *((u8*)(&TA));
//		canbuf[5] = *((u8*)(&TA) + 1);
//		canbuf[6] = *((u8*)(&TA) + 2);
//		canbuf[7] = *((u8*)(&TA) + 3);
//		res = Can_Send_Msg(canbuf, 8);
//		
//			USART_RX_STA=0;
//		}
}





