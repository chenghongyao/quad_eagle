#ifndef _NRF_DEV_H
#define _NRF_DEV_H


// nRF24L01 ָ��
#define NRF_READ_REG    		0x00  // �����üĴ���+�Ĵ�����ַ�ͳ�	
#define NRF_WRITE_REG  			0x20  // д���üĴ��� +�Ĵ�����ַ�ͳ�
	
#define RD_RX_PLOAD 				0x61  //��RX��Ч����,1-32�ֽ�,���ֽ�0��ʼ
#define WR_TX_PLOAD 				0xA0  //дTX��Ч����,1-32�ֽ�,���ֽ�0��ʼ

#define FLUSH_TX    				0xE1  // ���TX FIFO�Ĵ���
#define FLUSH_RX    				0xE2  // ���RX FIFO�Ĵ���

#define R_RX_PL_WID					0x60					//��ȡ�������ݵĿ��(ʹ�ö�̬���ݿ�ȹ���),�����ֵ����32Ҫ���RX FIFO
#define W_ACK_PAYLOAD				0xA8			//дӦ��ʱ�����ݰ�,������д����Ӧ���
#define W_TX_PAYLOAD_NOACK 	0xB0 	//���ò�Ӧ��(���ú󲻻��Զ�Ӧ��)

#define NRF_NOP         0xFF  //�ղ���,������״̬�Ĵ���

#define REUSE_TX_PL 		0xE3  // ���·�����һ����Ч����
#define ACTIVATE				0x50
//===================================================



// nRF24L01 �Ĵ�����ַ
#define NRF_CONFIG    0x00  // ���üĴ���
#define EN_AA      		0x01  // ʹ���Զ�Ӧ����
#define EN_RXADDR  		0x02  // ����ͨ������(Ĭ��ͨ��0��ͨ��1 ����(1))
#define SETUP_AW    	0x03  // ���õ�ַ���(3,4,5)
#define SETUP_RETR  	0x04  // �����Զ��ط�(�����ط���ʱ(),�ط���������(0-15))
#define RF_CH       	0x05  // ��Ƶͨ��(���ù���Ƶ��(125��))
#define RF_SETUP    	0x06  // '��Ƶ�Ĵ���(���ݴ�������,���书��,�������Ŵ�������)

#define STATUS      	0x07  // ״̬�Ĵ���(�ж�Դ,��������ͨ����,�Ĵ�������־)
#define OBSERVE_TX  	0x08  // '���ͼ��Ĵ���
#define CD          	0x09  // �ز����

/**����ͨ����ַ**/
#define RX_ADDR_P0  	0x0A  // ����ͨ��0���ܵ�ַ(��д���ֽ�,��д�ֽ���SETUP_AW�趨)
#define RX_ADDR_P1  	0x0B  // ����ͨ��1���ܵ�ַ
#define RX_ADDR_P2  	0x0C  // ����ͨ��2���ܵ�ַ
#define RX_ADDR_P3  	0x0D  // ����ͨ��3���ܵ�ַ
#define RX_ADDR_P4  	0x0E  // ����ͨ��4���ܵ�ַ
#define RX_ADDR_P5  	0x0F  // ����ͨ��5���ܵ�ַ
/**���͵�ַ**/
#define TX_ADDR     	0x10  // ���͵�ַ(ʹ���Զ�Ӧ����ʱ ����ͨ��0�ĵ�ַӦ������)
/**����ͨ����Ч���ݳ���**/
#define RX_PW_P0    	0x11  //  ����ͨ��0��Ч���ݳ���(0-32�ֽ�)
#define RX_PW_P1    	0x12  //  ����ͨ��2��Ч���ݳ���(0-32�ֽ�)
#define RX_PW_P2    	0x13  //  ����ͨ��3��Ч���ݳ���(0-32�ֽ�)
#define RX_PW_P3    	0x14  //  ����ͨ��3��Ч���ݳ���(0-32�ֽ�)
#define RX_PW_P4    	0x15  //  ����ͨ��4��Ч���ݳ���(0-32�ֽ�)
#define RX_PW_P5    	0x16  //  ����ͨ��5��Ч���ݳ���(0-32�ֽ�)

#define FIFO_STATUS 	0x17  // FIFI״̬�Ĵ���


//����ΪNRF2401+ ���еļĴ���
#define DYNPD 				0x1C			//�������ø�ͨ���Ƿ�Ϊ��̬���ݿ�� ��Ҫ��ʹ��FEATURE���EN_DPL
#define FEATURE				0x1D		//����ʹ�ܶ�̬���ݿ��,�����ݰ�Ӧ��,��Ӧ����




#endif
