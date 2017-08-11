#include "include.h"
u8 canbuf[8];
	float TO = 0;
	float TA = 0;
	u8 res;
void TIM6_camera_init(u16 arr,u16 psc)
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	TIM_DeInit(TIM6);
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM6, ENABLE);  //ʹ��TIMx			 
}

void AX12_send_data(u8 data[], u8 len )    //���ڸ����ֶ淢����
{
	u8 i;
	for(i=0; i<len; i++)
	{
		USART_SendData(UART4, data[i]);//�򴮿�4��������
		while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
}


unsigned int update_cnt = 0;
u8 up_dowm_mark = 0;              //�ı�������ת�ı�־
extern u8 num_1a[], num_2a[];

//��ʱ��6�жϷ������
void TIM6_IRQHandler(void)   //TIM6�ж�
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM6�����жϷ������
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
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
//		printf("Ŀ���¶ȣ�%.1f\t�����¶ȣ�%.1f\n",TO, TA);
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





