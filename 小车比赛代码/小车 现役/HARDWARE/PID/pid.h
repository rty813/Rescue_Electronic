#ifndef __PID_H
#define __PID_H
#include "sys.h"

#define Tine         0.025f
#define Tined        400.0f

#define MAX_INC_PWM	 300
#define MIN_INC_PWM	 -300

#define MAX_PWM      300
#define DEAD_PWM     0
#define Turn_Point   0.5

typedef struct PID
{
		
		double SetPoint; //  Desired Value
		double Proportion; // Proportional Const
		double Integral; // Integral Const
		double Derivative; //  Derivative Const
		
		double Error_1; //Error[-1]
		double Error_2; //Error[-2]
		double Error_3; //Error[-3]
		double iIncpid; //����
		
} PID;
	float Position_PID(int Encoder,int Target,int i);
float IncPIDCalc(PID *PIDx ,double NextPoint, int i);

void TaskMotorSpeedAdj(void);
double Speed_Choose(unsigned char i, double *speed,double *Target_Speed_Prv,double *Target_Speed_New);
#endif
