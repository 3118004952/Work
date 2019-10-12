#ifndef _pwm_H
#define _pwm_H

#include "system.h"

/*******************************************************************************
* 函 数 名         : TIM2_CHx_PWM_Init
* 函数功能		     : TIM2通道x PWM初始化函数
* 输    入         : per:重装载值
					           psc:分频系数
* 输    出         : 无
*******************************************************************************/
void TIM2_CHx_PWM_Init(u16 per,u16 psc);
	
void TIM3_CHx_PWM_Init(u16 per,u16 psc);

void pwm_Init(void);

#endif
