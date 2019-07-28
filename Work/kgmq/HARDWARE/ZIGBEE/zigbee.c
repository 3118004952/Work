#include "zigbee.h"
#include "usart.h"
#include "string.h"

u8 decode_flag = 0;
u8 code[100] = { 0 };
u8 send_data[100] = { 0 };
u8 code_len = 0;
u8 code_num = 0;
u8 online1 = 0;
u8 online2 = 0;


/*******************************************************************************
* �� �� ��         : void receive_data(u8 rec)
* ��������		   : ����zigbee���ݣ�ȥ���ݰ��ײ���β����
* ��    ��         : u8 rec�����ڽ��յ��ֽ�
* ��    ��         : ��
*******************************************************************************/ 
void receive_data(u8 rec)
{
	if((rec == 0xfe || rec == 0x3f) && decode_flag == 0)//����ͷ
	{
		decode_flag = 1;
		return;
	}
	if(decode_flag == 1)//�������ݳ���
	{
		code_len = rec;
		decode_flag = 2;
		return;
	}
	if(decode_flag == 2)//��ʼ����
	{
		if(rec == 0xff || rec == 0x3f)
		{
			if(code_len != (code_num - 4) * 2)//���ݳ����쳣
			{
				code_num = 0;
				code_len = 0;
				decode_flag = 0;
				return;
			}
			else
			{
				code[code_num] = '\0';
				code_num = 0;
				decode_flag = 3;
				decode_send();
				decode_flag = 0;//�������
				return;
			}
		}
		else
		{
			if(code_num >= 100)//���������������������쳣�����˳�
			{
				code_num = 0;
				decode_flag = 0;
				return;
			}
			
			code[code_num] = rec;
			code_num++;
		}
	}
}


/*******************************************************************************
* �� �� ��         : void decode(void)
* ��������		   : ����zigbee����
* ��    ��         : ��
* ��    ��         : ��
* ˵    ��         : ���ݸ�ʽΪ��1�˿ںţ�2�˿ںţ�34Դ��ַ��������Ϊ����
*******************************************************************************/ 
void decode_send(void)
{
	if(decode_flag == 3)
	{
		if(strstr((char *)code, "AARE") != NULL)
		{
			
		}
		else if(strstr((char *)code, "BBLO") != NULL)
		{
			
		}
	}
}

/*******************************************************************************
* �� �� ��         : void locate_self(void)
* ��������		   : ����zigbee����
* ��    ��         : ��
* ��    ��         : ��
* ˵    ��         : ���ݸ�ʽΪ��1�˿ںţ�2�˿ںţ�34Դ��ַ��������Ϊ����
*******************************************************************************/ 
void locate_self(void)
{
	
}

/*******************************************************************************
* �� �� ��         : zigbee_send(u8 addr1, u8 addr2, u8 *data)
* ��������		   : ����zigbee����
* ��    ��         : ��
* ��    ��         : ��
* ˵    ��         : ���ݸ�ʽΪ��1�˿ںţ�2�˿ںţ�34Դ��ַ��������Ϊ����
*******************************************************************************/ 
void zigbee_send(u8 addr1, u8 addr2, u8 *data)
{
	sprintf((char *)send_data, "%c%c%c%c%c%c%s%c", 254, strlen((char *)data) + 4, 144, 145, addr1, addr2, data, 255);
	USART3_Send(send_data, strlen((char *)send_data));
}

void Send_Server(u8 *data)
{
	zigbee_send(0x3a, 0x06, data);
}
