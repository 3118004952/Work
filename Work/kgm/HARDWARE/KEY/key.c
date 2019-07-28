#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"  
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʼ��KEY0-->GPIOA.13,KEY1-->GPIOA.15  ��������
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTEʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; ////PA0���ó����룬Ĭ������	
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//PA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; ////PA0��������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA0

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PC5�������� 
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA.0

}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY1����
//2��KEY2���� 
//3��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return 1;
		else if(KEY2==0)return 2;
		else if(KEY3==1)return 3;
	}else if(KEY1==1&&KEY2==1&&KEY3==0)key_up=1; 	    
 	return 0;// �ް�������
}
