#ifndef GAS_H
#define GAS_H

#define MQ135	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)//��Ϊ1ʱ��ʾ�쳣
#define MQ7		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)//��Ϊ1ʱ��ʾ�쳣
#define MQ6		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)//��Ϊ0ʱ��ʾ�쳣
#define MQ4		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)//��Ϊ0ʱ��ʾ�쳣

/*******************************************************************************
* �� �� ��         : GAS_Init()
* ��������		   : ���崫����IO�ڳ�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void GAS_Init(void);


#endif

