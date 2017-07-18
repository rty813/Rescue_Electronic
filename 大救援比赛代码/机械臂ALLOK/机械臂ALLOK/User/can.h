
#ifndef __CAN_H
#define __CAN_H

#include "stm32f10x.h"

 
#define CAN_ID	CAN_Id_Standard	//CAN_Id_Extended,	Standard Id or Extended Id
#define Set_ID	0x02//(uint32_t)0x002//标准ID要小于11位,拓展ID小于29位
#define Main_Track	Set_ID | 0x000
#define Side_Track	Set_ID | 0x001
#define LEFT		1	//左对齐是否开启

typedef enum { CAN_TX_READY , CAN_TX_RTR , CAN_TX_ID , CAN_TX_DATA , CAN_TX_CHECK} TEST_CAN_STA; 
typedef enum { CAN_START , CAN_DATAEXT , CAN_REMOTEEXT , CAN_DATASTD , CAN_REMOTESTD} SER_CAN_STA; 

uint8_t SER_CAN_SetMsg(CanTxMsg TxMessage);
uint8_t TEST_CAN_SetMsg(CanTxMsg *TxMessage);
void CAN_Config(void);
void TaskCanCommadDeal(void);

extern	CanTxMsg 	TxMessage;
extern	CanRxMsg	RxMessage;

#endif /* __CAN_H */
