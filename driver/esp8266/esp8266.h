#ifndef _ESP8266_H
#define _ESP8266_H

#include "sys.h"


/*
1.��ʼ����wifi֮ǰҪ���ù���ģʽ(setMode)ΪSTA��AP
(1).STAģʽ��joinAP����WIFI,��quitAP�˳�WIFI
(2).APģʽ��wifi_setAP����ap����
2.ģ���TCP��UDPͨ��,ͨ��CIPSTART�����type����ʵ��,��������˷�װ,����ֻ����TCPģʽ
(1).TCP����ΪClient��Server
(2).Client
	����startClient,���������IP,�Ͷ˿ں�
	closeClient�رո�����
	��������
		ֻ�е�����ģʽ�¿���͸��(setMUX(0)),����setMode(1)������Ϊ͸��ģʽ


			
(3).Server
		�����ȵ���setMUX(1)����Ϊ��·ģʽ,��·ģʽ����͸��(setMode(0))
		tcp_setServer(1,port),����˿ں�,����Server����,����TCPClient����ʱ��˳��ռ��һ������,���5��,id=0-5
		��closeClient()����id�Źرն�ӦClient,
		ͨ��CIFSR��ȡ����IP
		��������:
			ʹ��send����,����id�ź����ݴ�С			
			��������:���Client��������,��ʽΪ+IPD,id,����:data


		setMode(0);
		setMUX(1);
		setServer(1,port)
		
		
	
(1).TCP ServerҪ�ö����ӷ�ʽ(setMUX(1)),��Ϊ��Ҫ���Ӷ��Client,��ʱ����Ϊ͸��ģʽ
(2).TCP ClientҪ�õ�����(setMUX(0)),ֻ��Server��������
*/


//setMode��,wifi����ģʽ
#define WF_MODE_STA			1
#define WF_MODE_AP			2
#define WF_MODE_STA_AP		3
//setAP��,���ܷ�ʽ
#define WF_ECN_OPEN			0
#define WF_ECN_WPA_PSK		2
#define WF_ECN_WPA2_PSK 	3
#define WF_ECN_WPA_WPA2_PSK	4
//DHCP����ģʽ				
#define DHCP_AP				0
#define DHCP_STA			1
#define DHCP_STA_AP			2
//Ӧ������
#define ESP8266_RES_OK			0
#define ESP8266_RES_ERROR		1
#define ESP8266_RES_FAIL		2
#define ESP8266_RES_TIMEOUT		3



typedef struct
{
	uint8_t getip;
	uint8_t getserver;
}esp_info_t;

extern esp_info_t esp_info;

void esp8266_init(void);
//��������
uint8_t esp8266_reset(void);
uint8_t esp8266_ate(uint8_t enable);		//���û���
uint8_t esp8266_setUART(uint32_t baudrate,uint8_t databits,uint8_t stopbits,uint8_t parity,uint8_t flowcontrol);


//wifi��������ָ��
uint8_t wifi_setMode(uint8_t mode);//����STA��APģʽ
uint8_t wifi_joinAP(char *ssid,char *pwd);
uint8_t wifi_quitAP(void);
uint8_t wifi_setAP(char *ssid,char *pwd,uint8_t chl,uint8_t ecn);
uint8_t wifi_setDHCP(uint8_t mode, uint8_t enable);
uint8_t wifi_autoConnect(uint8_t enable);


uint8_t wifi_askAP(void);
uint8_t wifi_askIP(void);

//TCP/IP����
uint8_t ip_setMUX(uint8_t enable);				
uint8_t ip_setMode(uint8_t enable);						
uint8_t ip_tcpClient(char *addr, uint16_t port);
uint8_t ip_close(void);							


uint8_t ip_tcpServer(uint16_t port, uint8_t enable);//����tcp server
uint8_t tcp_closeServer(void);
uint8_t tcp_setOverTime(uint16_t ns);					//����tcp server�ȴ���ʱʱ�� 0-7200

uint8_t tcp_startSend(void);
uint8_t tcp_stopSend(void);

#endif


//	esp8266_reset();
//	esp8266_ate(1);
//	esp8266_setUART(500000,8,1,0,0);
//	
	//	wifi_setMode(WF_MODE_STA);
//	wifi_joinAP("cheng","a1234567");
//	wifi_quitAP();
//	wifi_setAP("cheng","a1234567",2,WF_ECN_OPEN);
//	wifi_setDHCP(DHCP_STA,1);
//	wifi_autoConnect(1);
//	ip_setMUX(1);
//	ip_setMode(0);
//	ip_tcpClient("198.168.1.1",1010);
//	ip_close();
//	ip_tcpServer(5000,1);
//	tcp_startSend();
//	tcp_stopSend();

