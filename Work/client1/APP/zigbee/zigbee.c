#include "zigbee.h"
#include "usart.h"
#include "string.h"


u8 decode_flag = 0;
u8 code[100] = { 0 };
u8 code_len = 0;
u8 code_num = 0;

/*******************************************************************************
* 函 数 名         : void receive_data(u8 rec)
* 函数功能		   : 接收zigbee数据（去数据包首部和尾部）
* 输    入         : u8 rec：串口接收的字节
* 输    出         : 无
*******************************************************************************/ 
void receive_data(u8 rec)
{
	if((rec == 0xfe || rec == 0x3f) && decode_flag == 0)//接收头
	{
		decode_flag = 1;
		return;
	}
	if(decode_flag == 1)//接收数据长度
	{
		code_len = rec;
		decode_flag = 2;
		return;
	}
	if(decode_flag == 2)//开始接收
	{
		if(rec == 0xff || rec == 0x3f)
		{
			if(code_len != code_num)//数据长度异常
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
				decode_flag = 0;
				printf("%s", code);
				return;
			}
		}
		else
		{
			if(code_num >= 100)//如果出现数据量过大或者异常，则退出
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
* 函 数 名         : zigbee_send(u8 addr1, u8 addr2, u8 *data)
* 函数功能		   : 发送zigbee数据
* 输    入         : 无
* 输    出         : 无
* 说    明         : 数据格式为：1端口号，2端口号，34源地址，其他的为数据
*******************************************************************************/ 
void zigbee_send(u8 addr1, u8 addr2, u8 *data)
{
	u8 send_data[100] = { 0 };
	sprintf((char *)send_data, "%c%c%c%c%c%c%s%c", 254, strlen((char *)data) + 4, 144, 145, addr1, addr2, data, 255);
	USART3_Send(send_data, strlen((char *)send_data));
}

