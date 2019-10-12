#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "system.h"
#include "esp8266_drive.h"


#define User_ESP8266_SSID	  "Jense_Wifi_5G"	      //Ҫ���ӵ��ȵ������
#define User_ESP8266_PWD	  "qgstudio"	  //Ҫ���ӵ��ȵ������

#define User_ESP8266_TCPServer_IP	  "172.20.10.3"	  //Ҫ���ӵķ�������IP
#define User_ESP8266_TCPServer_PORT	  "8080"	  //Ҫ���ӵķ������Ķ˿�

#define User_ESP8266_TCPclient "AT+CIPSTART=0,\"TCP\",\"47.103.29.31\",60000"

extern volatile uint8_t TcpClosedFlag;  //����һ��ȫ�ֱ���


void ESP8266_STA_TCPClient_Test(void);
void send_data(char *data, u8 length, ENUM_ID_NO_TypeDef id);


#endif
