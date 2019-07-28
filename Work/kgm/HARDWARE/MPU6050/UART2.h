#ifndef __UART2_H
#define __UART2_H

#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "UART2.h"
#include "JY901.h"

void USART2_Init(unsigned long baudrate);

void UART2_Put_Char(unsigned char DataToSend);

void UART2_Put_String(unsigned char *Str);

void CopeSerial2Data(unsigned char ucData);

void CopeSerial1Data(unsigned char ucData);

void mpu6050_measure(void);

/*************************************************
 * @函数名称: USARTx_Send_Byte
 * @函数功能: 串口单个字节
 * @输入参数: USARTx: 选择发送数组使用的串口号
 *           byte：  要发送的数组首地址
 * @输出参数: 无
 * @返回值  : 无
 * @其它说明: 无
 *************************************************/ 
void USART2_Send_Byte(unsigned char byte); //串口发送一个字节

/*************************************************
 * @函数名称: UARTx_Send_Str
 * @函数功能: 串口发送字符串
 * @输入参数: USARTx: 选择发送数组使用的串口号
 *           *s：    要发送的数组首地址指针
 * @输出参数: 无
 * @返回值  : 无
 * @其它说明: 发送字符串函数应用指针方法
 *************************************************/ 
void UART2_Send_Str(unsigned char *s);

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
void USART_SendArray(USART_TypeDef* USARTx, u8 *buf, u8 len);

#endif

