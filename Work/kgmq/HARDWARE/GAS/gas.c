#include "gas.h"
#include "stm32f10x.h"

/*******************************************************************************
* �� �� ��         : GAS_Init()
* ��������		   : ���崫����IO�ڳ�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void GAS_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��KEY0-->GPIOA.13,KEY1-->GPIOA.15  ��������
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_6 ;//MQ135 �� MQ7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; ////PA0���ó����룬Ĭ������	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_4;//MQ6 �� MQ4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; ////PA0��������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA0

}

