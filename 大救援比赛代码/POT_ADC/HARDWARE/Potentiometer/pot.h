#ifndef _POT_H
#define _POT_H

#include "adc.h"
#include "led.h"
#include "delay.h"
#include "usart.h"	 

void get_pot_adc(void);
void adc_angle_transform(void);

#endif
