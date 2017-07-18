#include "stm32f10x.h"
#include "usart.h"
#include "AX.h"

#define RX_TIMEOUT_COUNT2   500L
#define RX_TIMEOUT_COUNT1  (RX_TIMEOUT_COUNT2*10L)

volatile uint8_t gbpParameter[128];
volatile uint8_t gbRxBufferReadPointer;
volatile uint8_t gbpRxBuffer[128];
volatile uint8_t gbpTxBuffer[128];
volatile uint8_t gbRxBufferWritePointer;
volatile uint8_t gbpRxInterruptBuffer[256];
volatile uint8_t Timer_count;
volatile uint8_t axOline[AX_MAX_NUM];
volatile uint8_t axOlineNum;
volatile uint8_t delayCount;



/*uint8_t TxPacket(uint8_t bID,//ID�� uint8_t bInstruction,//ָ����� uint8_t bParameterLength//ָ���)
 * return ʵ�ʷ������ݰ�����
 *����ָ���*/

uint8_t TxPacket(uint8_t bID, uint8_t bInstruction, uint8_t bParameterLength)
	{

	uint8_t bCount, bPacketLength;
	uint8_t bCheckSum;
	gbpTxBuffer[0] = 0xff;
	gbpTxBuffer[1] = 0xff;
	gbpTxBuffer[2] = bID;
	gbpTxBuffer[3] = bParameterLength + 2;
//Length(Paramter,Instruction,Checksum) 
	gbpTxBuffer[4] = bInstruction;
	for (bCount = 0; bCount < bParameterLength; bCount++) {
		gbpTxBuffer[bCount + 5] = gbpParameter[bCount];//ָ������
	}
	bCheckSum = 0;//У��λ
	bPacketLength = bParameterLength + 4 + 2;//ָ����ܳ���
	for (bCount = 2; bCount < bPacketLength - 1; bCount++) //except 0xff,checksum

			{
		bCheckSum += gbpTxBuffer[bCount];
	}
	gbpTxBuffer[bCount] = (uint8_t) (~bCheckSum); //Writing Checksum  with  Bit Inversion
	 CLEAR_BUFFER;
	AX_TXD
	for (bCount = 0; bCount < bPacketLength; bCount++) {
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
		USART_SendData(USART3, gbpTxBuffer[bCount]);
	}
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    AX_RXD
	return (bPacketLength);
}



/*uint8_t RxPacket(uint8_t bRxPacketLength)
 * bRxPacketLength ����׼�����յ����ݳ���
 * return ʵ�ʷ��ص����ݳ���
 *  ����ָ���*/
uint8_t RxPacket(uint8_t bRxPacketLength) {

	unsigned long ulCounter;
	uint8_t bCount, bLength, bChecksum;
	uint8_t bTimeout;

	bTimeout = 0;//��ʱ��
	/*����ѭ��
	 * �����ܵ�ָ�����ȵ����ݻ��߳�ʱ��ʱ���˳�ѭ��*/
	for (bCount = 0; bCount < bRxPacketLength; bCount++) 
	{
		ulCounter = 0;
		/*�����жϽ�������
		 * �������ڵȴ�״̬
		 * ���жϻ���������û������gbRxBufferReadPointer == gbRxBufferWritePointer
		 * ʱ�����������
		 * ����ʱ�����жϻ��������������ݵ�ʱ�������ȴ�*/
	while (gbRxBufferReadPointer == gbRxBufferWritePointer) 
		{
			if (ulCounter++ > RX_TIMEOUT_COUNT1) 
				{
				bTimeout = 1;
				break;
			  }
		}
		if (bTimeout)
			break;
		gbpRxBuffer[bCount] = gbpRxInterruptBuffer[gbRxBufferReadPointer++];
	} ///��������ݶ����ж϶����зŵ�RxBuffer������
	bLength = bCount;
	bChecksum = 0;
	/*Ĭ�������TxPacket��RxPacket����ʹ��
	 * ����һ�����ݰ�����BROADCASTING_ID//�㲥id ʱ�з��ص�����*/
	if (gbpTxBuffer[2] != BROADCASTING_ID) {
		if (bTimeout && (bRxPacketLength != 255)) {
			//TxDString("\r\n [Error:RxD Timeout]");
			CLEAR_BUFFER; //��ս��ջ�����
		}

		if (bLength > 3) //checking is available.
				{
			if (gbpRxBuffer[0] != 0xff || gbpRxBuffer[1] != 0xff) {
				//TxDString("\r\n [Error:Wrong Header]");
				CLEAR_BUFFER;
				return 0;
			}
			if (gbpRxBuffer[2] != gbpTxBuffer[2]) {
				//TxDString("\r\n [Error:TxID != RxID]");
				CLEAR_BUFFER;
				return 0;
			}
			if (gbpRxBuffer[3] != bLength - 4) {
				//TxDString("\r\n [Error:Wrong Length]");
				CLEAR_BUFFER;
				return 0;
			}
			for (bCount = 2; bCount < bLength; bCount++)
				bChecksum += gbpRxBuffer[bCount];
			if (bChecksum != 0xff) {
				//TxDString("\r\n [Error:Wrong CheckSum]");
				CLEAR_BUFFER;
				return 0;
			}
		}
	}
	return bLength;
}


/* ���߶������
 * ���ܼ��ID�ķ�Χ0~AX_MAX_NUM-1
 * ������ߵĶ�����뱣����ȫ������axOline[30]
 * ������ߵĶ����������ȫ������axOlineNum
 * */
	
void getServoConnective(void) 
{							 //PING����Ӽ��ſ�ʼ
	uint8_t bCount;
	axOlineNum = 0;
	for (bCount = 0; bCount < AX_MAX_NUM - 1; bCount++) ///��һ��ѭ����ѯÿһ�������״̬Ϊ�˿�������ֻɨ��0x00~0x1f
	{
		TxPacket(bCount, INST_PING, 0);
		if (RxPacket(255) == DEFAULT_RETURN_PACKET_SIZE) //������ذ��ĳ�����ȷ
		axOline[axOlineNum++] = bCount;
	}

	Packet_Reply(USART1, ServoIDNumber, (uint8_t*) &axOline[0], axOlineNum);
   
}

void changeServoID(uint8_t p)
{
	gbpParameter[0] = P_ID; //���ID��ַ
	gbpParameter[1] = p;
	TxPacket(BROADCASTING_ID,INST_WRITE,2);
}

uint8_t axTorqueOff(uint8_t bID) ///�ͷŶ��������
{
	gbpParameter[0] = P_TORQUE_ENABLE; //Address of LED ����Ť��ָ�� ��Ӧ��ַ
	gbpParameter[1] = 0; //Writing Data
	TxPacket(bID, INST_WRITE, 2);
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
		return OK;
	else
		return NoSuchServo;
}
uint8_t axTorqueOn(uint8_t bID) ///ʹ�ܶ��������
{
	gbpParameter[0] = P_TORQUE_ENABLE; //Address of LED 
	gbpParameter[1] = 1; //Writing Data
	TxPacket(bID, INST_WRITE, 2);//����ָ�����
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
		return OK;
	else
		return NoSuchServo;
}

/*
void axTorqueOffAll(void) { 
	gbpParameter[0] = P_TORQUE_ENABLE;
	gbpParameter[1] = 0x00;
	TxPacket(BROADCASTING_ID, INST_WRITE, 2);
 
}
void axTorqueOnAll(void) {
	gbpParameter[0] = P_TORQUE_ENABLE;
	gbpParameter[1] = 0x01;
	TxPacket(BROADCASTING_ID, INST_WRITE, 2);
}
*/

/*
void changeBaudRateAll(void) {
	gbpParameter[0] = P_BAUD_RATE;
	gbpParameter[1] = 0x01;
	TxPacket(BROADCASTING_ID, INST_WRITE, 2);
}
*/


//�ָ������������
uint8_t reset(uint8_t bID)
{
	gbpParameter[0] = 0X06;
	TxPacket(bID, 0X06, 0);
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
	     return OK;
	else
		return NoSuchServo;
}

uint8_t resetall(void)
{
	gbpParameter[0] = 0X06;
	TxPacket(BROADCASTING_ID, 0X06, 0);
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
	     return OK;
	else
		return NoSuchServo;
}


///���͸�ĳһ�����һ��Ҫ�ƶ�����λ�ã������������� 1ID 2λ�� 3�ٶ�
uint8_t axSendPosition(uint8_t bID, uint16_t target_pos, uint16_t target_speed)
{
	gbpParameter[0] = P_GOAL_POSITION_L; //Address of Firmware Version
	gbpParameter[1] = target_pos; //Writing Data P_GOAL_POSITION_L
	gbpParameter[2] = target_pos >> 8; //Writing Data P_GOAL_POSITION_H
	gbpParameter[3] = target_speed; //Writing Data P_GOAL_SPEED_L
	gbpParameter[4] = target_speed >> 8; //Writing Data P_GOAL_SPEED_H
	TxPacket(bID, INST_WRITE, 5);
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
	     return OK;
	else
		return NoSuchServo;
}



//��ȡ��ǰ���λ��
/*uint16_t axReadPosition(uint8_t bID)
 * parameter
 * 		bID �����
 * return value
 * 		���λ��
 * 		0xffff��ʾ������
 *��ȡ��ǰ���λ(����)*/
uint16_t axReadPosition(uint8_t bID) {
	unsigned int Position;
	gbpParameter[0] = P_PRESENT_POSITION_L; //λ�����ݵ���ʼ��ַ #define P_GOAL_POSITION_L (30) �μ������ֲ�
	gbpParameter[1] = 2; //��ȡ����
	TxPacket(bID, INST_READ, 2);
	if(RxPacket(DEFAULT_RETURN_PACKET_SIZE + gbpParameter[1]) != DEFAULT_RETURN_PACKET_SIZE + gbpParameter[1]){
		Position = 0xFFFF;
	}
	else{
		
		Position = ((unsigned int) gbpRxBuffer[6]) << 8;
		Position += gbpRxBuffer[5];
	}

//Position = ((unsigned int) gbpRxBuffer[6]) << 8;
	//	Position += gbpRxBuffer[5];
return Position;
}


/*uint16_t axReadSpeed(uint8_t bID)
 * parameter
 * 		bID �����
 * return value
 * 		����ٶ�
 * 		0xffff��ʾ������
*��ȡ��ǰ����ٶ�*/
uint16_t axReadSpeed(uint8_t bID) {
	unsigned int Speed;
	gbpParameter[0] = P_GOAL_SPEED_L; //�ٶ����ݵ���ʼ��ַ #define P_GOAL_SPEED_L (32) �μ������ֲ�
	gbpParameter[1] = 2; //��ȡ����
	TxPacket(bID, INST_READ, 2);
	if(RxPacket(DEFAULT_RETURN_PACKET_SIZE + gbpParameter[1]) != DEFAULT_RETURN_PACKET_SIZE + gbpParameter[1]){
		Speed = 0xffff;
	}else{
//		USART_SendData(USART1, gbpRxBuffer[6]);
//   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) 
//		;
//	 USART_SendData(USART1, gbpRxBuffer[6]);
//	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) 
//	 ;
		Speed = ((unsigned int) gbpRxBuffer[6]) << 8;
		Speed += gbpRxBuffer[5];
	}
	return Speed;
}


//InstrType�����ظ�ָ�����ͣ�data���������ݵ�ָ��,length����Ҫ���͵�Ҳ�����ҵ��¼ҷ��ظ��ҵ����ݳ��ȣ�Ҫ����͸���λ��
void Packet_Reply(USART_TypeDef* USARTx, unsigned char InstrType,unsigned char * data, unsigned int length) 						
	{			   //lengthֻ����data�ĳ���

	 unsigned char Length_H, Length_L, Check_Sum = 0;
	 unsigned int j = 0;
	 Length_H = (length + 6) >> 8;
	 Length_L = (length + 6);
	 
//	 RS485_TX_MODE		   //��Ƭ������485����	
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)	 //�������ݼĴ����ձ�־λ
	 ;

	 USART_SendData(USARTx, 0XFF); //��ͷ
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Length_L); //����l
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + Length_L;
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Length_H); //����2
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + Length_H;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, 0); //ID =0,�������ֶ��
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, 2); //�������ǻظ���Ϣ
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + 2;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, InstrType); //����ָ������
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + InstrType;

	 //������Ч����
	for (j = 0; j < length; j++) 
	 {
		 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
		 ;
	
		 USART_SendData(USARTx, *(data + j));
		 Check_Sum = Check_Sum + *(data + j);
		 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
		 ;
	 }
	 Check_Sum = ~Check_Sum; //����У���

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Check_Sum); //����ָ������
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;

	 Check_Sum = 0;
 //    RS485_RX_MODE		 //����485 
}
