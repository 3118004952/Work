#include "wkup.h"
#include "usart.h"
#include "stm32f10x_pwr.h"

/*******************************************************************************
* 函 数 名         : Enter_Standby_Mode
* 函数功能		   : 进入待机模式
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Enter_Standby_Mode(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能PWR外设时钟
	
	PWR_WakeUpPinCmd(ENABLE);//使能唤醒管脚	使能或者失能唤醒管脚功能
	
	PWR_EnterSTANDBYMode();//进入待机模式
}

void Enter_Sleep_Mode(void)
{
	__WFI();	//WFI指令进入睡眠
}

void Enter_Stop_Mode(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);//使能PWR外设时钟
	
	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}



