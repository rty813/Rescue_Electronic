
#ifndef __CAN_H
#define __CAN_H

#include "stm32f10x.h"

#define Set_ID	(uint32_t)0x002//标准ID要小于11位
#define Main_Track	(Set_ID | 0x000)
#define Side_Track	(Set_ID | 0x001)

extern CanTxMsg TxMessage;
extern CanRxMsg RxMessage;

typedef enum { CAN_TX_READY , CAN_TX_RTR , CAN_TX_ID , CAN_TX_DATA , CAN_TX_CHECK} CAN_TX; 
//u8 Can_Send_Msg(u8* msg,u8 len);

void CAN_Config(void);
void TaskCanCommadDeal(void);
void USB_LP_CAN1_RX0_IRQHandler(void);
u8 Can_Receive_Msg(u8 *buf);
#endif /* __CAN_H */
