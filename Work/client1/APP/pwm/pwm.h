#ifndef _pwm_H
#define _pwm_H

#include "system.h"

/*******************************************************************************
* �� �� ��         : TIM2_CHx_PWM_Init
* ��������		     : TIM2ͨ��x PWM��ʼ������
* ��    ��         : per:��װ��ֵ
					           psc:��Ƶϵ��
* ��    ��         : ��
*******************************************************************************/
void TIM2_CHx_PWM_Init(u16 per,u16 psc);
	
void TIM3_CHx_PWM_Init(u16 per,u16 psc);

void pwm_Init(void);

#endif
