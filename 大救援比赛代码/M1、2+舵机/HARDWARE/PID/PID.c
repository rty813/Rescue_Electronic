#include "include.h"

extern  double  pwmA[4];//PWMA值		 PWM占空比
extern  double  pwmB[4];//PWMB值
extern  double  speed[4];
extern  double  speed_target[4];
extern  double  speed_target_Prv[4];

/************************************************************
double 型数据
			  1     		2						3					4		
		SetPoint  Proportion  Integral  Derivative 
		   
			  5					6						7					8
		 Error_1  	Error_2  	 Error_3    iIncpid; 
																				//增量
少年加油，我佛保佑，永无bug！																				
**************************************************************/
//						   	1   2   3   4   5   6   7   8
//								
PID vPID[4] ={ 	{ 0 , 4 , 4 , 0 , 0 , 0 , 0 , 0 } ,
								{ 0 , 4 , 4 , 0 , 0 , 0 , 0 , 0 } ,
								{ 0 , 4 , 4 , 0 , 0 , 0 , 0 , 0 } ,                            
								{ 0 , 4 , 4 , 0 , 0 , 0 , 0 , 0 } ,        
							};
 

float IncPIDCalc(PID *PIDx ,double NextPoint)//增量式pid控制
{
	float increase;
	PIDx->Error_1 = fabs(PIDx->SetPoint) - fabs(NextPoint);  

	PIDx->iIncpid = PIDx->Proportion *
								( ( PIDx->Error_1 -PIDx->Error_2 ) + 
									( PIDx->Integral * PIDx->Error_1 * Tine ) +
							    ( PIDx->Derivative * (PIDx->Error_1 - 2* PIDx->Error_2 + PIDx->Error_3) * Tined )
								 )
						      ;

	PIDx->Error_3 = PIDx->Error_2;	//存储误差，下次计算
	PIDx->Error_2 = PIDx->Error_1;

	increase = PIDx->iIncpid ;
	return(increase);//返回增量值
}

double Speed_Choose(unsigned char i, double *speed, double *Target_Speed_Prv, double *Target_Speed_New)
{
	if(Target_Speed_Prv[i] * Target_Speed_New[i] < 0)
	{
		return 0;
	} 
	else 
	{
		Target_Speed_Prv[i] = Target_Speed_New[i];
		return Target_Speed_New[i];
	}
}


/****************ucos  调速任务*************************/

/***************************************************************
	正转 															反转
						pwmA[0]  pwmA[1]							pwmB[0]  pwmB[1]
			TIM8  CH1_2    CH3_4					TIM8  CH1_2    CH3_4
						pwmA[2]  pwmA[3]							pwmB[2]  pwmB[3]
			TIM8  CH1_2    CH3_4					TIM8  CH1_2    CH3_4
****************************************************************/	

u8  flag[4]={0} ;
extern u8 lost_rc_flag;
void TaskMotorSpeedAdj()
{
	u8 i=0;
	double pwm_inc;

		//控制信号丢失保护
	if (lost_rc_flag){
		TIM_SetCompare1(TIM8, 0);
		TIM_SetCompare2(TIM8, 0);
		speed_target[0] = 0;
		speed_target[1] = 0;
		return;
	}
	
  for(i=0;i<2;i++)
	{		 
			vPID[i].SetPoint = Speed_Choose(i, speed, speed_target_Prv , speed_target);
		
			if(vPID[i].SetPoint > 0)
			{	
				flag[i] = 1;
				pwmB[i] = 0;
				pwm_inc = IncPIDCalc(&vPID[i],speed[i]);	 
				if(pwm_inc > MAX_INC_PWM)pwm_inc = MAX_INC_PWM;
				if(pwm_inc < MIN_INC_PWM)pwm_inc = MIN_INC_PWM;
				pwmA[i] = pwmA[i] + pwm_inc;
					
		    if(pwmA[i]>=MAX_PWM) pwmA[i]=MAX_PWM;  //限速
				if(pwmA[i]<= 0) pwmA[i]=0;

				switch(i)
				{
					case 0:
						CH1_A = 1;
						CH1_B = 0;
						TIM_SetCompare1(TIM8,pwmA[i]+DEAD_PWM );
						break;
					case 1:
						CH2_A = 1;
						CH2_B = 0;
						TIM_SetCompare2(TIM8,pwmA[i]+DEAD_PWM);
						break;
					default: break;		
				}
			 }
			
		  if(vPID[i].SetPoint < 0)
			{	
				flag[i] = 2;
				pwmA[i] = 0;
				pwm_inc= IncPIDCalc(&vPID[i],speed[i]);	
				if(pwm_inc > MAX_INC_PWM)pwm_inc = MAX_INC_PWM;
				if(pwm_inc < MIN_INC_PWM)pwm_inc = MIN_INC_PWM;
					
				pwmB[i] = pwmB[i] + pwm_inc ;

				if(pwmB[i]>=MAX_PWM) pwmB[i]=MAX_PWM;  //限速
				if(pwmB[i]<= 0) pwmB[i]=0;	
				
				switch(i)
				{
					case 0:
						CH1_A = 0;
						CH1_B = 1;
						TIM_SetCompare1(TIM8,pwmB[i]+DEAD_PWM);
						break;
					case 1:
						CH2_A = 0;
						CH2_B = 1;
						TIM_SetCompare2(TIM8,pwmB[i]+DEAD_PWM);
						break;
					default: break;	
				}
			 }	
		  else if(vPID[i].SetPoint == 0)
		  {
			 if(flag[i] == 1)
				{
				 pwmB[i] = 0 ;
				 pwmA[i]=pwmA[i]*0.95;
				 if(pwmA[i]<50) pwmA[i]=0;

				switch(i)
				{
					case 0:
						CH1_A = 1;
						CH1_B = 0;
						TIM_SetCompare1(TIM8,pwmA[i]);
						break;
					case 1:
						CH2_A = 1;
						CH2_B = 0;
						TIM_SetCompare2(TIM8,pwmA[i]);
						break;
					default: break;		
				}
		   } 

			if(flag[i] == 2)
			{
				pwmA[i] = 0;
   			pwmB[i]=pwmB[i]*0.95;
				if(pwmB[i]<50)pwmB[i]=0;
				switch(i)
				{
					case 0:
						CH1_A = 0;
						CH1_B = 1;
						TIM_SetCompare1(TIM8,pwmB[i]);
						break;
					case 1:
						CH2_A = 0;
						CH2_B = 1;
						TIM_SetCompare2(TIM8,pwmB[i]);
						break;
					default: break;	
				}
		   }
	  }

	}
 
}
