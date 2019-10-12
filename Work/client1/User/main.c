#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "esp8266_drive.h"
#include "sta_tcpclent_test.h"
#include "string.h"
#include "pwm.h"
#include "zigbee.h"
#include "queue.h"


u8 ID[] = "id:1\r\n";

u32 judge_time = 0;
u8 choice = 0;
u32 wifi_time = 0;
u8 *Wifi_buf;
u8 Wifi_buf_length = 0;

LQueue *Wifi_queue;

extern u8 decode_flag;
extern u8 code[100];

void Judge_Wifi(u8 *buf, int length);


int main()
{
	
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	USART3_Init(115200);
	USART1_Init(115200);
	TIM2_CHx_PWM_Init(199,7199);//(7200*200)/72000000=0.02=20ms
	TIM3_CHx_PWM_Init(199,7199);
	pwm_Init();
	Wifi_queue = (LQueue*)malloc(sizeof(LQueue));
	InitLQueue(Wifi_queue);
	ESP8266_Init(115200);
	ESP8266_STA_TCPClient_Test();
	send_data(ID, strlen(ID), 0);//发送该舵机编号
	send_data(ID, strlen(ID), 1);//发送该舵机编号
	send_data(ID, strlen(ID), 2);//发送该舵机编号
	send_data(ID, strlen(ID), 3);//发送该舵机编号
	send_data(ID, strlen(ID), 4);//发送该舵机编号
	while(1)
	{
		//	zigbee_send(0x1a,0x06,"000");//发送zigbee数据
		if(choice == 1 || choice ==2)
		{
			judge_time = 0;//使重连的计数重新开始
			if(wifi_time++ == 200000)//接收超时了，初始化WIFI接收的条件
			{
				choice = 0;
				free(Wifi_buf);
				Wifi_buf_length = 0;
				wifi_time = 0;
			}
		}
		else
			wifi_time = 0;//使判断接收超时的计数重新开始
	  	if(Wifi_queue->length != 0)//判断WIFI数据是否使用完
		{
			u8 *data = Wifi_queue->front->data;
			int length = Wifi_queue->front->length;
			if(data[1] != 'I' && data[2] != 'P' && data[3] != 'D')//判断是否接收正确
			{
				
			}
			else//接收正确
			{
				if(length != 0)
				{

					data[length] = '\0';
					Judge_Wifi(data, length);//开始判断并执行相应操作

				}
				data[0] = '\0';//完成一次操作，初始化接收数值
				length = 0;
			}
			DeLQueue(Wifi_queue);
		}
		if(judge_time++ > 10000000)
		{
			if(choice == 0)
				ESP8266_Send_AT_Cmd ( User_ESP8266_TCPclient, "OK", 0, 500 );
			judge_time = 0;
		}
	}
}

void Judge_computer(u8 *s)//来自编程平台的命令
{
	char *choice;

	char null[] = "";
	char str_A[] = "k4q4";//向前走（公头的前）
	char str_B[] = "k5e5";//向后走（母头的前）
	char str_L[] = "e5q4s2g5k2e5";//向左移动一次（厚的那一端）
	char str_R[] = "e5q4a2g5k2e5";//向右移动一次（薄的那一端）
	
	char str_C_A[] = "a2";//连接    底下
	char str_C_L[] = "a1";//  		左边（厚的那一端）
	char str_C_R[] = "a3";//		右边（薄的那一端）
	
	char str_E_A[] = "s2k4q4k2";//分离并移动一步 	底下
	char str_E_L[] = "s1k4q4k1";//  				左边（厚的那一端）
	char str_E_R[] = "s3k4q4k3";//					右边（薄的那一端）
	
	char str_1[] = "r5";//直接控制角度
	char str_2[] = "d4";
	
	int save[4];//用于保存冒号的位置坐标的下一位
	
	int i = 0;//用于循环
	int j = 0;
	int k = 1;
	int l = 0;
	
	choice = null;//置空
	
	while(i != 3)//一次只有3个冒号
	{
		if(s[j++] == ':')
		{
			save[i++] = j;//记录冒号位置的下一位
		}
		if(j > 50)//防止字符串内无冒号
			return;
	}
	
	switch(s[save[1]])
	{
		case 'a'://向前或向后运动
		case 's'://倾斜
			k = s[save[2]] - 48;
			j = save[2];
			while(s[++j] >= '0' && s[j] <= '9')
				k = k * 10 + s[j] - 48;//计算出步数的值
			switch(s[save[0]])//判断是哪种运动
			{
				case 'm':
					choice = str_A;
					break;
				case 'f':
					choice = str_B;
					break;
				case 'l':
					choice = str_L;
					break;
				case 'r':
					choice = str_R;
					break;
				default:break;
			}
			break;
		case 'c'://链接
			switch(s[save[2]])
			{
				case '2':
					choice = str_C_A;
					break;
				case '1':
					choice = str_C_L;
					break;
				case '3':
					choice = str_C_R;
					break;
				default:break;
			}
			break;
		case 'd'://断开连接
			switch(s[save[2]])
			{
				case '2':
					choice = str_E_A;
					break;
				case '1':
					choice = str_E_L;
					break;
				case '3':
					choice = str_E_R;
					break;
				default:break;
			}
			break;
		case 't'://直接控制角度14
			j = s[save[2]] - 48;
			if(s[save[2] + 1] >= '0' && s[save[2] + 1] <= '9')//计算角度的值
			{
				j = j * 10 + s[save[2] + 1] - 48;
				if(s[save[2] + 2] >= '0' && s[save[2] + 2] <= '9')
				{
					j = j * 100 + s[save[2] + 2] - 48;
					if(j > 180)
						break;
				}
			}
			j = (j + 7) / 14;//由于舵机精度的问题，只能分成13份角度
			switch(s[save[0]])
			{
				case 'm':
					str_1[0] = str_1[0] - j;
					choice = str_1;
					break;
				case 'f':
					str_2[0] = str_2[0] + j;
					choice = str_2;
					break;
				default:break;
			}
			break;
		default:break;
	}
	while(k > 0 && l < 20 && k-- != 0)//用于多个步数
	{
		l = 0;
		while(choice[l] != '\0')//直接控制舵机
		{
			i = 195 - choice[l] + 97;//根据ASCII码得出度数的相应数值
			if(choice[++l] == '1')
				TIM_SetCompare1(TIM2, i); //A0	厚的那一端的舵机
			if(choice[l] == '2')
				TIM_SetCompare2(TIM2, i); //A1	底部的舵机
			if(choice[l] == '3')
				TIM_SetCompare1(TIM3, i); //A6	薄的那一端的舵机
			if(choice[l] == '4')
				TIM_SetCompare2(TIM3, i); //A7	母头舵机
			if(choice[l++] == '5')
				TIM_SetCompare3(TIM3, i); //B0	公头舵机
			delay_ms(400);

		}
	}
	return;
}


void Judge_Wifi(u8 *buf, int length)
{
	u8 j;
	for(j = 0; j < length; j++)
	{
		if(buf[j] == ':')
			break;
	}
	if(buf[j + 1] == 'A' && buf[j + 2] == 'T')
	{
		if(buf[j + 3] == '_')//当平台发送  AT_
		{
			send_data(ID, strlen(ID), buf[5] - 48);//发送该舵机编号
		}
		else//发送wifi模块的设置指令
		{
			buf[length - 2] = '\0';
			u8 *a = buf + j + 1;
			ESP8266_Send_AT_Cmd((char*)a, "OK", 0, 500);
		}
		return;
	}
	int l = j + 1;
	if(buf[l] == '#')//当存在 #
	{	
		while(buf[l] != '\0')//分解指令
		{
			if(buf[l++] == '#')//判断是否来自编程平台发过来的模块运动指令
			{
				Judge_computer(buf + l);
			
			}
		}
		return;
	}
	l = j + 1;
	int i;
	while(buf[l] != '\0' && l < 100)//直接控制舵机
	{
		i = 195 - buf[l] + 97;//根据ASCII码得出度数的相应数值
		if(buf[++l] == '1')
			TIM_SetCompare1(TIM2, i); //A0	厚
		if(buf[l] == '2')
			TIM_SetCompare2(TIM2, i); //A1	底
		if(buf[l] == '3')
			TIM_SetCompare1(TIM3, i); //A6	薄
		if(buf[l] == '4')
			TIM_SetCompare2(TIM3, i); //A7	电
		if(buf[l++] == '5')
			TIM_SetCompare3(TIM3, i); //B0	脑
		if(buf[l - 1] == buf[l + 1])
			delay_ms(400);
		else
			delay_ms(40);
	}
	return;
}



