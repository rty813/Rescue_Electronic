#include "gy521.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//24CXX驱动 代码(适合24C01~24C16,24C32~256未经过测试!有待验证!)
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////



//初始化IIC接口
void GY521_Init(void)
{
	IIC_Init();
	GY521_WriteOneByte(PWR_MGMT_1, 0x00);	//解除休眠状态
	GY521_WriteOneByte(SMPLRT_DIV, 0x07);
	GY521_WriteOneByte(CONFIG, 0x06);
	GY521_WriteOneByte(GYRO_CONFIG, 0x18);
	GY521_WriteOneByte(ACCEL_CONFIG, 0x01);
}
//在GY521指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
u8 GY521_ReadOneByte(u16 ReadAddr)
{
	u8 temp=0;
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);
	IIC_Wait_Ack();
	IIC_Send_Byte(ReadAddr);   //发送低地址
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(SlaveAddress + 1);           //进入接收模式
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0);
	IIC_Stop();//产生一个停止条件
	return temp;
}
//在GY521指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void GY521_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{
	IIC_Start();
	IIC_Send_Byte(SlaveAddress);
	IIC_Wait_Ack();
	IIC_Send_Byte(WriteAddr);   //发送低地址
	IIC_Wait_Ack();
	IIC_Send_Byte(DataToWrite);     //发送字节
	IIC_Wait_Ack();
	IIC_Stop();//产生一个停止条件
	delay_ms(10);
}


