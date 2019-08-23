#ifndef _sta_tcpclent_test_H
#define _sta_tcpclent_test_H


#include "system.h"
#include "esp8266_drive.h"


#define User_ESP8266_SSID	  "ChenYu"	      //要连接的热点的名称
#define User_ESP8266_PWD	  "123456789"	  //要连接的热点的密码

#define User_ESP8266_TCPServer_IP	  "172.20.10.3"	  //要连接的服务器的IP
#define User_ESP8266_TCPServer_PORT	  "8080"	  //要连接的服务器的端口


extern volatile uint8_t TcpClosedFlag;  //定义一个全局变量


void ESP8266_STA_TCPClient_Test(void);
void send_data(char *data, u8 length, ENUM_ID_NO_TypeDef id);


#endif
