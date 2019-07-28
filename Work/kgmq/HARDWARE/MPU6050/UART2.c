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
		CopeSerial2Data((unsigned char)USART2->DR);//��������
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

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(unsigned char ucData)
{
	static unsigned char ucRxBuffer[250];
	static unsigned char ucRxCnt = 0;	
	
	ucRxBuffer[ucRxCnt++]=ucData;
	if (ucRxBuffer[0]!=0x55) //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
	{
		ucRxCnt=0;
		return;
	}
	if (ucRxCnt<11)
	{
		return;//���ݲ���11�����򷵻�
	}
	else
	{
		switch(ucRxBuffer[1])
		{
			case 0x51:	memcpy(&stcAcc,&ucRxBuffer[2],8);break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݹ�ͬ�����棬�Ӷ�ʵ�����ݵĽ�����
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
 * @��������: USARTx_Send_Byte
 * @��������: ���ڵ����ֽ�
 * @�������: USARTx: ѡ��������ʹ�õĴ��ں�
 *           byte��  Ҫ���͵������׵�ַ
 * @�������: ��
 * @����ֵ  : ��
 * @����˵��: ��
 *************************************************/ 
void USART2_Send_Byte(unsigned char byte) //���ڷ���һ���ֽ�
{
    USART_SendData(USART2, byte); //ͨ���⺯����������
	while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);  
        //�ȴ�������ɡ�  ��� USART_FLAG_TC �Ƿ���1��     
}
/*************************************************
 * @��������: UARTx_Send_Str
 * @��������: ���ڷ����ַ���
 * @�������: USARTx: ѡ��������ʹ�õĴ��ں�
 *           *s��    Ҫ���͵������׵�ַָ��
 * @�������: ��
 * @����ֵ  : ��
 * @����˵��: �����ַ�������Ӧ��ָ�뷽��
 *************************************************/ 
void UART2_Send_Str(unsigned char *s)
{
	unsigned char i=0; //����һ���ֲ��������������ַ���
	while(s[i]!='\0') //ÿ���ַ�����β ������"\0" ��β��
	{ 
		 /*ͨ���⺯����������*/
		USART_SendData(USART2,s[i]); 
		 /*�ȴ�������ɡ���� USART_FLAG_TC �Ƿ���1*/
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);  
		i++;   //i++һ��
	}
}
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
void USART_SendArray(USART_TypeDef* USARTx, u8 *buf, u8 len)
{   
	u8 t;
	for(t=0;t<len;t++)//ѭ����������
	{
		//�ȴ����ͽ���        
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET){}; 
		//��������
		USART_SendData(USARTx,buf[t]); 
	}   
	//�ȴ����ͽ���     
    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)==RESET);      
}
