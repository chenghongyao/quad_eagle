#include "esp8266.h"
#include "sys.h"
#include "stdarg.h"
#include "string.h"
#include "usart.h"



extern queue_t qUart;
#define esp8266_putc(_c) H_USART_PUT8_F(3,_c)

static char espbuf[50];
const char resOK[] = "OK";
const char  resERROR[] = "ERROR";
const char  resFAIL[] = "Fail";
esp_info_t esp_info;

char *itoa(uint16_t num, char buf[])
{
	char t;
	char *p = buf;
	char *b = buf;
	do
	{
		t = num % 10;
		num = num / 10;
		*p++ = t + '0';
	} while (num != 0);
	*p = '\0';
	p--;

	do
	{
		
		t = *p;
		*p-- = *b;
		*b++ = t;
	} while (p > b);
	return buf;
}

void esp8266_puts(char *s)
{
	while(*s!='\0')
	{
		esp8266_putc(*s);
		s++;
	}
}

void esp8266_at(const char *fmt,...)
{
	va_list ap;	
	char datbuf[10];
	esp8266_putc('A');	esp8266_putc('T');	esp8266_putc('+');
	va_start(ap,fmt);

	for(;*fmt;fmt++)
	{
		if(*fmt != '%')
		{
			esp8266_putc(*fmt);
		}
		else	
		{
			switch(*++fmt)
			{
				case 'd':
					esp8266_puts(itoa(va_arg(ap,int),datbuf));
					break;
				case 's':
					esp8266_putc('\"');
					esp8266_puts(va_arg(ap,char*));
					esp8266_putc('\"');
					break;
				case '%':
					esp8266_putc('%');
					break;
				default :
					break;
			}
		}
	}
	va_end(ap);
	esp8266_putc('\r');
	esp8266_putc('\n');
}


uint8_t esp8266_check(uint16_t mswait,uint16_t timeout)
{
	uint8_t fres = 0;
	uint32_t tNow;
	delay_ms(mswait);
	tNow = millis();
	while ((millis() - tNow)<timeout)
	{
		while (queue_gets(&qUart, (uint8_t*)espbuf, sizeof(espbuf)))
		{
			//printf("res:%s\r\n", espbuf);
			if (fres)
			{
				if (!strncmp(resOK, espbuf, 2))
				{
					//puts("CHECK:OK");
					return ESP8266_RES_OK;
				}
				else if (!strncmp(resERROR, espbuf, 5))
				{
					//puts("CHECK:ERROR");
					return ESP8266_RES_ERROR;
				}
				else if (!strncmp("busy p...", espbuf, 5))
				{
					fres = 0;
				}
				else 
				{
					//printf("res:%s\r\n", espbuf);
					//puts("CHECK:FAIL");
					return ESP8266_RES_FAIL;
				}
			}
			else
			{
				if (*espbuf == '\0')
				{
					//puts("res start");
					fres = 1;
				}
				else if(!strncmp("No AP", espbuf, 5))
				{
					esp_info.getip = 0;
				}
			}
		}
	}
	return ESP8266_RES_TIMEOUT;
}
///////////////////////////////////////////////////////////////////////////////////////////////

uint8_t esp8266_reset()
{
	esp8266_at("RST");
	return esp8266_check(80,10);
}

uint8_t esp8266_ate(uint8_t enable)
{
	if(enable)
		esp8266_puts("ATE1\r\n");
	else 
		esp8266_puts("ATE0\r\n");

	return esp8266_check(80,10);
}
/******************************************************************************* 
���ܣ����ô�������
������
stopbits:1/1bit,2/1.5bit,3/2bit
parity:0/none,1/odd,2/even
flow control:0/disable ,2/RTS,3/CTS,4/RTS&CTS
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/  
uint8_t esp8266_setUART(uint32_t baudrate,uint8_t databits,uint8_t stopbits,uint8_t parity,uint8_t flowcontrol)
{
	esp8266_at("UART_CUR=%d,%d,%d,%d,%d",baudrate,databits,stopbits,parity,flowcontrol);
	return esp8266_check(80,10);
}

///////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************* 
���ܣ����ù���ģʽ
������
mode:wifi����ģʽ
	WF_MODE_STA
	WF_MODE_AP
	WF_MODE_STAAP
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/  
uint8_t wifi_setMode(uint8_t mode)
{
	esp8266_at("CWMODE_CUR=%d", mode);
	return esp8266_check(80,10);
}

/******************************************************************************* 
���ܣ������ȵ�
�������� 
ʱ�䣺1/17/2017
ע�⣺ STAģʽ��ʹ��
*******************************************************************************/  
uint8_t wifi_joinAP(char *ssid,char *pwd)
{
	esp8266_at("CWJAP_CUR=%s,%s",ssid,pwd);
	return esp8266_check(5000,10000);
}

uint8_t wifi_askAP()
{
	esp8266_at("CWJAP?");
	return esp8266_check(100, 10);
}
uint8_t wifi_askIP()
{
	esp8266_at("CIFSR");
	return esp8266_check(100, 10);
}
/******************************************************************************* 
���ܣ��˳��ȵ�
�������� 
ʱ�䣺1/17/2017
ע�⣺ STAģʽ��ʹ��
*******************************************************************************/  
uint8_t wifi_quitAP()
{
	
	esp8266_at("CWQAP");
	return esp8266_check(80,10);
}

/******************************************************************************* 
���ܣ�����softAP����
������
chl:ͨ����
ecn:���ܷ�ʽ
WF_ECN_OPEN
WF_ECN_WPA_PSK
WF_ECN_WPA2_PSK
WF_ECN_WPA_WPA2_PSK
ʱ�䣺1/17/2017
ע�⣺ ��APģʽ��ʹ��
*******************************************************************************/  
uint8_t wifi_setAP(char *ssid,char *pwd,uint8_t chl,uint8_t ecn)
{
	esp8266_at("CWSAP_CUR=%s,%s,%d,%d",ssid,pwd,chl,ecn);
	return esp8266_check(80,10);
}

/*******************************************************************************
���ܣ�ʹ��DHCP
������
enable:
mode:���õ�ģʽ
	DHCP_STA
	DHCP_AP
	DHCP_STA_AP
ʱ�䣺1/17/2017
ע�⣺
*******************************************************************************/
uint8_t wifi_setDHCP(uint8_t mode ,uint8_t enable)
{
	esp8266_at("CWDHCP_CUR=%d,%d", mode,enable);
	return esp8266_check(80,10);
}


/******************************************************************************* 
���ܣ�ʹ���ϵ��Զ������ȵ�
�������� 
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/  
uint8_t wifi_autoConnect(uint8_t enable)
{
	esp8266_at("CWAUTOCONN=%d", enable);
	return esp8266_check(80,10);
}

//////////////////////////////////////////////////////////////////////////


/******************************************************************************* 
���ܣ����ö�ͨ��
������
ʱ�䣺1/17/2017
ע�⣺ 
tcpServerֻ֧�ֶ�����,��Ҫ�رշ�����������
*******************************************************************************/  
uint8_t ip_setMUX(uint8_t enable)
{
	if(enable)
		esp8266_at("CIPMUX=1\r\n");
	else
		esp8266_at("CIPMUX=0\r\n");
	return esp8266_check(80,10);
}

/******************************************************************************* 
���ܣ�����͸��
������
ʱ�䣺1/17/2017
ע�⣺ ������ģʽ�²�������͸��
*******************************************************************************/  
uint8_t ip_setMode(uint8_t enable)
{
	if(enable)
		esp8266_at("CIPMODE=1");
	else 
		esp8266_at("CIPMODE=0");
	
	return esp8266_check(80,10);
}


/******************************************************************************* 
���ܣ�����ΪTCP Client
������
ip_remote:Զ��ip��ַ
remote_port:Զ�̶˿�
ʱ�䣺1/17/2017
ע�⣺ ����Ϊ������ģʽ
*******************************************************************************/  
uint8_t ip_tcpClient(char *remote_ip, uint16_t remote_port)
{
	esp8266_at("CIPSTART=\"TCP\",%s,%d", remote_ip, remote_port);
	return esp8266_check(80,1000);
}

/******************************************************************************* 
���ܣ��ر�ip����
�������� 
ʱ�䣺1/17/2017
ע�⣺����TcpClient,TcpServier,UDP,������ģʽ
*******************************************************************************/  
uint8_t ip_close()
{
	esp8266_at("CIPCLOSE");
	return esp8266_check(80,10);
}


/******************************************************************************* 
���ܣ�����ΪTCP Server
������
port:���Ӷ˿ں�
enable:�Ƿ��
ʱ�䣺1/17/2017
ע�⣺������ģʽ

*******************************************************************************/  
uint8_t ip_tcpServer(uint16_t port, uint8_t enable)
{
	if (enable)
	{
		esp8266_at("CIPSERVER=1,%d", port);
		return esp8266_check(80,10);
	}
	else
	{
		esp8266_at("CIPSERVER=0");
	}
	return 0;

}




/******************************************************************************* 
���ܣ���͸��ģʽ����
������
dat:
len:���ݳ���
ʱ�䣺1/17/2017
ע�⣺ ������ģʽ
*******************************************************************************/  
uint8_t tcp_send(uint8_t *dat,uint16_t len)
{
	if(len>2048)len=2048;
	esp8266_at("CIPSEND=%d",len);
	if(esp8266_check(3000,10))return 1;

//	esp8266_write(dat,len);
	
	
	return 0;
}


/******************************************************************************* 
���ܣ�͸��ģʽ����������
�������� 
ʱ�䣺1/17/2017
ע�⣺ 
*******************************************************************************/  
uint8_t tcp_startSend()
{
	uint8_t c;
	esp8266_at("CIPSEND");
	if(!esp8266_check(100,10))
	{
	//while(*esp8266_readLine()!='\0');
		//esp8266_read(&c,1);
		if(c=='>')return 0;
	}
	
	return 1;
}

/******************************************************************************* 
���ܣ�͸��ģʽ��ֹͣ����
�������� 
ʱ�䣺1/17/2017
ע�⣺ ��һ�����ݱ�����1s�Ժ�
*******************************************************************************/  
uint8_t tcp_stopSend()
{
	esp8266_puts("+++");
	return 0;
}

uint8_t tcp_setOverTime(uint16_t ns)
{
	if (ns > 7200)ns = 7200;
	esp8266_at("CIPSTO=%d", ns);
	return esp8266_check(80,10);
}


uint8_t wifi_setAPMAC(char *mac)
{
	esp8266_at("CIPAPMAC_CUR=%s",mac);
	return esp8266_check(80,10);
}
uint8_t wifi_setSTAMAC(char *mac)
{
	esp8266_at("CIPSTAMAC_CUR=%s",mac);
	return esp8266_check(80,10);
}
uint8_t wifi_setAPIP(char *ip)
{
	esp8266_at("CIPAP=%s",ip);
	return esp8266_check(80,10);
}
uint8_t wifi_setSTAIP(char *ip)
{
	esp8266_at("CIPSTA=%s",ip);
	return esp8266_check(80,10);
}


void esp8266_init()
{
	esp_info.getip = 0;
	esp_info.getserver = 0;
	
 	if (esp8266_ate(0) == ESP8266_RES_OK)
 	{
 		puts("ate0 ok");
 	}
 	else
 	{
 		puts("ate0 fail");
 	}
	/*
// 	if (wifi_autoConnect(1) == ESP8266_RES_OK)
// 	{
// 		puts("autoconn ok");
// 	}
// 	else
// 	{
// 		puts("autoconn fail");
// 	}

//	if (wifi_askAP() == ESP8266_RES_OK)
//	{
//		puts("askAP ok");
//	}
//	else
//	{
//		puts("askAP fail");
//	}
	
	if (wifi_askIP() == ESP8266_RES_OK)
	{
		puts("askip ok");
	}
	else
	{
		puts("askip fail");
	}

	if(esp_info.getip == 0)
	{
		if (wifi_setMode(WF_MODE_STA) == ESP8266_RES_OK)
		{
			puts("wifimode ok");
		}
		else
		{
			puts("wifimode fail");
		}
		
	 	if (wifi_joinAP("Phicomm_EE80F0", "interset0754") == ESP8266_RES_OK)
		{
			puts("joinAP ok");
		}
		else
		{
			puts("joinAP fail");
		}
		
	}
	
	
	if (ip_close() == ESP8266_RES_OK)		//��ֹ�Ѿ�����
	{
		puts("ipclose ok");
	}
	else
	{
		puts("ipclose fail");
	}
	
	
	if (ip_setMUX(0) == ESP8266_RES_OK)
	{
		puts("MUX ok");
	}
	else
	{
		puts("MUX fail");
	}

	if (ip_setMode(1) == ESP8266_RES_OK)
	{
		puts("͸�� ok");
	}
	else
	{
		puts("͸�� fail");
	}



	if (ip_tcpClient("192.168.1.113",777) == ESP8266_RES_OK)
	{
		puts("TCPClient ok");
		esp_info.getserver = 1;
	}
	else
	{
		puts("TCPClient fail");
	}*/
}

