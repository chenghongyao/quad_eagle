#include "nrf_dev.h"
#include "nrf.h"



//#include "delay.h"
//#include "spi.h"
//#include "led.h"
//#include "usart.h"





static u8 f_tx_wait = 0;
static u8 rf_setup;
static u8 packet_size=32;
static u8 rfch = 48;
static u8 bitrate = NRF_BR_2M;
static u8 rfpower = NRF_POWER_0dB;
static u8 txrx_address[5] = { 'w', 'e', 'i', 'c', 'h' };  //���͵�ַ


#ifdef NRF_ACKPAY_ENABLE
u8 nrf_ackbuf[32];
u8 nrf_acklen=0;
#endif






void nrf24l01_init()
{
	nrf24l01_setTxRxMode(TXRX_OFF);
	nrf24l01_setTxAddr(txrx_address,5);							//���͵�ַ	
	nrf24l01_setRxAddr(RX_ADDR_P0,txrx_address,5);	//ͨ��0���յ�ַ
	nrf24l01_enaleRxAddr(NRF_P0);									//ʹ��ͨ��0	
#ifdef NRF_ACKPAY_ENABLE
	nrf24l01_enableAck(NRF_P0,1);								
	nrf24l01_setRetry(10,1);
	//ʹ�ܶ�̬���ݿ��,Ӧ���
	nrf24l01_setFeature(NRF_EN_DPL|NRF_EN_ACK_PAY);
	nrf24l01_enableDyn(NRF_P0);	
#else
	nrf24l01_enableAck(NRF_P0,0);
	nrf24l01_setRxPW(RX_PW_P0,packet_size);			//���ݿ��,�����뷢��һ��
#endif
	
	nrf24l01_setBitrate(bitrate);									
	nrf24l01_setPower(rfpower);									
	nrf_setRFCH(rfch);											
	nrf24l01_FlushTx();	nrf24l01_FlushRx();
	nrf24l01_clearStatus();
}



//========================================================
//���³�������Ķ�
u8 nrf24l01_writeReg(u8 reg,u8 value)
{
	u8 status;
	NRF_CSN=0; //ʹ�� SPI ����
	status =NRF_ReadWriteByte(NRF_WRITE_REG|reg); //���ͼĴ�����
	NRF_ReadWriteByte(value); //д��Ĵ�����ֵ
	NRF_CSN=1; //��ֹ SPI ����
	return(status); //����״ֵ̬
}

u8 nrf24l01_writeRegMulti(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr; 
	NRF_CSN = 0; //ʹ�� SPI ����
	status = NRF_ReadWriteByte(NRF_WRITE_REG|reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
		NRF_ReadWriteByte(*pBuf++); //д������
	NRF_CSN = 1; //�ر� SPI ����
	return status; //���ض�����״ֵ̬
} 


//��ȡ SPI �Ĵ���ֵ
//reg:Ҫ���ļĴ���
u8 nrf24l01_readReg(u8 reg)
{
	u8 reg_val; 
	NRF_CSN = 0; //ʹ�� SPI ���� 
	NRF_ReadWriteByte(NRF_READ_REG|reg); //���ͼĴ�����
	reg_val=NRF_ReadWriteByte(0XFF); //��ȡ�Ĵ�������
	NRF_CSN = 1; //��ֹ SPI ���� 
	return(reg_val); //����״ֵ̬
} 

u8 nrf24l01_readRegMulti(u8 reg, u8 *pBuf, u8 len)
{
	u8 status,u8_ctr; 
	NRF_CSN = 0; //ʹ�� SPI ����
	status = NRF_ReadWriteByte(NRF_READ_REG|reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
		*pBuf++=NRF_ReadWriteByte(0xFF); //д������
	NRF_CSN = 1; //�ر� SPI ����
	return status; //���ض�����״ֵ̬
} 





//���ڲ��������Ƿ����,���ɹ�����0
u8 nrf24l01_check(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i;
	nrf24l01_writeRegMulti(TX_ADDR,buf,5);//д�� 5 ���ֽڵĵ�ַ.
	nrf24l01_readRegMulti(TX_ADDR,buf,5); //����д��ĵ�ַ 
	for(i=0;i<5;i++)
		if(buf[i]!=0XA5)return 1;		//���жԱ�
	return 0;
} 

static u8 Strobe(u8 state)		//д����
{
	u8 res;
	NRF_CSN = 0; //ʹ�� SPI ���� 
	res = NRF_ReadWriteByte(state);
	NRF_CSN = 1;	
	return res;
}
u8 nrf24l01_FlushTx()
{
	return Strobe(FLUSH_TX);
}
u8 nrf24l01_FlushRx()
{
	return Strobe(FLUSH_RX);
}
u8 nrf24l01_getStatus()
{
	return Strobe(NRF_NOP);
}
void nrf24l01_clearStatus()
{
	nrf24l01_writeReg(STATUS,0x70); //��� TX_DS ��MAX_RT �жϱ�־

}
u8 nrf24l01_getDynamicPayloadSize()
{
	u8 res;
	NRF_CSN = 0; //ʹ�� SPI ���� 
	NRF_ReadWriteByte(R_RX_PL_WID);
	res = NRF_ReadWriteByte(NRF_NOP);
	NRF_CSN = 1;
	return res;
}




//0 1Mbps
//1 2Mbps
//2 250kbps
u8 nrf24l01_setBitrate(u8 bitrate)
{
	rf_setup = (rf_setup & 0xD7) | ((bitrate & 0x02) << 5) | ((bitrate & 0x01) << 3);
	return nrf24l01_writeReg(RF_SETUP,rf_setup); //���� TX �������,0db����,2Mbps,���������濪��
}

//pwr: 0=-18dB  1=-12dB 2=-6dB 3=0dB
void nrf24l01_setPower(u8 pwr)
{
  rf_setup = (rf_setup & 0xF9) | ((pwr & 0x03) << 1);
	nrf24l01_writeReg(RF_SETUP,rf_setup); //���� TX �������,0db����,2Mbps,���������濪��
}


//���÷��͵�ַ
void nrf24l01_setTxAddr(u8 *addr,u8 len)
{	
	nrf24l01_setAddressWidth(len);
	nrf24l01_writeRegMulti(TX_ADDR,addr,len);	
}


//���÷��͵�ַ
void nrf24l01_setRxAddr(u8 ch,u8 *addr,u8 len)
{
	nrf24l01_setAddressWidth(len);
	nrf24l01_writeRegMulti(ch,addr,len);	
}


//���ý�����Ч���ݿ�� 0-32
//Ĭ��Ϊ0,ͨ��δʹ��
//ch: RX_PW_P0-RX_PW_P5
void nrf24l01_setRxPW(u8 ch,u8 width)
{
	packet_size = width;
	nrf24l01_writeReg(ch,width&0x3f);	//ѡ�����ͨ��0 ����Ч���ݿ��
}

//3-5
void nrf24l01_setAddressWidth(u8 width)
{
	if (width > 5) width = 5;
  if (width < 3) width = 3;  
	nrf24l01_writeReg(SETUP_AW,width-2);		//���õ�ַ���
}


//����Ƶ�� 0-127
void nrf_setRFCH(u8 ch)
{
	nrf24l01_writeReg(RF_CH,ch); //���� RF ͨ��
}

//�����ط�
//count:�ط�����0-15,Ϊ0ʱ��ֹ�ط�
//delay:�ط�ʱ��0-15, 0Ϊ250us,ÿλ��250us
void nrf24l01_setRetry(u8 count,u8 delay)
{
	nrf24l01_writeReg(SETUP_RETR,((delay<<4)&0xF0) | (count&0x0F));
}


//ʹ��Ӧ��
//ch��NRF_P0-NRF_P5 �����û�����
void nrf24l01_enableAck(u8 ch,u8 en)
{
	if(en)
	{
		nrf24l01_writeReg(EN_AA,ch); //ʹ��ͨ�� 0 ���Զ�Ӧ��
	}
	else
	{
		nrf24l01_writeReg(EN_AA,ch^ch); //ʹ��ͨ�� 0 ���Զ�Ӧ��
	}
}
//NRF_P0-NRF_P5 ,���û�����
void nrf24l01_enaleRxAddr(u8 ch)
{
	nrf24l01_writeReg(EN_RXADDR,ch); //ʹ��ͨ��0�Ľ��յ�ַ
}



//NRF_P0-NRF_P5 ,���û�����
void nrf24l01_enableDyn(u8 ch)
{
	nrf24l01_writeReg(DYNPD,ch);		//ʹ��ͨ��0�Ķ�̬���ݿ��(�����ݰ�Ӧ��Ĺ���������)
}

//ʹ�ܶ�̬���ݿ��,Ӧ��..
void nrf24l01_setFeature(u8 fea)
{
	nrf24l01_writeReg(FEATURE,fea);	
}


u8 nrf24l01_writePayload(u8 *dat,u8 length)
{
	u8 res,i;
	NRF_CSN = 0; //ʹ�� SPI ���� 
	
	res	= NRF_ReadWriteByte(WR_TX_PLOAD); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(i=0;i<length;i++)
	{
		NRF_ReadWriteByte(*dat++);
	}
	
	NRF_CSN = 1;
	return res;
}

u8 nrf24l01_readPayload(u8 *dat,u8 length)
{
	u8 res,i;
	NRF_CSN = 0; //ʹ�� SPI ���� 
	res	= NRF_ReadWriteByte(RD_RX_PLOAD); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(i=0;i<length;i++)
	{
		*dat++=NRF_ReadWriteByte(0xFF);
	}
	NRF_CSN = 1;	
	return res;
}

//============================================================




void nrf24l01_sendAckPacket(u8 *buf,u8 len)
{
	u8 i;	
	NRF_CSN = 0; //ʹ�� SPI ���� 
	NRF_ReadWriteByte(W_ACK_PAYLOAD); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(i=0;i<len;i++)
	{
		NRF_ReadWriteByte(*buf++);
	}
	NRF_CSN = 1;	
}

//32�ֽ�
u8 nrf24l01_sendPacket(void *txbuf,u8 len)
{
	u8 sta;
	nrf24l01_writePayload(txbuf,len);//д���ݵ�TX BUF 32 ���ֽ�
	while(NRF_IRQ == 1); 				//�ȴ��������
	sta=nrf24l01_readReg(STATUS); //��ȡ״̬�Ĵ�����ֵ 
	nrf24l01_writeReg(STATUS,sta); //��� TX_DS ��MAX_RT �жϱ�־
	
#ifdef NRF_ACKPAY_ENABLE
	if(sta&RX_OK)//�������
	{
		nrf_acklen = nrf24l01_getDynamicPayloadSize();
		nrf24l01_readPayload(nrf_ackbuf,nrf_acklen);//��ȡ����	
		return RX_OK;
	}
	if(sta&MAX_TX) //�ﵽ����ط�����
	{
		nrf24l01_FlushTx();
		return MAX_TX;
	}
#endif
	if(sta&TX_OK)//�������
	{
		return TX_OK;
	}
	return 0xff;//����ԭ����ʧ��
}





//32�ֽ�,������ɺ���������
u8 nrf24l01_sendPacket2(void *txbuf,u8 len)
{
	u8 sta;
	if(f_tx_wait)
	{
		while(NRF_IRQ == 1); 				//�ȴ��������
		sta=nrf24l01_readReg(STATUS); //��ȡ״̬�Ĵ�����ֵ 
		nrf24l01_writeReg(STATUS,sta); //��� TX_DS ��MAX_RT �жϱ�־
#ifdef NRF_ACKPAY_ENABLE
		if(sta&RX_OK)//�������
		{
			nrf_acklen = nrf24l01_getDynamicPayloadSize();
			nrf24l01_readPayload(nrf_ackbuf,nrf_acklen);//��ȡ����	
			return RX_OK;
		}
		if(sta&MAX_TX) //�ﵽ����ط�����
		{
			nrf24l01_FlushTx();
			return MAX_TX;
		}
#endif
	}
	
	nrf24l01_writePayload(txbuf,len);//д���ݵ�TX BUF 32 ���ֽ�
	f_tx_wait = 1;
	return 0xff;//����ԭ����ʧ��
}



//���ؽ������ݳ���,���󷵻�0
u8 nrf24l01_recvPacket(void *rxbuf)
{
	if(!NRF_IRQ)
	{
#ifdef NRF_ACKPAY_ENABLE
		u8 rxlen;	
		rxlen = nrf24l01_getDynamicPayloadSize();
		if(rxlen>0)
		{
			nrf24l01_readPayload(rxbuf,rxlen);//��ȡ����	
			nrf24l01_clearStatus();		
			return rxlen;
		}
#else
		nrf24l01_readPayload(rxbuf,packet_size);//��ȡ����	
		nrf24l01_clearStatus();
		return packet_size;
#endif
		
	}
	return 0;//û�յ��κ�����	
}



void nrf24l01_setTxRxMode(u8 mode)
{	
	if(mode == NRF_RXEN)
	{
		NRF_CE = 0;
		//�򿪽���ģʽ
		nrf24l01_writeReg(NRF_CONFIG,	NRF_CONFIG_ENRCC
																|NRF_CONFIG_CRCO
																|NRF_CONFIG_PWRUP
																|NRF_CONFIG_PRIMRX);
		
		nrf24l01_FlushTx();	nrf24l01_FlushRx();
		nrf24l01_clearStatus();
		NRF_CE = 1;
		delay_us(200);
	}
	else if(mode == NRF_TXEN)
	{
		NRF_CE = 0;
		nrf24l01_writeReg(NRF_CONFIG,NRF_CONFIG_ENRCC
																|NRF_CONFIG_CRCO
																|NRF_CONFIG_PWRUP);
		nrf24l01_FlushTx();	nrf24l01_FlushRx();
		nrf24l01_clearStatus();
		NRF_CE = 1;
		delay_us(200);
	}
	else
	{
		NRF_CE = 0;
		nrf24l01_writeReg(NRF_CONFIG,0);
		NRF_CE = 1;
		
	}
}







