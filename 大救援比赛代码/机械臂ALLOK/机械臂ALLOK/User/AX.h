#include <stm32f10x.h>

#ifndef AX_H
#define AX_H
/*
void getServoConnective(void);
void disableServo(uint8_t *p, uint8_t num);
void enableServo(uint8_t *p, uint8_t num);
void getServoPosition(uint8_t *p, uint8_t num);
void moveServoPosition(uint8_t *p, uint8_t num);
void moveServoPosWithSpeed(uint8_t *p, uint8_t num);
void playMicroAction(uint8_t *p, uint16_t poolSize);

void TxPacketBroadSynWrite(uint8_t bInstruction, uint8_t bParameterLength);
uint8_t axSendSpeed(uint8_t bID, uint16_t target_speed);
*/

uint8_t TxPacket(uint8_t bID, uint8_t bInstruction, uint8_t bParameterLength);
uint8_t RxPacket(uint8_t bRxPacketLength);
/*
uint8_t axPing(uint8_t bID);
uint8_t axTorqueOn(uint8_t bID);
uint8_t axTorqueOff(uint8_t bID);
void axTorqueOffAll(void);
void axTorqueOnAll(void);
*/
uint8_t reset(uint8_t bID);
uint8_t resetall(void);
void changeServoID(uint8_t p);
void getServoConnective(void) ;
void Packet_Reply(USART_TypeDef* USARTx, unsigned char InstrType,unsigned char * data, unsigned int length);
uint8_t axSendPosition(uint8_t bID, uint16_t target_pos, uint16_t target_speed);
uint16_t axReadPosition(uint8_t bID);
uint16_t axReadSpeed(uint8_t bID);
void axTorqueOffAll(void);
void axTorqueOnAll(void);
uint8_t axTorqueOn(uint8_t bID);
uint8_t axTorqueOff(uint8_t bID);

//--- Control Table Address ---
//EEPROM AREA
#define P_MODEL_NUMBER_L      0
#define P_MODOEL_NUMBER_H     1
#define P_VERSION             2
#define P_ID                  3
#define P_BAUD_RATE           4
#define P_RETURN_DELAY_TIME   5
#define P_CW_ANGLE_LIMIT_L    6
#define P_CW_ANGLE_LIMIT_H    7
#define P_CCW_ANGLE_LIMIT_L   8
#define P_CCW_ANGLE_LIMIT_H   9
#define P_SYSTEM_DATA2        10
#define P_LIMIT_TEMPERATURE   11
#define P_DOWN_LIMIT_VOLTAGE  12
#define P_UP_LIMIT_VOLTAGE    13
#define P_MAX_TORQUE_L        14
#define P_MAX_TORQUE_H        15
#define P_RETURN_LEVEL        16
#define P_ALARM_LED           17
#define P_ALARM_SHUTDOWN      18
#define P_OPERATING_MODE      19
#define P_DOWN_CALIBRATION_L  20
#define P_DOWN_CALIBRATION_H  21
#define P_UP_CALIBRATION_L    22
#define P_UP_CALIBRATION_H    23

#define P_TORQUE_ENABLE         (24)
#define P_LED                   (25)
#define P_CW_COMPLIANCE_MARGIN  (26)
#define P_CCW_COMPLIANCE_MARGIN (27)
#define P_CW_COMPLIANCE_SLOPE   (28)
#define P_CCW_COMPLIANCE_SLOPE  (29)
#define P_GOAL_POSITION_L       (30)
#define P_GOAL_POSITION_H       (31)
#define P_GOAL_SPEED_L          (32)
#define P_GOAL_SPEED_H          (33)
#define P_TORQUE_LIMIT_L        (34)
#define P_TORQUE_LIMIT_H        (35)
#define P_PRESENT_POSITION_L    (36)
#define P_PRESENT_POSITION_H    (37)
#define P_PRESENT_SPEED_L       (38)
#define P_PRESENT_SPEED_H       (39)
#define P_PRESENT_LOAD_L        (40)
#define P_PRESENT_LOAD_H        (41)
#define P_PRESENT_VOLTAGE       (42)
#define P_PRESENT_TEMPERATURE   (43)
#define P_REGISTERED_INSTRUCTION (44)
#define P_PAUSE_TIME            (45)
#define P_MOVING (46)
#define P_LOCK                  (47)
#define P_PUNCH_L               (48)
#define P_PUNCH_H               (49)

//--- Instruction ---
#define INST_PING           0x01
#define INST_READ           0x02
#define INST_WRITE          0x03
#define INST_REG_WRITE      0x04
#define INST_ACTION         0x05
#define INST_RESET          0x06
#define INST_DIGITAL_RESET  0x07
#define INST_SYSTEM_READ    0x0C
#define INST_SYSTEM_WRITE   0x0D
#define INST_SYNC_WRITE     0x83
#define INST_SYNC_REG_WRITE 0x84

#define CLEAR_BUFFER gbRxBufferReadPointer = gbRxBufferWritePointer
#define DEFAULT_RETURN_PACKET_SIZE 6
#define BROADCASTING_ID 0xfe

#define  AX_TXD  GPIO_SetBits(GPIOB,GPIO_Pin_12); GPIO_ResetBits(GPIOB,GPIO_Pin_13);
#define  AX_RXD  GPIO_SetBits(GPIOB,GPIO_Pin_13); GPIO_ResetBits(GPIOB,GPIO_Pin_12);

#define ACTION_COMPLETE 0
#define ACTION_ERROR 1

#define AX_MAX_NUM 30//最大舵机编号，检测舵机号只检测从0到AX_MAX_NUM-1。
#define PNULL 0

#define DEFAULTSPEED 512


//下位机的回复包的指令集合
#define	OK                      0x01// 返回状态正常或该指令完成
//#define	ResServoPosInfo         0x01// 请求舵机位置信息
#define	CheckError              0x02// 校验位错误
//#define	InvalidPosition         0x03// 插入或删除位置出现错误
#define	NoSuchServo             0x00// 没有这个舵机号码
//#define	NoSuchType              0x05// 没有这种数据包类型
//#define	NoSuchInstructionType 	0x06// 没有这种指令类型
//#define NoSuchFile              0x07// 没有fianlfile文件
//#define HaveFile                0x08// 存在finalfile文件,但没有动作序列
//#define HaveActionFrame         0x09// 文件中存在动作序列
#define ServoIDNumber           0x0a// 舵机的连通的ID号

#define Mode_AutoON             0x00
#define Mode_Auto               0x01
#define Mode_AutoOFF            0x02
#define Mode_Normal             0x03

extern volatile uint8_t gbpParameter[];
extern volatile uint8_t gbpRxBuffer[];
extern volatile unsigned char gbpRxInterruptBuffer[];
extern volatile unsigned char gbRxBufferWritePointer;

#endif

