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
 * @��������: USARTx_Send_Byte
 * @��������: ���ڵ����ֽ�
 * @�������: USARTx: ѡ��������ʹ�õĴ��ں�
 *           byte��  Ҫ���͵������׵�ַ
 * @�������: ��
 * @����ֵ  : ��
 * @����˵��: ��
 *************************************************/ 
void USART2_Send_Byte(unsigned char byte); //���ڷ���һ���ֽ�

/*************************************************
 * @��������: UARTx_Send_Str
 * @��������: ���ڷ����ַ���
 * @�������: USARTx: ѡ��������ʹ�õĴ��ں�
 *           *s��    Ҫ���͵������׵�ַָ��
 * @�������: ��
 * @����ֵ  : ��
 * @����˵��: �����ַ�������Ӧ��ָ�뷽��
 *************************************************/ 
void UART2_Send_Str(unsigned char *s);

/*************************************************
 * @��������: USART_SendArray
 * @��������: ���ڷ��Ͷ��ֽ�
 * @�������: USARTx: ѡ��������ʹ�õĴ��ں�
 *            buf   ��Ҫ���͵������׵�ַ
 *            len   ��Ҫ���͵����鳤��
 * @�������: ��
 * @����ֵ  : ��
 * @����˵��: ��
 *************************************************/ 
void USART_SendArray(USART_TypeDef* USARTx, u8 *buf, u8 len);

#endif

