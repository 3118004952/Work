#include "sta_tcpclent_test.h"
#include "SysTick.h"
#include "usart.h"
#include "esp8266_drive.h"


volatile u8 TcpClosedFlag = 0;

void send_data(char *data, u8 length, ENUM_ID_NO_TypeDef id)
{
	char cmd_wifi[50];
	sprintf(cmd_wifi, "AT+CIPSEND=%d,%d",id, length);
	ESP8266_Send_AT_Cmd ( cmd_wifi, "<", 0, 500 );
	ESP8266_USART ( "%s", data );
}

void ESP8266_STA_TCPClient_Test()
{
	
	ESP8266_CH_PD_Pin_SetH;
	ESP8266_AT_Test();
	ESP8266_Net_Mode_Choose(STA);
	ESP8266_Send_AT_Cmd ( "AT+RST", "ready", 0, 500 );
	
	ESP8266_Send_AT_Cmd ("AT+CIPMODE=0", "OK", 0, 500);
	ESP8266_Send_AT_Cmd ( "AT+CIPMUX=1", "OK", 0, 500 );
	ESP8266_Send_AT_Cmd ( "AT+CIPSERVER=0", "OK", 0, 500 );
	while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
	while(ESP8266_Send_AT_Cmd ( User_ESP8266_TCPclient, "OK", 0, 500));
}

//void ESP8266_STA_TCPClient_Test(void)
//{
//	u8 res;
//	
//	char str[100]={0};
//	
//	printf ( "\r\n��������ESP8266�����ĵȴ�...\r\n" );

//	ESP8266_CH_PD_Pin_SetH;

//	ESP8266_AT_Test();
//	ESP8266_Net_Mode_Choose(STA);
//	while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
//	ESP8266_Enable_MultipleId ( DISABLE );
//	ESP8266_Send_AT_Cmd ( "AT+CIPSERVER=0", "OK", 0, 500 );
//	while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));
//	
//	while(!ESP8266_UnvarnishSend());
//	printf ( "\r\n����ESP8266 OK��\r\n" );
//	
//	while ( 1 )
//	{		
//		sprintf (str,"www.prechin.cn\r\n" );
//		ESP8266_SendString ( ENABLE, str, 0, Single_ID_0 );
//		delay_ms(1000);
//		if(TcpClosedFlag) //����Ƿ�ʧȥ����
//		{
//			ESP8266_ExitUnvarnishSend(); //�˳�͸��ģʽ
//			do
//			{
//				res = ESP8266_Get_LinkStatus();     //��ȡ����״̬
//			} 	
//			while(!res);
//			
//			if(res == 4)                     //ȷ��ʧȥ���Ӻ�����
//			{
//				printf ( "\r\n���Եȣ����������ȵ�ͷ�����...\r\n" );
//				
//				while (!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD ) );
//				
//				while (!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0 ) );
//				
//				printf ( "\r\n�����ȵ�ͷ������ɹ�\r\n" );
//			} 
//			while(!ESP8266_UnvarnishSend());					
//		}
//	}
//		
//}

