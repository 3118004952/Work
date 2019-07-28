#include "gas.h"
#include "stm32f10x.h"

/*******************************************************************************
* 函 数 名         : GAS_Init()
* 函数功能		   : 气体传感器IO口初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void GAS_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_6 ;//MQ135 和 MQ7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; ////PA0设置成输入，默认下拉	
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_4;//MQ6 和 MQ4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; ////PA0设置上拉
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA0

}

