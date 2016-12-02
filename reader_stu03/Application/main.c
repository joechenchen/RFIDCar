#include "nrf.h"
#include "nrf_gpio.h"
#include "mxgpio.h"
#include "radio_config.h"
#include "simple_uart.h"
#include "tim.h"
#include "nrf_nvmc.h"
#include "sys.h"
#include "stdlib.h"
#include "common.h"
#include "crc.h"
/*
Sta[0] .7�͵�ָʾ 1:�͵�
			 .6~4��������
			 .3~0��������
Sta[1] �ɸ�����
*/		
//update
//#if BOOTLOADER
//#define _UPD_VARIABLE
//#endif

IRQn_Type i;
#include "update.h"
#define	CAPACITY	60		//��ǩ����
struct {
	uint8_t aucID[4];//ultrasonic tag
	uint8_t	Age;     //ʱ��
	uint8_t Sta[2];		//state
	uint8_t info[10];
}astRFID[CAPACITY];	
#define US_Leave_time 3 //6����Ϊ��������ǩ�뿪
uint8_t const constTagID[4] = {0xfd,0,0,0};
typedef struct
{
	uint8_t ID[4];//���ò���Ŀ��ID
	uint8_t payload[32];//�����������ݷ���
	uint8_t state;//���ñ�־λ 1:�ɹ�
	uint8_t rx_time;//���մ���	
	uint8_t send_flag;
}config_para;
config_para config;

extern uint8_t radio_status;
extern uint8_t payload[PACKET_PAYLOAD_MAXSIZE];

//���¼�������������¼ROM�е�16/32�飨ƫ�������ļ���ֵ ֵ16/32����ʾ��¼������0��ʾδ��¼
//255��������0x3fc00
//254����rom0 �洢Ƶ������  0x3f800
//253����rom1 ������ 0x3f400
//252����rom2 �û���1 0x3f00
//251����rom3 �û���2 0x3ec00
uint8_t ucRomMark;//������һ��������
uint8_t	ucROM0;		//�����ڶ��������ڲ�������
uint8_t	ucROM1;		//����������������������
uint8_t	ucROM2;	  //�������ĸ��������û���1
uint8_t	ucROM3;   //�������������,�û���2
uint8_t *pROM;	//��¼ָ��
uint8_t	* caucpROM[]={&ucROM0,&ucROM1,&ucROM2,&ucROM3};	
uint32_t Page_Base[5];//page addr
#define para_area 1
#define reserved_area 2
#define user_area1 3
#define user_area2 4
//1��2��16����¼��3��4��32����¼
//ÿ����¼16���ֽ�
uint8_t Rom_record_size[4] = {16,16,32,32};//4��������Ӧ�ļ�¼����
uint8_t Rom_record_byte[4] = {16,16,16,16};//ÿ����¼��Ӧ���ֽ���
#define Rom0_record_size 16
#define Rom1_record_size 16
#define Rom2_record_size 32
#define Rom3_record_size 32
#define Rom0_record_byte 16
#define Rom1_record_byte 16
#define Rom2_record_byte 16
#define Rom3_record_byte 16
//uint8_t nvtempRom0[Rom0_record_size][Rom0_record_byte];
//uint8_t nvtempRom1[Rom1_record_size][Rom1_record_byte];
//uint8_t nvtempRom2[Rom2_record_size][Rom2_record_byte];
//uint8_t nvtempRom3[Rom3_record_size][Rom3_record_byte];


//flash������¼
//����洢������С����mcu�ͺž���.
/*FICR�Ĵ����е�CODEPAGESIZE��Ӧ��ҳ������CODESIZE��Ӧҳ������memory��С
CODEPAGESIZE*CODESIZE��ΪROM�Ĵ�С,pg_size=1024,pg_num = 256,256KB�Ĵ���洢����FLASH��
����洢�������һҳ�洢�û�������Ϣ*/
uint32_t pg_size;//һҳ��Ӧ���ֽ���
uint32_t pg_num;//ҳ����
uint32_t addr;
//user-defined MARK
const uint8_t nvmc_flash_mark[11]={0xaa,0xaa,0x01,0x03,0x05,0x01,0x23,0x45,0x67,0x89,0x8e};

/*
485�������ڲ���������
[0]	.7~6 	0		Ӳ������
		.5~0	0~15	�汾��
[1]	.7~4	6		���书�ʣ�0- (-30dBm), 1- (-20dBm), 2- (-16dBm), 3- (-12dBm)
													4- (8dBm), 5- (4dBm), 6- (0dBm), 7- (4dBm)
		.3~2	0~2		�Զ��ϱ�Я������Ϣ��Դ��0-��������1-�û���1, 2-�û���2
		.1~0	0		ʹ�����ڷ��� 1-����
[2]	.7~0	0		����
[3]	.7~0	0		����
[4]	.7		0		����		
		.6		0/1		˥��������0-�շ��Գ�˥����1-����˥��
		.5		0/1		����RPD���ˣ�1-����
		.4~0	0~31	˥��ֵ��0~31-��С0dB���31dB
[5~8]				�󶨵ĳ�����̽���ǩID��MSB���ȣ�
[5]	.7~0	0xfe~0xfc
							0xFEXXXXXX-�������еĳ�������ǩ������󶨣�
							0xFDXXXXXX-����ƥ��ĳ�������ǩ���󶨣�
							0xFCXXXXXX-�����ճ�������ǩ				
*/
//5�ֽ���Ƶ��ַ������Ƶ��������Ƶ��,���书��
#define	HWTYP		0 		//0-ƻ������˥������1-�̺�壨��˥����
#define	VERSION		0x02
#define data_channel 79//����Ƶ������ǩ�Զ��ϱ� 
#define config_channel 5//����Ƶ��
uint8_t const ParaSet[16]={0xe7,0xe7,0xe7,0xe7,0xe7,79,5,0,0x61,0,0,0,0,0,0,0};	//Ĭ�ϲ���
//[0]~[6] read-only
//[7]~[15] can write
uint8_t CurParaSet[16];//��ǰ���в���,����ParaSet�Ĳ���
//���书��0,
//#define RADIO_TXPOWER_TXPOWER_0dBm (0x00UL) /*!< 0dBm. */
//#define RADIO_TXPOWER_TXPOWER_Pos4dBm (0x04UL) /*!< +4dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg30dBm (0xD8UL) /*!< -30dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg20dBm (0xECUL) /*!< -20dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg16dBm (0xF0UL) /*!< -16dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg12dBm (0xF4UL) /*!< -12dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg8dBm (0xF8UL) /*!< -8dBm. */
//#define RADIO_TXPOWER_TXPOWER_Neg4dBm (0xFCUL) /*!< -4dBm. */
//0- (-30dBm), 1- (-20dBm), 2- (-16dBm), 3- (-12dbm) 4- (-8dBm)  5- (-4dBm) 6- (0dBm) 7- (4dBm)
uint8_t	const caucTXPOWER[8]={0xd8,0xec,0xf0,0xf4,0xf8,0xfc,0x00,0x04};
//uint8_t data_channel = 79;//����Ƶ������ǩ�Զ��ϱ� 
//uint8_t config_channel = 5;//����Ƶ��
//uint8_t caucCH[2] ={79,5};
uint8_t my_tx_power;//���书�ʣ���ParaSet[8] .7~4����
uint8_t En_Period_Tx;//enable period tx ��ParaSet[8] .0����
//��Ƶ����Ƶ��״̬���������ö���
typedef enum
{
	run_config_channel,
	run_data_channel,
	run_idle
}radio_run_chaneel;
uint8_t radio_channel = run_idle;
uint8_t config_send_state=0; //transmit mode switch 1:switch to config channel rx mode  2:config channel tx mode
typedef enum
{
	config_start=1,//��ʼ������������
	config_timeout,//�������ó�ʱ
	config_success//�������óɹ�
}rf_config_state;
uint8_t rf_cfg_state;//���óɹ�

//Ĭ��2s����һ������ID,10s����һ�ν��մ��ڣ���������������Ϣ
uint8_t caucID[4]={0xff,0xff,0xff,0xf9};		//MSB first ������ID
uint8_t	withRFRx=0;//1:with receive window,������λ��Я�����մ��ڡ�
uint8_t	ucCntRFRx=0;//10sЯ�����մ���
#define RF_RX_FREQ 5
uint8_t	bRFRxed=0;//�յ�������ָʾ
uint8_t bRFRPD=0;	//RPD��Чָʾ
//���ڲ�������
uint8_t uart_tx_en;//���ڷ���ʹ��
uint16_t tx_len;//���ڳ���
uint16_t pkt_len_temp;//��len����
UartData_TypeDef  U_Master; //����
PACKET_TypeDef RX_PACKET;
uint8_t tx_buf[1000]; 
uint8_t *ptx_temp;
//������ָ�����
typedef enum
{
	//�г���ǩ���
	idle_mode=0,
	list_Tag,
	config_Tag,
	advertise,
}work_Type;
uint8_t work_mode = 0;

//TIM0
//���1S����һ��RFID��Ϣ
#define	RFID_CYCLE		20  //CC:50MS*20 = 1s
uint8_t RFID_TX_EN; //CC:������Ƶ����
uint8_t ucCycRFID;
uint8_t timeout_cnt_start;//start cnt
uint8_t timeout_cnt;//timeout cnt

uint16_t time_cnt;//����ֵ
uint16_t time_cycle= 0xff;//��������
uint8_t time_en;//����ʹ��



static void hfclk_config(void)
{
    // Start 16 MHz crystal oscillator.
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;

    // Wait for the external oscillator to start up.
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
    {
        // Do nothing.
    }
}

//�������¼�¼ROMλ��
uint8_t Rom_Pos(uint32_t temp_addr,uint8_t temp_size,uint8_t temp_byte)
{
	uint8_t i,j;
	uint8_t (* temp_buf)[16];
	temp_buf = (uint8_t(*)[16])malloc(temp_size*temp_byte*sizeof(uint8_t));
	//��ȡbuff
	for(i=0;i<temp_size;i++)
	{
		for(j=0;j<temp_byte;j++)
		{
			temp_buf[i][j]=*(uint8_t*)temp_addr++;
		}
	}
	//��֤buff�Ƿ�Ϊ�գ����ؼ�¼�е����¼�¼
	//����ȣ���������,�ҵ��գ��򷵻�romλ�ã�i =  1~~Rom_record_size������0��ʾ������ȫ��
	for(i=0;i<temp_size;)
	{
		for(j=0;j<temp_byte;j++)
		{
			if(temp_buf[i][j]!=0xff) break;
		}
		if(j>=temp_byte) break;
		i++;
	}
	return i;
}

void UART_Init(void)
{
    simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);  
	  //simple_uart_config( TX_PIN_NUMBER,RX_PIN_NUMBER, HWFC);  
	  //NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
	  NRF_UART0->INTENSET = (UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos);
		NRF_UART0->INTENSET = UART_INTENSET_ERROR_Msk;
		NVIC_SetPriority(UART0_IRQn, 0);
    NVIC_EnableIRQ(UART0_IRQn);
}
//������ʼ��
void settingsSetDefaultValues(void)
{
	uint16_t flash_i,mm;
	uint8_t flash_temp[11];//temp memory
	//flash����,����洢������С����mcu�ͺž���.
	/*FICR�Ĵ����е�CODEPAGESIZE��Ӧ��ҳ������CODESIZE��Ӧҳ������memory��С
	CODEPAGESIZE*CODESIZE��ΪROM�Ĵ�С,pg_size=1024,pg_num = 256,256KB�Ĵ���洢����FLASH��
	����洢�������һҳ�洢�û�������Ϣ*/
	pg_size = NRF_FICR->CODEPAGESIZE;
	pg_num  = NRF_FICR->CODESIZE - 1;
	//para area
	Page_Base[0] = (pg_size * (pg_num-para_area));
	//reserved area
	Page_Base[1] = (pg_size * (pg_num-reserved_area));
	//user area1
	Page_Base[2] = (pg_size * (pg_num-user_area1));
	//user area2
	Page_Base[3] = (pg_size * (pg_num-user_area2));
	
	addr = (pg_size * pg_num);
	nrf_nvmc_read_bytes(addr,flash_temp,11);
	
	//���һ�������������ǣ�����գ������ROM0-ROM5�洢��,�ж��Ƿ��������صĳ���
	if((flash_temp[0]!=nvmc_flash_mark[0])||(flash_temp[1]!=nvmc_flash_mark[1])||(flash_temp[2]!=nvmc_flash_mark[2])
		||(flash_temp[3]!=nvmc_flash_mark[3])||(flash_temp[4]!=nvmc_flash_mark[4])||(flash_temp[5]!=nvmc_flash_mark[5])
		||(flash_temp[6]!=nvmc_flash_mark[6])||(flash_temp[7]!=nvmc_flash_mark[7])||(flash_temp[8]!=nvmc_flash_mark[8])
		||(flash_temp[9]!=nvmc_flash_mark[9])||(flash_temp[10]!=nvmc_flash_mark[10]))
	{			 
		addr = Page_Base[0];		
		nrf_nvmc_page_erase(addr);

		addr = Page_Base[1];
		nrf_nvmc_page_erase(addr);

		addr = Page_Base[2];
		nrf_nvmc_page_erase(addr);

		addr = Page_Base[3];
		nrf_nvmc_page_erase(addr);

		addr = (pg_size * pg_num);
		nrf_nvmc_page_erase(addr);
				
		addr = (pg_size * pg_num);
	
		nrf_nvmc_write_bytes(addr,nvmc_flash_mark,11);
	}
	else
	{
		//�����Ƕ�ȡ�ϴδ洢����Ϣ
		//������Ϣ	ucROM0
		addr = Page_Base[0];
		ucROM0 = Rom_Pos(addr,Rom0_record_size,Rom0_record_byte);
		//reserved area
		addr = Page_Base[1];
		ucROM1 = Rom_Pos(addr,Rom1_record_size,Rom1_record_byte);
		//user area1
		addr = Page_Base[2];
		ucROM2 = Rom_Pos(addr,Rom2_record_size,Rom2_record_byte);
		//user area2
		addr = Page_Base[3];
		ucROM3 = Rom_Pos(addr,Rom3_record_size,Rom3_record_byte);			
	}
	//�ָ����в���
	//test
//	for(ucROM0=0;ucROM0<15;ucROM0++)
//	{
//		for(flash_i=0;flash_i<16;flash_i++)
//		payload[flash_i] = flash_i;
//		addr = pg_size * (pg_num-para_area);
//		nrf_nvmc_write_bytes(addr+ucROM0*Rom0_record_byte,payload,16);
//	}
	if(ucROM0)
	{
		addr = Page_Base[0];
		flash_i = (ucROM0 - 1)*Rom0_record_byte;
		nrf_nvmc_read_bytes(addr+flash_i,CurParaSet,Rom0_record_byte);
	}
	else//����Ĭ�ϲ���
	{
		for(flash_i = 0;flash_i<Rom0_record_byte;flash_i++)
		{
			CurParaSet[flash_i] = ParaSet[flash_i];
		}
	}
	my_tx_power = (uint32_t)caucTXPOWER[CurParaSet[8]>>4];//0dbm
	En_Period_Tx = CurParaSet[8]&0x01;
	
	for(mm=0;mm<CAPACITY;mm++)
	{
		astRFID[mm].aucID[0]=0xff;
		astRFID[mm].aucID[1]=0xff;
		astRFID[mm].aucID[2]=0xff;
		astRFID[mm].aucID[3]=0xff;
		astRFID[mm].Sta[0]=0xff;
		astRFID[mm].Sta[1]=0xff;
		astRFID[mm].Age=0xff;

	}
}
#define head_bytes 15
void uart_process(void)
{
	static uint8_t rfid_j,rfid_i;
	static uint8_t cycle;
	static uint8_t tx_XOR;
	static uint8_t i_xor;
	static uint8_t ii,jj;
	static uint16_t up_crc;
//	static uint8_t ii,jj;
	if(U_Master.MUART_RC_END_FLAG)
	{
		time_en = 0;
		time_cnt =0;
		time_cycle=0xff;
		U_Master.MUART_RC_END_FLAG = 0;
		RX_PACKET.XOR = 0;
		for(i_xor=6;i_xor<RX_PACKET.PackLen;i_xor++) RX_PACKET.XOR^=U_Master.Uart_RxBuffer[i_xor];
		if(!RX_PACKET.XOR)
		{
			
			if(U_Master.Uart_RxBuffer[8]==0XFF&&U_Master.Uart_RxBuffer[9]==0XFF
			&&U_Master.Uart_RxBuffer[10]==0XFF&&U_Master.Uart_RxBuffer[11]==0XFE)
			{
				RX_PACKET.type = U_Master.Uart_RxBuffer[14];
				if(0x08 == (RX_PACKET.type&0X0f))
				{
					cycle = RX_PACKET.type>>4;
					if(cycle)
					{
						for(rfid_j=0;rfid_j<CAPACITY;rfid_j++)
						{
							astRFID[rfid_j].aucID[0]=0xff;
							astRFID[rfid_j].aucID[1]=0xff;
							astRFID[rfid_j].aucID[2]=0xff;
							astRFID[rfid_j].aucID[3]=0xff;
							astRFID[rfid_j].Sta[0]=0;
							astRFID[rfid_j].Sta[1]=0;
							astRFID[rfid_j].Age=0;
							for(rfid_i=0;rfid_i<10;rfid_i++)
							{
								astRFID[rfid_j].info[rfid_i]=0xff;
							}
						}	
						radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit, data_channel);
						radio_channel = run_data_channel;
						time_en = 1;
						time_cnt= 0;//ʱ�����
						time_cycle = cycle*40;
						work_mode = list_Tag;
						NRF_UART0->TASKS_STOPRX = 1;  //����ֹͣ����
					}		
					else
					{
						time_en = 1;
						time_cnt= 0;//ʱ�����
						time_cycle = 5;
						work_mode = list_Tag;
						NRF_UART0->TASKS_STOPRX = 1;  //����ֹͣ����
					}
				}
				else if(0x0a == (RX_PACKET.type&0X0f))//��չ��Ϣ
				{
					if(RX_PACKET.type>>7)//1-���յ�Ŀ���ǩ������ظ�
					{
						cycle = (RX_PACKET.type>>4)&0x07;
						if(0 == cycle)//0�޳�ʱ��ֱ�����յ���ǩ��Ϣ��������ָ�����
						{
							config.rx_time = 0;
							config.state = 0;
							config.send_flag = 0;
							config.ID[0] = U_Master.Uart_RxBuffer[15];config.ID[1] = U_Master.Uart_RxBuffer[16];
							config.ID[2] = U_Master.Uart_RxBuffer[17];config.ID[3] = U_Master.Uart_RxBuffer[18];
							radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit, data_channel);
							radio_channel = run_data_channel;
							work_mode = config_Tag;
						}
						else
						{
							config.rx_time = 0;
							config.state = 0;
							config.send_flag = 0;
							config.ID[0] = U_Master.Uart_RxBuffer[15];config.ID[1] = U_Master.Uart_RxBuffer[16];
							config.ID[2] = U_Master.Uart_RxBuffer[17];config.ID[3] = U_Master.Uart_RxBuffer[18];
							radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit, data_channel);
							radio_channel = run_data_channel;
							time_en = 1;
							time_cnt= 0;//ʱ�����
							time_cycle = cycle*40;
							work_mode = config_Tag;
							NRF_UART0->TASKS_STOPRX = 1;  //����ֹͣ����	

						}
						
					}
					else
					{
						work_mode = advertise;
						//ת������ָ��
						payload[1]=0x0a;
						payload[2]=config.ID[0];
						payload[3]=config.ID[1];
						payload[4]=config.ID[2];
						payload[5]=config.ID[3];
						payload[6]=0;
						payload[7]=CurParaSet[7];
						payload[8]=0;
						payload[9]=0;
						payload[10]=0;
						payload[11]=0;
						for(ii=12,jj=19;ii<32;ii++,jj++) payload[ii]=U_Master.Uart_RxBuffer[jj];//��չ��Ϣ20�ֽ�

						for(ii=0,jj=1;jj<32;jj++) ii^=payload[jj];
						payload[0]=ii;
						radio_tx_carrier(RADIO_TXPOWER_TXPOWER_Pos4dBm,RADIO_MODE_MODE_Nrf_1Mbit, config_channel);
						radio_channel = run_config_channel;						
					}
//				if((U_Master.Uart_RxBuffer[8]==0xff&&U_Master.Uart_RxBuffer[9]==0xff
//				&&U_Master.Uart_RxBuffer[10]==0xff&&U_Master.Uart_RxBuffer[11]>=0xf0)
//				{
//					if(U_Master.Uart_RxBuffer[11]<0xfc)
//					{
//						
//					}
//				}
//					payload[1]=0x0a;
//					payload[2]=U_Master.Uart_RxBuffer[8];
//					payload[3]=U_Master.Uart_RxBuffer[9];
//					payload[4]=U_Master.Uart_RxBuffer[10];
//					payload[5]=U_Master.Uart_RxBuffer[11];
//					payload[6]=0;
//					payload[7]=CurParaSet[7];
//					payload[8]=0;
//					payload[9]=0;
//					payload[10]=0;
//					payload[11]=0;
//					for(ii=12,jj=15;ii<32;ii++,jj++) payload[ii]=U_Master.Uart_RxBuffer[jj];
//	
//					for(ii=0,jj=1;jj<32;jj++) ii^=payload[jj];
//					payload[0]=ii;
//					
//					radio_tx_carrier(RADIO_TXPOWER_TXPOWER_Pos4dBm,RADIO_MODE_MODE_Nrf_1Mbit, config_channel);
				
				}
			}
		}
	}	
	#if BOOTLOADER
	if(U_Master.UP_RC_END_FLAG)
	{
		U_Master.UP_RC_END_FLAG = 0;

		up_crc = crc_check(&U_Master.Uart_RxBuffer[0]);
		if(up_crc == (U_Master.Uart_RxBuffer[10]<<8|U_Master.Uart_RxBuffer[11]))
		{
			if(U_Master.Uart_RxBuffer[2]==caucID[0]&&U_Master.Uart_RxBuffer[3]==caucID[1]
			&&U_Master.Uart_RxBuffer[4]==caucID[2]&&U_Master.Uart_RxBuffer[5]==caucID[3])
			{
				if(0X09 == U_Master.Uart_RxBuffer[7])
				{
//					time_en = 0;
					GotoBootLoader(U_Master.Uart_RxBuffer[6]);
				}
			}
		}
			
	}
	#endif

	if(time_cnt >= time_cycle || 1 == config.state)
	{
		time_en = 0;
		time_cnt = 0;
		time_cycle = 0Xff;
		if(list_Tag == work_mode)
		{
			radio_disable();
			ptx_temp=tx_buf;
			*ptx_temp++ = pkt_head1;
			*ptx_temp++ = pkt_head2;
			*ptx_temp++ = pkt_head3;
			*ptx_temp++ = pkt_head4;
			*ptx_temp++ = pkt_head5;
			*ptx_temp++ = pkt_head6;//6bytes head
			*ptx_temp++ =0;//len high
			*ptx_temp++ =0;//len low  2 bytes len
			*ptx_temp++ = caucID[0];
			*ptx_temp++ = caucID[1];
			*ptx_temp++ = caucID[2];
			*ptx_temp++ = caucID[3];//4 bytes id
			*ptx_temp++ = 0;
			*ptx_temp++ = 0;//2bytes attribute
			*ptx_temp++ = 0;//1byte type
			//15bytes
			tx_len = 0;
			for(rfid_j=0;rfid_j<CAPACITY;rfid_j++)//�ϱ�����ı�ǩ
			{
				if(astRFID[rfid_j].aucID[0]!=0xff)
				{
					*ptx_temp++ =astRFID[rfid_j].aucID[0];
					*ptx_temp++ =astRFID[rfid_j].aucID[1];
					*ptx_temp++ =astRFID[rfid_j].aucID[2];
					*ptx_temp++ =astRFID[rfid_j].aucID[3];
					*ptx_temp++ =astRFID[rfid_j].Sta[0];
					*ptx_temp++ =astRFID[rfid_j].Sta[1];//6bytes
					for(rfid_i=0;rfid_i<10;rfid_i++)
					{
						*ptx_temp++ = astRFID[rfid_j].info[rfid_i];
					}
					tx_len++;
				}
			}
			NRF_UART0->TASKS_STARTRX    = 1; //�����������չ���
			NRF_UART0->EVENTS_RXDRDY    = 0; 
			tx_len=tx_len<<4;//ÿ����ǩ16�ֽ�
			pkt_len_temp = 8 + tx_len;//4Addr + 2Attr + 1Type + Info
			tx_buf[6] = pkt_len_temp>>8;
			tx_buf[7] = pkt_len_temp;
			tx_len = tx_len +15;//ͷ15�ֽ�+info
			tx_XOR = 0;
			for(i_xor=6;i_xor<tx_len;i_xor++) tx_XOR = tx_XOR ^ tx_buf[i_xor];
			*ptx_temp++ = tx_XOR;
			tx_len = tx_len + 1;//���ֽ��������15+60*16+1=976�ֽ�
			ptx_temp = tx_buf;
			UART_Send(ptx_temp,tx_len);	
			work_mode = 0;
		}
		else if(config_Tag == work_mode)
		{
			radio_disable();
			NRF_UART0->TASKS_STARTRX    = 1; //�����������չ���
			NRF_UART0->EVENTS_RXDRDY    = 0; 
			
//			ptx_temp=tx_buf;
			tx_buf[0] = pkt_head1;
			tx_buf[1] = pkt_head2;
			tx_buf[2] = pkt_head3;
			tx_buf[3] = pkt_head4;
			tx_buf[4] = pkt_head5;
			tx_buf[5] = pkt_head6;//6bytes head
			tx_buf[6] =0;//len high
			tx_buf[7] =0x20;//len low  2 bytes len
			tx_buf[8] = caucID[0];
			tx_buf[9] = caucID[1];
			tx_buf[10] = caucID[2];
			tx_buf[11] = caucID[3];//4 bytes id
			tx_buf[12] = 0;
			tx_buf[13] = 0;//2bytes attribute
			tx_buf[14] = 0X02;//1byte type
			if(1 == config.state)
			{
				copybuf(&tx_buf[15],&config.payload[2],4);
				copybuf(&tx_buf[19],&config.payload[12],20);
				config.state = 0;
			}
			else
			{
				my_memset(&tx_buf[15],20);
			}
			tx_len = 40;
			tx_XOR = 0;
			for(i_xor=6;i_xor<tx_len-1;i_xor++) tx_XOR = tx_XOR ^ tx_buf[i_xor];
			tx_buf[39]=tx_XOR;
			UART_Send(tx_buf,tx_len);	
			work_mode = 0;
			config.rx_time = 0;
			config.state = 0;
			config.send_flag = 0;
		}
	}
}

void rf_process(void)
{
	static uint8_t ii;
	static uint8_t jj;
	//��ʱϵͳ
	/*-------------------RFID----------------------------*/
	//2s����һ�η��䣬10s����һ�ν��գ�������Я�����մ���
	if(RFID_TX_EN&&En_Period_Tx)
	{
//			withRFRx = 1;//Я�����մ���
		radio_disable();
		RFID_TX_EN = 0;
		//1.tx
		//׼������
		for(ii=0;ii<32;ii++)
		{
			payload[ii] = 0;
		}
		//10s with receive window
		if(++ucCntRFRx>=RF_RX_FREQ) 
		{
			ucCntRFRx=0;
		}
		withRFRx = ucCntRFRx?0:1;
		//rf transmit payload defined
		/*
		0:xor
		1:seq
		2~5:id FEFDFFFF~FEFD0000
		6:is with receive rf window?
		7:reserve
		8:sensor type
		9:sensor data
		10:sensor para
		11:current counter
		/8~11 test des receiver ID
		12:7bit receive instructions   6:1-PDR active
		*/
		payload[1]=0;
		payload[2]=caucID[0];
		payload[3]=caucID[1];
		payload[4]=caucID[2];
		payload[5]=caucID[3];
		payload[6]=withRFRx?0x80:0;
		payload[7]=CurParaSet[7];	
		payload[8]=0;
		payload[9]=0;
		payload[10]=0;
		payload[11]=0;
		ii=0;
		if(bRFRxed) ii|=0x80;//�յ�ָ��
		if(bRFRPD) ii|=0x40;//RPD�Ƿ���Ч
		payload[12]=ii;
		payload[13]=0;
		payload[14]=0;
		payload[15]=0;
		//is with ext area?
		switch(CurParaSet[8]&0x0c)
		{
			case 0:
				if(ucROM1)
				{
					payload[13]=0x60|(ucROM1-1);
					ii = (ucROM1 - 1)*Rom1_record_byte;
					nrf_nvmc_read_bytes(addr+ii,&payload[16],Rom1_record_byte);
				}
				else
				{
					payload[13]=0x60;
					for(ii=0;ii<16;ii++) payload[16+ii]=0;	//��0���
				}					
				break;
			case 0x04:
				if(ucROM2)
				{
					payload[13]=0xa0|(ucROM2-1);
					ii = (ucROM2 - 1)*Rom2_record_byte;
					nrf_nvmc_read_bytes(addr+ii,&payload[16],Rom2_record_byte);
//						for(ii=0;ii<16;ii++) payload[16+ii]=nvaucPage2[ucROM2-1][i];
				}
				else
				{
					payload[13]=0xa0;
					for(ii=0;ii<16;ii++) payload[16+ii]=0;	//��0���
				}
				break;
			case 0x08:					
				if(ucROM3)
				{
					payload[13]=0xe0|(ucROM3-1);
					ii = (ucROM3 - 1)*Rom3_record_byte;
					nrf_nvmc_read_bytes(addr+ii,&payload[16],Rom3_record_byte);
//						for(ii=0;ii<16;ii++) payload[16+i]=nvaucPage3[ucROM3-1][i];
				}
				else
				{
					payload[13]=0xe0;
					for(ii=0;ii<16;ii++) payload[16+ii]=0;	//��0���
				}
				break;
			default:
				break;
		}
		//XOR
		for(ii=31,jj=0;ii;ii--) jj^=payload[ii];
		payload[ii]=jj;
		
		//����
		RFN_TX_MODE;//RW=0-����  RW=1-����	
		//2s�¼�����������Ƶ��ID,��Ƶ�ж��п�������Ƶ�����գ�Ȼ��50ms���������Ƶ�������ݽ�������ϸ˵������Ƶ����ͼ�ĵ
		if(withRFRx)//Я�����մ���
		{
			withRFRx = 0;//clear recive window
			config_send_state = 1;//rf�ж�����������ý���
			timeout_cnt_start = 1;//��ʼ��ʱ����
			timeout_cnt = 0;//��ʱ����ֵ��50ms�ص�����Ƶ������
		}			
		
		radio_tx_carrier(my_tx_power,RADIO_MODE_MODE_Nrf_1Mbit,data_channel);
		radio_channel = run_data_channel;

	}
	if( timeout_cnt>=1 )//time-out
	{
		timeout_cnt_start = 0;
		timeout_cnt = 0;
		rf_cfg_state =config_timeout; //time-out	
	}
	if(rf_cfg_state>1)//time-out or success,switch to rx datachannel
	{
		RFN_RX_MODE;//RW=0-����  RW=1-����		
		radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,data_channel);
		radio_channel = run_data_channel;
		//clear 
		bRFRxed = 0;
		bRFRPD = 0;
		config_send_state = 0;
		rf_cfg_state = 0;
	}
}

int main(void)
{

	AppFlag = __ON_APP;
	hfclk_config();//��ʱ�ӳ�ʼ��
	settingsSetDefaultValues();//��������
	Radio_Init();//��Ƶ��ʼ��
	Timer0_Init(50);//ʱ�ӳ�ʼ��
	UART_Init();//����485��ʼ��
	nrf_gpio_cfg_output(LED);
	LED_ON;
	//bootloader ��Ҫ��ID_BEGINλ�ñ���ID��
	nrf_nvmc_write_bytes(ID_BEGIN,caucID,4);
	NRF_TIMER0->TASKS_START    = 1; // Start timer. 
	while(1)
	{
		//ƽʱ���ڽ���״̬��һ�д���ָ�ִ�д�������
		uart_process();
		if(work_mode==0)
		{
			rf_process();
		}
	}
}
/*���ڽ����ж�*/
void UART0_IRQHandler()
{
	uint8_t rx_temp;
	if(NRF_UART0->EVENTS_RXDRDY)
	{
		NRF_UART0->EVENTS_RXDRDY=0;
		rx_temp = NRF_UART0->RXD;
		switch(U_Master.PKT_RX_STATE)
		{
			case PKT_HEAD1:																						//֡ͷ1
						if(pkt_head1==rx_temp)	                                
						{                                                   
							U_Master.rx_cnt = 0;													  	//���մ�����0
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//�������BUFF
							U_Master.PKT_RX_STATE = PKT_HEAD2;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1
						}
						#if BOOTLOADER
						else if(up_head1 == rx_temp)
						{
							U_Master.rx_cnt = 0;													  	//���մ�����0
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//�������BUFF
							U_Master.PKT_RX_STATE = UP_HEAD2;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1							
						}
						#endif
						break;                                              
			case PKT_HEAD2:																						//֡ͷ2                                  
						if(pkt_head2==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		// �������BUFF
							U_Master.PKT_RX_STATE = PKT_HEAD3;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1
						}                                                   
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}						                                        
						break;                                              
			case PKT_HEAD3:																						//֡ͷ3                                  
						if(pkt_head3==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//�������BUFF 
							U_Master.PKT_RX_STATE = PKT_HEAD4;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1
						}	                                                  
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}									                                  
						break;                                              
			case PKT_HEAD4:																						//֡ͷ4                                  
						if(pkt_head4==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//���մ�����0
							U_Master.PKT_RX_STATE = PKT_HEAD5;								 		//״̬�л� 
							U_Master.rx_cnt++;														 		//���մ���+1
						}		                                                
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}									                                  
						break;	
			case PKT_HEAD5:																						//֡ͷ4                                  
						if(pkt_head4==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//���մ�����0
							U_Master.PKT_RX_STATE = PKT_LEN_ID;								 		//״̬�л� 
							U_Master.rx_cnt++;														 		//���մ���+1
						}		                                                
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}									                                  
						break;						                                           
			case PKT_LEN_ID:																					//���հ����ȶ����ֽ���
						U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;			//�������buff
						if(cont_array_pos == U_Master.rx_cnt)								//�жϰ��ֽڳ����Ƿ�������
						{                                                   
							U_Master.PKT_RX_STATE = PKT_LEN;									//״̬�л�
							U_Master.rx_cont_len = ((U_Master.Uart_RxBuffer[U_Master.rx_cnt-1] << 8) | (U_Master.Uart_RxBuffer[U_Master.rx_cnt]));
							if(U_Master.rx_cont_len > 200)										//������һ�㲻����200����������ص���ʼ״̬
							{                                                 
								U_Master.PKT_RX_STATE = PKT_HEAD1;              
							}                                                 
							else if(0 == U_Master.rx_cont_len)				  			//��������Ϊ0
							{                                                 
								U_Master.PKT_RX_STATE = PKT_REAR1;              
							}                                                 
						}                                                   
						U_Master.rx_cnt++;																	//���մ���+1
						break;                                              
			case PKT_LEN:																							//���հ������ֽ���
						U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;	//�������Buff
						if(1 == U_Master.rx_cont_len)												//�����ݽ�����ɣ����֡β
						{                                                    
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
							U_Master.MUART_RC_END_FLAG=1;							    		//�������
							RX_PACKET.PackLen = U_Master.rx_cnt+1; 						//�������ֽ���
							U_Master.rx_cnt = 0;		              
						}                                                   
						U_Master.rx_cont_len--;															//������-1
						U_Master.rx_cnt++;																	//���մ���+1										
						break;
			//Receive update cmd
			#if BOOTLOADER                                            
			case UP_HEAD2:																						//֡ͷ2                                  
						if(up_head2==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		// �������BUFF
							U_Master.PKT_RX_STATE = UP_LEN;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1
							U_Master.rx_cont_len = 10;
						}                                                   
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}						                                        
						break; 
			case UP_LEN:																							//���հ������ֽ���
						U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;			//�������Buff
						if(1 == U_Master.rx_cont_len)												//�����ݽ�����ɣ����֡β
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
							U_Master.UP_RC_END_FLAG=1;							    		//�������
							RX_PACKET.PackLen = U_Master.rx_cnt+1; 						//�������ֽ���
							U_Master.rx_cnt = 0;	               
						}                                                   
						U_Master.rx_cont_len--;															//������-1
						U_Master.rx_cnt++;																	//���մ���+1										
						break; 
	 		#endif				
			#if 0
			case UP_LEN:																							//���հ������ֽ���
					U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;			//�������Buff
					if(1 == U_Master.rx_cont_len)												//�����ݽ�����ɣ����֡β
					{                                                   
						U_Master.PKT_RX_STATE = UP_REAR1;                
					}                                                   
					U_Master.rx_cont_len--;															//������-1
					U_Master.rx_cnt++;																	//���մ���+1										
					break;  					
			case UP_REAR1:																						//֡β1           				
						if(up_rear1==rx_temp)	                                
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;		//�������BUFF
							U_Master.PKT_RX_STATE = UP_REAR2;								//״̬�л�
							U_Master.rx_cnt++;														 		//���մ���+1
						}                                                   
						else                                                
						{                                                   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
						}									                                  
						break;		                                          
			case UP_REAR2:																						//֡β2                                      
						if(up_rear2 == rx_temp)                               
						{                                                   
							U_Master.Uart_RxBuffer[U_Master.rx_cnt] = rx_temp;   
							U_Master.PKT_RX_STATE = PKT_HEAD1;                
							U_Master.UP_RC_END_FLAG=1;							    		//�������
							RX_PACKET.PackLen = U_Master.rx_cnt+1; 		//�������ֽ���
							U_Master.rx_cnt = 0;															//���մ�����0
						}
						else
						{
							U_Master.PKT_RX_STATE = PKT_HEAD1;
							U_Master.UP_RC_END_FLAG=0;
							U_Master.rx_cnt = 0;
						}	
			#endif						
			default:U_Master.PKT_RX_STATE = PKT_HEAD1;break;
		}
	}
	else if( NRF_UART0->EVENTS_ERROR )
	{
		NRF_UART0->EVENTS_ERROR = 0;
	}
}

/*�������·�
payload[1]:	7~4:����
						3~0:8-������Ϣ 10-Я��ָ����չ��Ϣ
payload[2~5]:	��Ϊ�豸IDʱ����ָ���豸���в�������
							��IDΪfffffffeʱ��������485��������������
payload[6]:����
payload[7]:��д���̺�
payload[8]~[11]:����
payload[12]:7~4�ж���
						3~0:
								0:��
								1��д
								2������
								3������
								4�������в���(ram)
								5��д���в���(ram,���綪ʧ��
								6����ram��
								7�����ò��Ա�ǩ
								8���ϱ����Խ��
								9������
								10������
								11������
	payload[13]:7~6:�ڲ�FLASHҳ��ַ
									0���ڲ�������
									1��������
									2���û���1
									3���û���2
								5: ��¼ƫ����Ч��:1��Ч
							4~0:��¼ƫ��
									ҳ0��1��16����¼0~15
									ҳ2��3��32����¼0~31
	payload[14]:����
	payload[15]:����
	payload[16]~payload[20]: ��Ƶ��ַE7E7E7E7E7
	payload[21]:����Ƶ��
	payload[22]:����Ƶ��
	payload[23]:�̺�
	payload[24]:7~6:����
							5~4:���书��
									0:(-16dbm)
									1:(-8dbm)
									2:(0dbm)
									3:(4dbm)
							3~2:�Զ��ϱ�Я������Ϣ��Դ
									0��������
									1���û���1
									2���û���2
							1������
							0������
	payload[25]:����
	payload[26]:����
	payload[27]:����
	payload[28]:
							0xfe-�������еĳ�������ǩ
							0xfd-����ƥ��ĳ�������ǩ
							0xfc-�����ܳ�������ǩ
	payload[29]~payload[31]:�󶨵ĳ�����ID����3�ֽ�
	*/
	
	
//д���ж����ֹ�Ժ�������flashоƬʱ��������Ҫ�ȴ��ܾ�
void radio_cmd_process(const uint8_t* RxPayload,uint8_t isack)
{
	uint8_t cmd;//����
	uint8_t rx_page_addr;//flashҳ��ַ
	uint8_t rx_page_offset;//ƫ����
	uint8_t rx_offset_valid;//��¼ƫ����Ч
	uint8_t set_page_offset;
//	uint8_t rx_radio_power;
//	uint8_t rx_radio_power_valid;
	uint8_t error_flag;
	uint8_t ack;
	uint8_t m,n;
	uint8_t temp_flash1[16];
	uint8_t *ptROM;//��ʱָ��rom

	uint8_t TxPaylod[32];
	copybuf(TxPaylod,RxPayload,32);
	
	//power
//	rx_radio_power_valid = (TxPaylod[17]&0xc0);
//	rx_radio_power = (TxPaylod[24]&0x30)>>4;	
	//checkָ��
	cmd=TxPaylod[12]&0x0f;	//ָ��0-read 1-write 2-erase 3-reserve 4-read run-para 
	rx_page_addr=TxPaylod[13]>>6;		//ҳ��ַ
	rx_offset_valid = (TxPaylod[13]&0x20)>>5;//��¼ƫ����Ч
	rx_page_offset = TxPaylod[13]&0x1f;	
	error_flag=0;	//0-pass, 1-error
	ack = 0;
	switch(cmd)
	{
		case 0://read

			if(rx_page_addr<2&&rx_page_offset>15) {error_flag=1;break;}	//������ȡ��Χ16ƫ����
			if(0 == rx_offset_valid)//��Ч���������²���
			{
				pROM=caucpROM[rx_page_addr];
				set_page_offset=*pROM;
				if(set_page_offset) set_page_offset--;//*ucpROM��ʾ�Ǽ�¼����������Ҫ��1
				TxPaylod[13] |= 0x20;
			}
			else
			{
				set_page_offset = rx_page_offset;
				TxPaylod[13] &= (~0x20);
			}
			ack = 1;
			break;

		case 1://write
		
			if(TxPaylod[13]&0x20) {error_flag=1;break;}	//���ò���ʱ����¼ƫ����ЧλΪ0.
			if(0 == rx_page_addr)//page0-set���ڲ�����		
			{				
				set_page_offset = ucROM0;//����ƫ����
				if(set_page_offset) set_page_offset--;
				addr = Page_Base[rx_page_addr];
				m = set_page_offset*Rom_record_byte[rx_page_addr];
				nrf_nvmc_read_bytes(addr+m,temp_flash1,Rom_record_byte[rx_page_addr]);
				//1-10��ָ���¼������0ָ��Ԥ�����
/*
[0]	.7~6 	0		Ӳ������
		.5~0	0~15	�汾��
[1]	.7~4	6		���书�ʣ�0- (-30dBm), 1- (-20dBm), 2- (-16dBm), 3- (-12dBm)
													4- (8dBm), 5- (4dBm), 6- (0dBm), 7- (4dBm)
		.3~2	0~2		�Զ��ϱ�Я������Ϣ��Դ��0-��������1-�û���1, 2-�û���2
		.1~0	0		ʹ�����ڷ��� 1-����
[2]	.7~0	0		����
[3]	.7~0	0		����
[4]	.7		0		����		
		.6		0/1		˥��������0-�շ��Գ�˥����1-����˥��
		.5		0/1		����RPD���ˣ�1-����
		.4~0	0~31	˥��ֵ��0~31-��С0dB���31dB
[5~8]				�󶨵ĳ�����̽���ǩID��MSB���ȣ�
[5]	.7~0	0xfe~0xfc
							0xFEXXXXXX-�������еĳ�������ǩ������󶨣�
							0xFDXXXXXX-����ƥ��ĳ�������ǩ���󶨣�
							0xFCXXXXXX-�����ճ�������ǩ				
*/
				ptROM=ucROM0?temp_flash1:CurParaSet;
		
				//������������Ĳ�����Ч���������payload�����һظ����ϴ����¼�¼�Ĳ���
				//���书�� 7~6������00����Ч 5~4����11��Ч                                                               
				if(TxPaylod[24]&0x82)//��8/2bit��Ϊ1.
				{
					TxPaylod[24] = ptROM[8];
				}
				//�Զ�Я����Ϣ��Դ
				TxPaylod[25] = ptROM[9];	
				TxPaylod[26] = ptROM[10];
					//rssi
				if(TxPaylod[27]&0x80)
				{
					TxPaylod[27]=ptROM[11];
				}	
				if(TxPaylod[28]==0xff||TxPaylod[28]<0xfc)//
				{
					TxPaylod[28]=CurParaSet[12];
					TxPaylod[29]=CurParaSet[13];
					TxPaylod[30]=CurParaSet[14];
					TxPaylod[31]=CurParaSet[15];
				}
				else
				{
					if(TxPaylod[28]!=0xfd)
					{
						TxPaylod[29]=CurParaSet[13];
						TxPaylod[30]=CurParaSet[14];
						TxPaylod[31]=CurParaSet[15];
					}
				}
				//read-only
				TxPaylod[16]=ParaSet[0];
				TxPaylod[17]=ParaSet[1];
				TxPaylod[18]=ParaSet[2];
				TxPaylod[19]=ParaSet[3];
				TxPaylod[20]=ParaSet[4];
				TxPaylod[21]=ParaSet[5];
				TxPaylod[22]=ParaSet[6];
				TxPaylod[23]=ParaSet[7];	
				//�Ƚ�д�������������¼�����ظ�д��
				for(m=0;m<Rom_record_byte[rx_page_addr];m++)
				{
					if(ptROM[m]!=TxPaylod[16+m]) break;
				}	
				if(m<Rom_record_byte[rx_page_addr])//change
				{
					if(ucROM0>=Rom_record_size[rx_page_addr]) //full? erase
					{
						ucROM0=0;	
						nrf_nvmc_page_erase(addr);	
					}
					
					nrf_nvmc_write_bytes(addr+ucROM0*Rom0_record_byte,&TxPaylod[16],16);	
					//�����в���������acuSet�У��������ò���
					//������Ч,[0]~[6] read-only
					for(m=7;m<16;m++) CurParaSet[m]=TxPaylod[m+16];//update parameter
					ucROM0++;
					set_page_offset = ucROM0-1;
					my_tx_power = (uint32_t)caucTXPOWER[CurParaSet[8]>>4];//transmit power
					En_Period_Tx = CurParaSet[8]&0x01;
					TxPaylod[13] |= 0x20;
				}
				else
				{
					TxPaylod[13] &= (~0x20);
				}				
			}
			else if(1 == rx_page_addr || 2==rx_page_addr || 3 == rx_page_addr)//reserve page,user page1,user page2
			{					
				//��д�ռ�¼
				for(m=16;m<32;m++)
				{
					if(TxPaylod[m]!=0xff) break;	
				}
				if(m>=32) 
				{
					error_flag=1;
					break;
				}
				//�Ƚ�д�������������¼�����ظ�д��
				pROM = caucpROM[rx_page_addr];//��¼����
				set_page_offset = *pROM;
				if(set_page_offset>0) set_page_offset--;
				addr = Page_Base[rx_page_addr]; //BASE ADDR
				m = set_page_offset*Rom_record_byte[rx_page_addr];//offset addr
				nrf_nvmc_read_bytes(addr+m,temp_flash1,Rom_record_byte[rx_page_addr]);
				for(m=0;m<Rom_record_byte[rx_page_addr];m++)//���ظ�д��
				{
					if(temp_flash1[m]!=TxPaylod[16+m])break;//different parameter
				}
				if(m<Rom_record_byte[rx_page_addr])//write new parameter
				{
					if(*pROM>=Rom_record_size[rx_page_addr])
					{
						#if 0//not erase ,ack new parameter
						addr = Page_Base[rx_page_addr]; //BASE ADDR
						nrf_nvmc_page_erase(addr);
						(*pROM) = 0;
						#endif
						TxPaylod[13] &= (~0x20);
					}
					else
					{
						m = (*pROM) * Rom_record_byte[rx_page_addr];//offset
						nrf_nvmc_write_bytes(addr+m,&TxPaylod[16],Rom_record_byte[rx_page_addr]);
						
						//check read back
						nrf_nvmc_read_bytes(addr+m,temp_flash1,Rom_record_byte[rx_page_addr]);
						for(m=0;m<Rom_record_byte[rx_page_addr];m++)
						{
							if(temp_flash1[m]!=0xff)break;//different parameter
						}
						if(m<Rom_record_byte[rx_page_addr])
						{
							(*pROM)++;
							set_page_offset = (*pROM)-1;
						}
						TxPaylod[13] |= 0x20;
					}
				}
				else
				{
					TxPaylod[13] &= (~0x20);
				}
			}	
			ack = 1;
			break;
		
		case 2://erase
		
			pROM = caucpROM[rx_page_addr];//��¼����
			addr = Page_Base[rx_page_addr]; //BASE ADDR
			nrf_nvmc_page_erase(addr);
			(*pROM) = 0;
			ack =2;
					
		case 3:break;
		case 4://read ram
		
			for(m=0;m<16;m++) TxPaylod[m+16]=CurParaSet[m];
			ack = 4;
			break;
		
		case 5://write ram
		
			//��д�ռ�¼
			for(m=16;m<32;m++)
			{
				if(TxPaylod[m]!=0xff) break;	
			}
			if(m>=32) error_flag=1;
			else
			{
				//check para
				if(TxPaylod[24]&0x02)//��2bit��Ϊ1.
				{
					error_flag=1;
				}
				if(TxPaylod[28]==0xff||TxPaylod[28]<0xfc)
				{
					error_flag=1;
				}
			}
			ack =5;
			break;
			
		default:break;
	}	
		
	
	if(isack)
	{
		if(!error_flag)//ack
		{
			if(1==ack)//��д�ظ�
			{
				#if 1
				m = TxPaylod[13]&0xe0;//7~6λҳ��ָʾ	
//				m |=0x20;//ƫ����Ч
				TxPaylod[13] = m | set_page_offset;
				addr = Page_Base[rx_page_addr];
				m = set_page_offset *Rom_record_byte[rx_page_addr];
				nrf_nvmc_read_bytes(addr+m,&TxPaylod[16],Rom_record_byte[rx_page_addr]);
				#endif
				if(0 == rx_page_addr)
				{
					TxPaylod[23]=(HWTYP<<6)|(VERSION&0x3f);
				}
			}
			else if(2==ack)//����
			{
				addr = Page_Base[rx_page_addr];
				nrf_nvmc_read_bytes(addr,&TxPaylod[16],Rom_record_byte[rx_page_addr]);
				TxPaylod[13] = TxPaylod[13]&0xc0;
			}
			else if(4 == ack)//read ram
			{
				TxPaylod[23]=(HWTYP<<6)|(VERSION&0x3f);//����Ӳ�����ͺ�����汾: ԭ�̺�λ��
			}
			else if(5 == ack)//write ram
			{
				for(m=7;m<16;m++) CurParaSet[m] = TxPaylod[16+m];
				my_tx_power = (uint32_t)caucTXPOWER[CurParaSet[8]>>4];//transmit power
				En_Period_Tx = CurParaSet[8]&0x01;
				
			}
			TxPaylod[1] = 0x02;//����
			TxPaylod[2]=caucID[0];
			TxPaylod[3]=caucID[1];
			TxPaylod[4]=caucID[2];
			TxPaylod[5]=caucID[3];
			TxPaylod[6]=0;
			TxPaylod[7]=CurParaSet[7];
			TxPaylod[8]=0;
			TxPaylod[9]=0;
			TxPaylod[10]=0;
			TxPaylod[11]=0;
			m = 0;
			m = (bRFRxed<<7)|(bRFRPD<<6);
			TxPaylod[12] = TxPaylod[12]| m;
			//XOR
			for(m=31,n=0;m;m--) n^=TxPaylod[m];
			TxPaylod[m]=n;
			
			RFN_TX_MODE;//RW=0-����  RW=1-����	
			copybuf(payload,TxPaylod,32);
			radio_tx_carrier(my_tx_power,RADIO_MODE_MODE_Nrf_1Mbit, config_channel);
			config_send_state = 2;//ack
			radio_channel = run_config_channel;
		}
	}
}

/*�������·�
payload[1]:	7~4:����
						3~0:8-������Ϣ 10-Я��ָ����չ��Ϣ
payload[2~5]:	��Ϊ�豸IDʱ����ָ���豸���в�������
							��IDΪfffffffeʱ��������485��������������
payload[6]:����
payload[7]:��д���̺�
payload[8]~[11]:����
payload[12]:7~4�ж���
						3~0:
								0:��
								1��д
								2������
								3������
								4�������в���(ram)
								5��д���в���(ram,���綪ʧ��
								6����ram��
								7�����ò��Ա�ǩ
								8���ϱ����Խ��
								9������
								10������
								11������
	payload[13]:7~6:�ڲ�FLASHҳ��ַ
									0���ڲ�������
									1��������
									2���û���1
									3���û���2
								5: ��¼ƫ����Ч��:1��Ч
							4~0:��¼ƫ��
									ҳ0��1��16����¼0~15
									ҳ2��3��32����¼0~31
	payload[14]:����
	payload[15]:����
	payload[16]~payload[20]: ��Ƶ��ַE7E7E7E7E7
	payload[21]:����Ƶ��
	payload[22]:����Ƶ��
	payload[23]:�̺�
	payload[24]:7~6:����
							5~4:���书��
									0:(-16dbm)
									1:(-8dbm)
									2:(0dbm)
									3:(4dbm)
							3~2:�Զ��ϱ�Я������Ϣ��Դ
									0��������
									1���û���1
									2���û���2
							1������
							0������
	payload[25]:����
	payload[26]:����
	payload[27]:����
	payload[28]:
							0xfe-�������еĳ�������ǩ
							0xfd-����ƥ��ĳ�������ǩ
							0xfc-�����ܳ�������ǩ
	payload[29]~payload[31]:�󶨵ĳ�����ID����3�ֽ�
*/

void RADIO_IRQHandler()
{
	uint8_t radio_uc1,radio_uc2;
	uint8_t rfid_uc1,rfid_uc2,rfid_uc3;
	uint8_t b1,b2,bAck,ii,jj;
	static uint8_t count;
  
	if(1 == NRF_RADIO->EVENTS_END)
	{
		NRF_RADIO->EVENTS_END = 0;
		if(radio_status == RADIO_STATUS_TX)
		{
			if(work_mode == config_Tag)//��չ��Ϣ�·���ɣ�ת����Ƶ������
			{
				if(1 == config.send_flag)
				{
					radio_channel = run_config_channel;
					radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,config_channel);
				}
			}
			else if(advertise == work_mode)
			{
				NRF_RADIO->TASKS_TXEN = 1;
			}
			else if(0 == work_mode)
			{
				if(1==config_send_state)
				{
					RFN_RX_MODE;//RW=0-����  RW=1-����	
					radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit, config_channel);//switch to receive 
					radio_channel = run_config_channel;
					rf_cfg_state = config_start;//��ʼ�������ò���
					count = 0;
				}
				else if(2 == config_send_state)
				{
					if(count++<=3)
					{
						RFN_TX_MODE;//RW=0-����  RW=1-����	
						radio_tx_carrier(RADIO_TXPOWER_TXPOWER_Pos4dBm,RADIO_MODE_MODE_Nrf_1Mbit, config_channel);
						radio_channel = run_config_channel;					
					}
					else
					{
						rf_cfg_state = config_success;
					}
				}
				else
				{
					RFN_RX_MODE;//RW=0-����  RW=1-����		
					radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,data_channel);
					radio_channel = run_data_channel;
				}
			}
		}
		else if(RADIO_STATUS_RX == radio_status)
		{
			if(NRF_RADIO->CRCSTATUS)
			{
				//XOR���Ϊ0 ��ʾ������Ч
				for(radio_uc1=0,radio_uc2=0;radio_uc2<32;radio_uc2++)
				{
					radio_uc1^=payload[radio_uc2];
				}
				if(!radio_uc1)
				{
					//���忴Э���ĵ�-���˵��
					if(run_config_channel == radio_channel)
					{
						if(work_mode == config_Tag)//����������Ϣ����
						{
							if(payload[2] == config.ID[0]&&payload[3] == config.ID[1]
								&&payload[4] == config.ID[2]&&payload[5] == config.ID[3])
							{
								if(1 == config.send_flag)
								{
									if(config.rx_time<1)
									{
										config.state = 1;//���óɹ�����Ϣ����
										copybuf(config.payload,payload,32);
									}
									config.rx_time++;
								}
							}
						}
						if((payload[1]&0x0a)==0x0a)//���У�Я����չָ��
						{
							//b1:1 receive config info 
							//b2:1:allow ack 0:forbid ack
							b1=0;b2=0;
							if(payload[2]==caucID[0]
							&&payload[3]==caucID[1]
							&&payload[4]==caucID[2]
							&&payload[5]==caucID[3])//id match
							{
								b1=1;
								b2=1;
							}
							else if(payload[2]==0xff
							&&payload[3]==0xff
							&&payload[4]==0xff
							&&payload[5]>=0xfe)//����ǹ㲥��Ϣ��һ�ɲ��ظ���
							{
								b1=1;
//								if(payload[5]!=0xff) b2=1;
							}
							if(b1)
							{
								bAck=b2;
								bRFRxed=1;//�յ�������ָʾ
								if(NRF_RADIO->RSSISAMPLE<64)
								{
									bRFRPD=1;//�ź�ǿ����Ч
								}
								#if 0
								if(!bAck)//50ms timeout,back to rx data channel
								{
									//��������Ƶ������
									radio_rx_carrier(RADIO_MODE_MODE_Nrf_1Mbit,data_channel);
									RFN_RX_MODE;//RW=0-����  RW=1-����
									radio_channel = run_data_channel;
								}
								#endif
								radio_cmd_process(&payload[0],bAck);
												
							}
						}
					}
					else if(run_data_channel == radio_channel)
					{
						if(work_mode == list_Tag&&payload[2]!=0xff&&payload[2]!=0xfe)
						{
								for(rfid_uc1=0,rfid_uc2=CAPACITY;rfid_uc1<CAPACITY;rfid_uc1++)	
								{
									if(astRFID[rfid_uc1].aucID[0]==0xff)//��ǩID��һ����������0XFF
									{
										if(rfid_uc2>=rfid_uc1) rfid_uc2=rfid_uc1; //��һȦ���ҵ��ռ�¼�����Ҳ�����ͬ�ı�ǩID�ţ�rfid_uc1 = 135
									}
									if(astRFID[rfid_uc1].aucID[0]==payload[2]
									&&astRFID[rfid_uc1].aucID[1]==payload[3]
									&&astRFID[rfid_uc1].aucID[2]==payload[4]
									&&astRFID[rfid_uc1].aucID[3]==payload[5])
									{
										rfid_uc2=rfid_uc1;
										break;
									}
								}
							
								if(rfid_uc2<CAPACITY)	//������Χ����
								{												 																				 
									if(rfid_uc1<CAPACITY)//update
									{
										astRFID[rfid_uc2].aucID[0]=payload[2];
										astRFID[rfid_uc2].aucID[1]=payload[3];
										astRFID[rfid_uc2].aucID[2]=payload[4];
										astRFID[rfid_uc2].aucID[3]=payload[5];
										rfid_uc3=payload[8]&0x7f;
										if(payload[6]&0x01) rfid_uc3|=0x80;
										astRFID[rfid_uc2].Sta[0]=rfid_uc3;
										astRFID[rfid_uc2].Sta[1]=payload[9];
										for(rfid_uc3=0;rfid_uc3<10;rfid_uc3++)
										{
											astRFID[rfid_uc2].info[rfid_uc3]=payload[16+rfid_uc3];
										}
//											astRFID[rfid_uc2].RSSI= NRF_RADIO->RSSISAMPLE;
										astRFID[rfid_uc2].Age++;
									}
									else //��¼�±�ǩ
									{
										astRFID[rfid_uc2].aucID[0]=payload[2];
										astRFID[rfid_uc2].aucID[1]=payload[3];
										astRFID[rfid_uc2].aucID[2]=payload[4];
										astRFID[rfid_uc2].aucID[3]=payload[5];
										rfid_uc3=payload[8]&0x7f;
										if(payload[6]&0x01) rfid_uc3|=0x80;
										astRFID[rfid_uc2].Sta[0]=rfid_uc3;
										astRFID[rfid_uc2].Sta[1]=payload[9];
										for(rfid_uc3=0;rfid_uc3<10;rfid_uc3++)
										{
											astRFID[rfid_uc2].info[rfid_uc3]=payload[16+rfid_uc3];
										}
//											astRFID[rfid_uc2].RSSI= NRF_RADIO->RSSISAMPLE;
										astRFID[rfid_uc2].Age++;
									}	
								}		
						}
						else if(work_mode == config_Tag)//
						{
							if(payload[2] == config.ID[0]&&payload[3] == config.ID[1]
								&&payload[4] == config.ID[2]&&payload[5] == config.ID[3])
							{
								if(0x80 == (payload[6]&0xff))//Я�����մ���?
								{
									//ת������ָ��
									payload[1]=0x0a;
									payload[2]=config.ID[0];
									payload[3]=config.ID[1];
									payload[4]=config.ID[2];
									payload[5]=config.ID[3];
									payload[6]=0;
									payload[7]=CurParaSet[7];
									payload[8]=0;
									payload[9]=0;
									payload[10]=0;
									payload[11]=0;
									for(ii=12,jj=19;ii<32;ii++,jj++) payload[ii]=U_Master.Uart_RxBuffer[jj];//��չ��Ϣ20�ֽ�
				
									for(ii=0,jj=1;jj<32;jj++) ii^=payload[jj];
									payload[0]=ii;
									radio_tx_carrier(RADIO_TXPOWER_TXPOWER_Pos4dBm,RADIO_MODE_MODE_Nrf_1Mbit, config_channel);
									radio_channel = run_config_channel;
									config.send_flag = 1;
								}
							}
						}
					}
				}
			}
		}
		if(RADIO_STATUS_RX == radio_status)
		{	
			NRF_RADIO->TASKS_START = 1U;
		}
	}
}
						

void TIMER0_IRQHandler()
{
	if(NRF_TIMER0->EVENTS_COMPARE[0])
	{
		//RFID 2S��ʱ
		ucCycRFID++;
		if(ucCycRFID>=RFID_CYCLE)
		{
			ucCycRFID = 0;
			RFID_TX_EN = 1;
		}
		if(timeout_cnt_start)
		{
			timeout_cnt++;
		}
		if(time_en)
		{
			time_cnt++;
		}
		//rfid config recive time-out
		NRF_TIMER0->EVENTS_COMPARE[0] = 0;
	}
}


