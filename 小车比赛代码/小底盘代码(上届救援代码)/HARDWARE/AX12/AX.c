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



/*uint8_t TxPacket(uint8_t bID,//ID号 uint8_t bInstruction,//指令代号 uint8_t bParameterLength//指令长度)
 * return 实际发送数据包长度
 *发送指令包*/

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
		gbpTxBuffer[bCount + 5] = gbpParameter[bCount];//指令数据
	}
	bCheckSum = 0;//校验位
	bPacketLength = bParameterLength + 4 + 2;//指令包总长度
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
 * bRxPacketLength 我们准备接收的数据长度
 * return 实际返回的数据长度
 *  接收指令包*/
uint8_t RxPacket(uint8_t bRxPacketLength) {

	unsigned long ulCounter;
	uint8_t bCount, bLength, bChecksum;
	uint8_t bTimeout;

	bTimeout = 0;//计时器
	/*接受循环
	 * 当接受到指定长度的数据或者超时的时候退出循环*/
	for (bCount = 0; bCount < bRxPacketLength; bCount++) 
	{
		ulCounter = 0;
		/*利用中断接受数据
		 * 主程序处于等待状态
		 * 当中断缓冲区里面没有数据gbRxBufferReadPointer == gbRxBufferWritePointer
		 * 时间计数器自增
		 * 当超时或者中断缓冲区里面有数据的时候跳出等待*/
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
	} ///这里把数据都从中断队列中放到RxBuffer里面了
	bLength = bCount;
	bChecksum = 0;
	/*默认情况下TxPacket与RxPacket连续使用
	 * 当上一个数据包不是BROADCASTING_ID//广播id 时有返回的数据*/
	if (gbpTxBuffer[2] != BROADCASTING_ID) {
		if (bTimeout && (bRxPacketLength != 255)) {
			//TxDString("\r\n [Error:RxD Timeout]");
			CLEAR_BUFFER; //清空接收缓冲区
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


/* 在线舵机总数
 * 所能检测ID的范围0~AX_MAX_NUM-1
 * 检测连线的舵机号码保存在全局数组axOline[30]
 * 检测连线的舵机数保存在全局数组axOlineNum
 * */
	
void getServoConnective(void) 
{							 //PING舵机从几号开始
	uint8_t bCount;
	axOlineNum = 0;
	for (bCount = 0; bCount < AX_MAX_NUM - 1; bCount++) ///用一个循环查询每一个舵机的状态为了快速我们只扫描0x00~0x1f
	{
		TxPacket(bCount, INST_PING, 0);
		if (RxPacket(255) == DEFAULT_RETURN_PACKET_SIZE) //如果返回包的长度正确
		axOline[axOlineNum++] = bCount;
	}

	Packet_Reply(USART1, ServoIDNumber, (uint8_t*) &axOline[0], axOlineNum);
   
}

void changeServoID(uint8_t p)
{
	gbpParameter[0] = P_ID; //舵机ID地址
	gbpParameter[1] = p;
	TxPacket(BROADCASTING_ID,INST_WRITE,2);
}

uint8_t axTorqueOff(uint8_t bID) ///释放舵机的力矩
{
	gbpParameter[0] = P_TORQUE_ENABLE; //Address of LED 激活扭矩指令 对应地址
	gbpParameter[1] = 0; //Writing Data
	TxPacket(bID, INST_WRITE, 2);
	if (RxPacket(DEFAULT_RETURN_PACKET_SIZE) == DEFAULT_RETURN_PACKET_SIZE)
		return OK;
	else
		return NoSuchServo;
}
uint8_t axTorqueOn(uint8_t bID) ///使能舵机的力矩
{
	gbpParameter[0] = P_TORQUE_ENABLE; //Address of LED 
	gbpParameter[1] = 1; //Writing Data
	TxPacket(bID, INST_WRITE, 2);//发送指令包，
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


//恢复舵机出厂设置
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


///发送给某一个舵机一个要移动到的位置，包括三个参数 1ID 2位置 3速度
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



//读取当前舵机位置
/*uint16_t axReadPosition(uint8_t bID)
 * parameter
 * 		bID 舵机号
 * return value
 * 		舵机位置
 * 		0xffff表示出错了
 *读取当前舵机位(单个)*/
uint16_t axReadPosition(uint8_t bID) {
	unsigned int Position;
	gbpParameter[0] = P_PRESENT_POSITION_L; //位置数据的起始地址 #define P_GOAL_POSITION_L (30) 参见数据手册
	gbpParameter[1] = 2; //读取长度
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
 * 		bID 舵机号
 * return value
 * 		舵机速度
 * 		0xffff表示出错了
*读取当前舵机速度*/
uint16_t axReadSpeed(uint8_t bID) {
	unsigned int Speed;
	gbpParameter[0] = P_GOAL_SPEED_L; //速度数据的起始地址 #define P_GOAL_SPEED_L (32) 参见数据手册
	gbpParameter[1] = 2; //读取长度
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


//InstrType代被回复指令类型，data代表返回数据的指针,length代表要发送的也就是我的下家返回给我的数据长度，要打包送给上位机
void Packet_Reply(USART_TypeDef* USARTx, unsigned char InstrType,unsigned char * data, unsigned int length) 						
	{			   //length只包含data的长度

	 unsigned char Length_H, Length_L, Check_Sum = 0;
	 unsigned int j = 0;
	 Length_H = (length + 6) >> 8;
	 Length_L = (length + 6);
	 
//	 RS485_TX_MODE		   //单片机发送485数据	
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)	 //发送数据寄存器空标志位
	 ;

	 USART_SendData(USARTx, 0XFF); //包头
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Length_L); //长度l
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + Length_L;
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Length_H); //长度2
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + Length_H;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, 0); //ID =0,代表数字舵机
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, 2); //代表这是回复信息
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + 2;

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, InstrType); //代表指令类型
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;
	 Check_Sum = Check_Sum + InstrType;

	 //发送有效数据
	for (j = 0; j < length; j++) 
	 {
		 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
		 ;
	
		 USART_SendData(USARTx, *(data + j));
		 Check_Sum = Check_Sum + *(data + j);
		 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
		 ;
	 }
	 Check_Sum = ~Check_Sum; //计算校验和

	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
	 ;

	 USART_SendData(USARTx, Check_Sum); //代表指令类型
	 while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
	 ;

	 Check_Sum = 0;
 //    RS485_RX_MODE		 //接收485 
}
