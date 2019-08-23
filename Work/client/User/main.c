#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "esp8266_drive.h"
#include "sta_tcpclent_test.h"
#include "string.h"
#include "pwm.h"
#include "zigbee.h"


u32 judge_time = 0;
u8 choice = 0;
u32 wifi_time = 0;
u8 computer_ip = 0;
u8 Wifi_buf[Wifi_Buf_Length];
u8 Wifi_buf_length = 0;
u8 A_ip = 0;
u8 Computer_ip = 1;

extern u8 decode_flag;
extern u8 code[100];

void Judge_Wifi(u8 *buf);


int main()
{
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	USART3_Init(115200);
	USART1_Init(115200);
	TIM2_CHx_PWM_Init(199,7199);//(7200*200)/72000000=0.02=20ms
	TIM3_CHx_PWM_Init(199,7199);
	ESP8266_Init(115200);
	ESP8266_STA_TCPClient_Test();
	ESP8266_Send_AT_Cmd("AT+CIFSR", 0, 0, 500);
	while(1)
	{
		//	zigbee_send(0x1a,0x06,"000");//发送zigbee数据
	  	if(choice == 1 && wifi_time++ == 20000)
		{
			if(Wifi_buf[1] != 'I' && Wifi_buf[2] != 'P' && Wifi_buf[3] != 'D')
			{
				Wifi_buf[0] = '\0';
				Wifi_buf_length = 0;
				choice = 0;
				wifi_time = 0;
			}
			else
			{
				if(Wifi_buf_length != 0)
				{

					Wifi_buf[Wifi_buf_length] = '\0';
					printf("!!!");
					printf("%s   %d\r\n", Wifi_buf, Wifi_buf_length);
					
					Judge_Wifi(Wifi_buf);

				}
				Wifi_buf[0] = '\0';
				Wifi_buf_length = 0;
				choice = 0;
				wifi_time = 0;
			}
		}
		if(judge_time++ > 10000000)
		{
			if(choice == 0)
				ESP8266_Send_AT_Cmd ( "AT+CIPSTART=0,\"TCP\",\"172.20.10.3\",8000", "OK", 0, 500 );
			judge_time = 0;
		}
	}			
}


void Judge_Wifi(u8 *buf)
{
	u8 j;
	for(j = 0; j < Wifi_buf_length; j++)
	{
		if(Wifi_buf[j] == ':')
			break;
	}
	if(Wifi_buf[j + 1] == 'A' && Wifi_buf[j + 2] == 'T')
	{
		if(Wifi_buf[j + 3] == '_')
		{
			computer_ip = Wifi_buf[5] - 48;
		}
		else
		{
			u8 *a = Wifi_buf + j + 1;
			ESP8266_Send_AT_Cmd((char*)a, "OK", 0, 500);
		}
		return;
	}
	u8 s[50];
	int i;
	for(i = j + 1; Wifi_buf[i] != '\0'; i++)
		s[i - j - 1] = Wifi_buf[i];
	s[i - j - 1] = '\0';
	printf("%d", s[0]);
	i = 195 - s[0] + 97;
	if(s[1] == '1')
		TIM_SetCompare1(TIM2, i); //45度,1ms	A0	厚
	if(s[1] == '2')
		TIM_SetCompare2(TIM2, i); //45度,1ms	A0	底
	if(s[1] == '3')
		TIM_SetCompare1(TIM3, i); //45度,1ms	A0	薄
	if(s[1] == '4')
		TIM_SetCompare2(TIM3, i); //45度,1ms	A0	电
	if(s[1] == '5')
		TIM_SetCompare3(TIM3, i); //45度,1ms	A0	脑
	return;
}


//int main(void)
//{
//	int i = 10;
//	SysTick_Init(72);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
//	//TIM2_CHx_PWM_Init(500-1,72-1);   //PWM T=72*10^6/(499+1)/(71+1)=20ms 
//	TIM2_CHx_PWM_Init(199,7199);//(7200*200)/72000000=0.02=20ms
//	TIM3_CHx_PWM_Init(199,7199);
//	USART3_Init(115200);
//	/*
//	高电平脉冲的时间0.5ms的话，舵机转到0度，
//	1ms -45度 1.5ms--90度 2ms--135度2.5ms-180度，
//	只能在0.5~2.5ms之间调节，转动0到180度任意角度
//	*/
//	while(1)
//	{
//	//	for(i = 0;i < 21; i ++)
//		{
////			TIM_SetCompare1(TIM2, 195 - i); //45度,1ms	A0
////			delay_ms(200); 
//		}
////		TIM_SetCompare2(TIM2,190); //45度,1ms	A1
////		delay_ms(500); 
////		TIM_SetCompare1(TIM3,185); //45度,1ms	A6
////		delay_ms(500); 
////		TIM_SetCompare2(TIM3,180); //45度,1ms	A7
////		delay_ms(500); 
////		TIM_SetCompare3(TIM3,175); //45度,1ms	B0
////		
////		delay_ms(500); 
////		printf("45\r\n");
////		TIM_SetCompare1(TIM2,185); //90,1.5ms
////		delay_ms(1); 
////		TIM_SetCompare2(TIM2,185); //90,1.5ms
////		
////		TIM_SetCompare1(TIM3,185); //90,1.5ms
////		delay_ms(1); 
////		TIM_SetCompare2(TIM3,185); //90,1.5ms
////		delay_ms(1); 
////		TIM_SetCompare3(TIM3,185); //90,1.5ms
//		
////		delay_ms(200);      
//	//	printf("90\r\n");

//	}
//}

