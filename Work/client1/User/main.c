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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	USART3_Init(115200);
	USART1_Init(115200);
	TIM2_CHx_PWM_Init(199,7199);//(7200*200)/72000000=0.02=20ms
	TIM3_CHx_PWM_Init(199,7199);
	pwm_Init();
	Wifi_queue = (LQueue*)malloc(sizeof(LQueue));
	InitLQueue(Wifi_queue);
	ESP8266_Init(115200);
	ESP8266_STA_TCPClient_Test();
	send_data(ID, strlen(ID), 0);//���͸ö�����
	send_data(ID, strlen(ID), 1);//���͸ö�����
	send_data(ID, strlen(ID), 2);//���͸ö�����
	send_data(ID, strlen(ID), 3);//���͸ö�����
	send_data(ID, strlen(ID), 4);//���͸ö�����
	while(1)
	{
		//	zigbee_send(0x1a,0x06,"000");//����zigbee����
		if(choice == 1 || choice ==2)
		{
			judge_time = 0;//ʹ�����ļ������¿�ʼ
			if(wifi_time++ == 200000)//���ճ�ʱ�ˣ���ʼ��WIFI���յ�����
			{
				choice = 0;
				free(Wifi_buf);
				Wifi_buf_length = 0;
				wifi_time = 0;
			}
		}
		else
			wifi_time = 0;//ʹ�жϽ��ճ�ʱ�ļ������¿�ʼ
	  	if(Wifi_queue->length != 0)//�ж�WIFI�����Ƿ�ʹ����
		{
			u8 *data = Wifi_queue->front->data;
			int length = Wifi_queue->front->length;
			if(data[1] != 'I' && data[2] != 'P' && data[3] != 'D')//�ж��Ƿ������ȷ
			{
				
			}
			else//������ȷ
			{
				if(length != 0)
				{

					data[length] = '\0';
					Judge_Wifi(data, length);//��ʼ�жϲ�ִ����Ӧ����

				}
				data[0] = '\0';//���һ�β�������ʼ��������ֵ
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

void Judge_computer(u8 *s)//���Ա��ƽ̨������
{
	char *choice;

	char null[] = "";
	char str_A[] = "k4q4";//��ǰ�ߣ���ͷ��ǰ��
	char str_B[] = "k5e5";//����ߣ�ĸͷ��ǰ��
	char str_L[] = "e5q4s2g5k2e5";//�����ƶ�һ�Σ������һ�ˣ�
	char str_R[] = "e5q4a2g5k2e5";//�����ƶ�һ�Σ�������һ�ˣ�
	
	char str_C_A[] = "a2";//����    ����
	char str_C_L[] = "a1";//  		��ߣ������һ�ˣ�
	char str_C_R[] = "a3";//		�ұߣ�������һ�ˣ�
	
	char str_E_A[] = "s2k4q4k2";//���벢�ƶ�һ�� 	����
	char str_E_L[] = "s1k4q4k1";//  				��ߣ������һ�ˣ�
	char str_E_R[] = "s3k4q4k3";//					�ұߣ�������һ�ˣ�
	
	char str_1[] = "r5";//ֱ�ӿ��ƽǶ�
	char str_2[] = "d4";
	
	int save[4];//���ڱ���ð�ŵ�λ���������һλ
	
	int i = 0;//����ѭ��
	int j = 0;
	int k = 1;
	int l = 0;
	
	choice = null;//�ÿ�
	
	while(i != 3)//һ��ֻ��3��ð��
	{
		if(s[j++] == ':')
		{
			save[i++] = j;//��¼ð��λ�õ���һλ
		}
		if(j > 50)//��ֹ�ַ�������ð��
			return;
	}
	
	switch(s[save[1]])
	{
		case 'a'://��ǰ������˶�
		case 's'://��б
			k = s[save[2]] - 48;
			j = save[2];
			while(s[++j] >= '0' && s[j] <= '9')
				k = k * 10 + s[j] - 48;//�����������ֵ
			switch(s[save[0]])//�ж��������˶�
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
		case 'c'://����
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
		case 'd'://�Ͽ�����
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
		case 't'://ֱ�ӿ��ƽǶ�14
			j = s[save[2]] - 48;
			if(s[save[2] + 1] >= '0' && s[save[2] + 1] <= '9')//����Ƕȵ�ֵ
			{
				j = j * 10 + s[save[2] + 1] - 48;
				if(s[save[2] + 2] >= '0' && s[save[2] + 2] <= '9')
				{
					j = j * 100 + s[save[2] + 2] - 48;
					if(j > 180)
						break;
				}
			}
			j = (j + 7) / 14;//���ڶ�����ȵ����⣬ֻ�ֳܷ�13�ݽǶ�
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
	while(k > 0 && l < 20 && k-- != 0)//���ڶ������
	{
		l = 0;
		while(choice[l] != '\0')//ֱ�ӿ��ƶ��
		{
			i = 195 - choice[l] + 97;//����ASCII��ó���������Ӧ��ֵ
			if(choice[++l] == '1')
				TIM_SetCompare1(TIM2, i); //A0	�����һ�˵Ķ��
			if(choice[l] == '2')
				TIM_SetCompare2(TIM2, i); //A1	�ײ��Ķ��
			if(choice[l] == '3')
				TIM_SetCompare1(TIM3, i); //A6	������һ�˵Ķ��
			if(choice[l] == '4')
				TIM_SetCompare2(TIM3, i); //A7	ĸͷ���
			if(choice[l++] == '5')
				TIM_SetCompare3(TIM3, i); //B0	��ͷ���
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
		if(buf[j + 3] == '_')//��ƽ̨����  AT_
		{
			send_data(ID, strlen(ID), buf[5] - 48);//���͸ö�����
		}
		else//����wifiģ�������ָ��
		{
			buf[length - 2] = '\0';
			u8 *a = buf + j + 1;
			ESP8266_Send_AT_Cmd((char*)a, "OK", 0, 500);
		}
		return;
	}
	int l = j + 1;
	if(buf[l] == '#')//������ #
	{	
		while(buf[l] != '\0')//�ֽ�ָ��
		{
			if(buf[l++] == '#')//�ж��Ƿ����Ա��ƽ̨��������ģ���˶�ָ��
			{
				Judge_computer(buf + l);
			
			}
		}
		return;
	}
	l = j + 1;
	int i;
	while(buf[l] != '\0' && l < 100)//ֱ�ӿ��ƶ��
	{
		i = 195 - buf[l] + 97;//����ASCII��ó���������Ӧ��ֵ
		if(buf[++l] == '1')
			TIM_SetCompare1(TIM2, i); //A0	��
		if(buf[l] == '2')
			TIM_SetCompare2(TIM2, i); //A1	��
		if(buf[l] == '3')
			TIM_SetCompare1(TIM3, i); //A6	��
		if(buf[l] == '4')
			TIM_SetCompare2(TIM3, i); //A7	��
		if(buf[l++] == '5')
			TIM_SetCompare3(TIM3, i); //B0	��
		if(buf[l - 1] == buf[l + 1])
			delay_ms(400);
		else
			delay_ms(40);
	}
	return;
}



