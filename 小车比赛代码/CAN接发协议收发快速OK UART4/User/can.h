
#ifndef __CAN_H
#define __CAN_H

#include "stm32f10x.h"

#define Set_ID	(uint32_t)0x002//标准ID要小于11位
#define Main_Track	Set_ID | 0x000
#define Side_Track	Set_ID | 0x001

extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;

typedef enum { CAN_START , CAN_DATAEXT , CAN_REMOTEEXT , CAN_DATASTD , CAN_REMOTESTD} CAN_STA; 

uint8_t CAN_SetMsg(void);
void CAN_Config(void);
void TaskCanCommadDeal(void);
void USB_LP_CAN1_RX0_IRQHandler(void);

#endif /* __CAN_H */
