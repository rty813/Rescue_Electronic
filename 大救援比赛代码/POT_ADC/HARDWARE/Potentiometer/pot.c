#include "pot.h"


float v[4]={0};
float angle[4]={0};
void get_pot_adc()
{
	  u16 adc[4]={0,0,0,0};
		u8 i;
		adc[0]=Get_Adc_Average(ADC_Channel_0,3);
		adc[1]=Get_Adc_Average(ADC_Channel_1,3);
		adc[2]=Get_Adc_Average(ADC_Channel_2,3);
		adc[3]=Get_Adc_Average(ADC_Channel_3,3);
			for(i=0;i<4;i++)
		{
			v[i]=(float)(adc[i])*(3.3/4096);
			adc[i]=v[i];
			//LCD_ShowxNum(156,150,adcx,1,16,0);//ÏÔÊ¾µçÑ¹Öµ
			//printf("v = %f\t", v[i]);
			//v[i]-=adc[i];
			//v[i]*=1000;
		}
		//printf("\r\n");
		
}

void adc_angle_transform()
{
	u8 i;
	for(i=0;i<4;i++)
	{
		angle[i]=v[i]*300/3.3;
		printf("angle = %f\t", angle[i]);
	}
	printf("\r\n");
}
