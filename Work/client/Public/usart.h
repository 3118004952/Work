#ifndef __usart_H
#define __usart_H

#include "system.h" 
#include "stdio.h" 

void USART3_Init(u32 bound);
void USART3_Send(u8 *buf, u8 len);
void USART1_Init(u32 bound);

#endif


