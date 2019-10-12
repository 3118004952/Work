/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdio.h"
#include "esp8266_drive.h"
#include "SysTick.h"
#include <stdio.h>  
#include <string.h>  
#include <stdbool.h>
#include "sta_tcpclent_test.h"
#include "esp8266_public.h"
#include "zigbee.h"
#include "queue.h"

extern LQueue *Wifi_queue;
extern u8 *Wifi_buf;
extern u8 Wifi_buf_length;
extern u8 choice;
extern u32 wifi_time;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/


/*******************************************************************************
* 函 数 名         : TIM4_IRQHandler
* 函数功能		   : TIM4中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	
//	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
//	{
//		choice = 1;
//	}
//	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,DISABLE);//失能TIM4时钟
}


/*******************************************************************************
* 函 数 名         : USART3_IRQHandler
* 函数功能		   : USART3中断函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/ 
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	u8 r;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		r =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
		
		receive_data(r);
		USART_SendData(USART3,r);
	}
	USART_ClearFlag(USART3,USART_FLAG_TC);
	
}

void USART2_IRQHandler( void )
{	
	u8 ucCh;
	wifi_time = 0;
	if(choice == 0)
	{
		choice = 1;
		Wifi_buf = (u8*)malloc(sizeof(u8) * 100);
	}
	if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART2 );
		Wifi_buf[Wifi_buf_length++] = ucCh;
		if(ESP8266_Fram_Record_Struct .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) ) 
		{
			//预留1个字节写结束符
			ESP8266_Fram_Record_Struct .Data_RX_BUF[ ESP8266_Fram_Record_Struct .InfBit .FramLength ++ ]  = ucCh;	
		}                      
		
	}
	if( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
	{
    	ESP8266_Fram_Record_Struct .InfBit .FramFinishFlag = 1;
		
		ucCh = USART_ReceiveData( USART2 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
		Wifi_buf[Wifi_buf_length++] = ucCh;

		TcpClosedFlag = strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
  	}	
	if(Wifi_buf[0] != '+')
	{
		Wifi_buf_length = 0;
		choice = 0;
		free(Wifi_buf);
	}
	if(choice == 2)
	{
		if(Wifi_buf[Wifi_buf_length - 1] == '%')
		{
			EnLQueue(Wifi_queue, Wifi_buf, Wifi_buf_length);
			Wifi_buf_length = 0;
			choice = 0;
		}
		else
		{
			free(Wifi_buf);
			choice = 0;
			Wifi_buf_length = 0;
		}
	}
	if(Wifi_buf[Wifi_buf_length - 1] == '$')
		choice = 2;
}

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
