#ifndef GAS_H
#define GAS_H

#define MQ135	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//当为1时表示异常
#define MQ7		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//当为1时表示异常
#define MQ6		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//当为0时表示异常
#define MQ4		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//当为0时表示异常

/*******************************************************************************
* 函 数 名         : GAS_Init()
* 函数功能		   : 气体传感器IO口初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void GAS_Init(void);


#endif

