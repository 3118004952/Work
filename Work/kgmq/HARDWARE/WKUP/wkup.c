#include "wkup.h"
#include "usart.h"
#include "stm32f10x_pwr.h"

/*******************************************************************************
* �� �� ��         : Enter_Standby_Mode
* ��������		   : �������ģʽ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Enter_Standby_Mode(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ��PWR����ʱ��
	
	PWR_WakeUpPinCmd(ENABLE);//ʹ�ܻ��ѹܽ�	ʹ�ܻ���ʧ�ܻ��ѹܽŹ���
	
	PWR_EnterSTANDBYMode();//�������ģʽ
}

void Enter_Sleep_Mode(void)
{
	__WFI();	//WFIָ�����˯��
}

void Enter_Stop_Mode(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//ʹ��PWR����ʱ��
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}



