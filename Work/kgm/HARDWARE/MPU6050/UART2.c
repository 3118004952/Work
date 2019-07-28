#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "UART2.h"
#include "JY901.h"

static unsigned char TxBuffer[256];
static unsigned char TxCounter=0;
static unsigned char count=0;

struct SAcc		stcAcc;
struct SGyro	stcGyro;
struct SAngle 	stcAngle;

struct Measure measure;

void USART2_Init(unsigned long baudrate)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 
	
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);    
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_ClearFlag(USART2,USART_FLAG_TC);	
	USART_Cmd(USART2, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		USART_SendData(USART2, TxBuffer[TxCounter++]); 
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
		if(TxCounter == count) USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
	else if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		CopeSerial2Data((unsigned char)USART2->DR);//处理数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	USART_ClearITPendingBit(USART2,USART_IT_ORE);
}


void UART2_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;  
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  
}

void UART2_Put_String(unsigned char *Str)
{
	while(*Str)
	{
		if(*Str=='\r')UART2_Put_Char(0x0d);
			else if(*Str=='\n')UART2_Put_Char(0x0a);
				else UART2_Put_Char(*Str);
		Str++;
	}
}

//CopeSerialData为串口2中断调用函数，串口每收到一个数据，调用一次这个函数。
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) //数据头不对，则重新开始寻找0x55数据头
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11)
	{
		return;//数据不满11个，则返回
	}
	else
	{
		switch(ucRxBuffer[1])
		{
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;//memcpy为编译器自带的内存拷贝函数，需引用"string.h"，将接收缓冲区的字符拷贝到数据共同体里面，从而实现数据的解析。
			case 0x52:	memcpy(&stcGyro,&ucRxBuffer[2],8);break;
			case 0x53:	memcpy(&stcAngle,&ucRxBuffer[2],8);break;
			default : break;
		}
		ucRxCnt=0;
	}
}

void mpu6050_measure(void)
{
	measure.Acceleration_X = (float)stcAcc.a[0] / 32768 * 16;
	measure.Acceleration_Y = (float)stcAcc.a[1] / 32768 * 16;
	measure.Acceleration_Z = (float)stcAcc.a[2] / 32768 * 16;
	
	measure.Gyro_X = (float)stcGyro.w[0] / 32768 * 2000;
	measure.Gyro_Y = (float)stcGyro.w[1] / 32768 * 2000;
	measure.Gyro_Z = (float)stcGyro.w[2] / 32768 * 2000;
	
	measure.Angle_X = (float)stcAngle.Angle[0] / 32768 * 180;
	measure.Angle_Y = (float)stcAngle.Angle[1] / 32768 * 180;
	measure.Angle_Z = (float)stcAngle.Angle[2] / 32768 * 180;
}


/*************************************************
 * @函数名称: USARTx_Send_Byte
 * @函数功能: 串口单个字节
 * @输入参数: USARTx: 选择发送数组使用的串口号
 *           byte：  要发送的数组首地址
 * @输出参数: 无
 * @返回值  : 无
 * @其它说明: 无
 *************************************************/ 
void USART2_Send_Byte(unsigned char byte) //串口发送一个字节
{
    USART_SendData(USART2, byte); //通过库函数发送数据
	while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);  
        //等待发送完成。  检测 USART_FLAG_TC 是否置1；     
}
/*************************************************
 * @函数名称: UARTx_Send_Str
 * @函数功能: 串口发送字符串
 * @输入参数: USARTx: 选择发送数组使用的串口号
 *           *s：    要发送的数组首地址指针
 * @输出参数: 无
 * @返回值  : 无
 * @其它说明: 发送字符串函数应用指针方法
 *************************************************/ 
void UART2_Send_Str(unsigned char *s)
{
	unsigned char i=0; //定义一个局部变量用来发送字符串
	while(s[i]!='\0') //每个字符串结尾 都是以"\0" 结尾的
	{ 
		 /*通过库函数发送数据*/
		USART_SendData(USART2,s[i]); 
		 /*等待发送完成。检测 USART_FLAG_TC 是否置1*/
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);  
		i++;   //i++一次
	}
}
/*************************************************
 * @函数名称: USART_SendArray
 * @函数功能: 串口发送多字节
 * @输入参数: USARTx: 选择发送数组使用的串口号
 *            buf   ：要发送的数组首地址
 *            len   ：要发送的数组长度
 * @输出参数: 无
 * @返回值  : 无
 * @其它说明: 无
 *************************************************/ 
void USART_SendArray(USART_TypeDef* USARTx, u8 *buf, u8 len)
{   
	u8 t;
	for(t=0;t<len;t++)//循环发送数据
	{
		//等待发送结束        
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET){}; 
		//发送数据
		USART_SendData(USARTx,buf[t]); 
	}   
	//等待发送结束     
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);      
}
