#ifndef __P_BSP_H
#define __P_BSP_H

#include "stm32f10x.h"
#include "stdio.h"
#include "queue.h"
#include "delay.h"
#include "usart.h"
#include "can.h"

#define PRINT_MAX	10		//单次打印最多字节

#define MIN(x,y)	((x)<(y)?(x):(y))
#define MAX(x,y)	((x)<(y)?(y):(x))


void Bsp_init(void);
void NVIC_Group_Init(void);

#endif

