#ifndef __DEVICE_TYPE
#define __DEVICE_TYPE		   
#include "sys.h"
#define OPEN 0X01
#define CLOSE 0X00


/*----------------���ڶ���------------------*/
#define pkt_head1 0xFF
#define pkt_head2 0xFF
#define pkt_head3 0xFF
#define pkt_head4 0xFF
#define pkt_head5 0xFF
#define pkt_head6 0x00

#define pkt_head_byte 6
//#define pkt_id_byte 2   
#define pkt_cont_byte 2 
//#define id_array_pos (pkt_head_byte + pkt_id_byte - 1
#define cont_array_pos (pkt_head_byte + pkt_cont_byte - 1) 

typedef enum
{
	PKT_HEAD1=0,
	PKT_HEAD2,
	PKT_HEAD3,
	PKT_HEAD4,
	PKT_HEAD5,
	PKT_LEN_ID,
	PKT_LEN,
	PkT_XOR,
	PKT_REAR1,
	PKT_REAR2,
	PKT_REAR3,
	PKT_REAR4
}PKT_STATE;

//�����Ͷ���
typedef struct
{
	u16 PackLen;
	u8 type;
	u8 XOR;
}PACKET_TypeDef;

typedef struct
{
	u16 rx_cnt;									//���ڽ������ݸ���
	u16 rx_cont_len;						//���ڽ����������ݳ���
	u8 PKT_RX_STATE;						//���ڽ���״̬
	u8 MUART_RC_END_FLAG;				//���ڽ�����ɱ�־λ
	u8 Uart_RxBuffer[200];			//���ڽ������ݻ�����
	u8 Temp_Buf[1];							//���ڽ��յ�����������
}UartData_TypeDef;
//#define COM_REQUEST_ACK 0X87
//#define COM_SIGNIN_ACK 0X80
//#define COM_CONFIRM_ACK 0X82
//#define COM_HB_ACK 0X83
//#define COM_CHANGE_ACK 0X84
//#define COM_SEARCH_ACK 0X85

/*----------------���ڶ���------------------*/
//#define pkt_head1 0xAA
//#define pkt_head2 0xAA
//#define pkt_head3 0x00
//#define pkt_head4 0x01
//#define pkt_head 0xAA
// 	 
//#define pkt_rear1 0x5A
//#define pkt_rear2 0x5A
//#define pkt_rear3 0xBB
//#define pkt_rear4 0xBB
//#define pkt_rear 0xBB
//#define pkt_head_byte 4
//#define pkt_id_byte 2   
//#define pkt_cont_byte 2 
//#define id_array_pos (pkt_head_byte + pkt_id_byte - 1
//#define cont_array_pos (pkt_head_byte + pkt_id_byte + pkt_cont_byte - 1) 

//#define COM_REQUEST_ACK 0X87
//#define COM_SIGNIN_ACK 0X80
//#define COM_CONFIRM_ACK 0X82
//#define COM_HB_ACK 0X83
//#define COM_CHANGE_ACK 0X84
//#define COM_SEARCH_ACK 0X85

//void COM_REQUEST_TO_SERVER(u8* Device_ID,u8 Seq_No,u8 *temp,u8* GPS_Temp);
//void COM_SIGNIN_TO_SERVER(u8* Device_ID,u8 Seq_No,u8 Device_Type,u8 *temp,u8* GPS_Temp);
//void COM_CONFIRM_TO_SERVER(u8* Device_ID,u8 Seq_No,u8 *temp,u8* GPS_Temp);
//void COM_HB_TO_SERVER(u8* Device_ID,u8 Seq_No,u8 Device_Type,u8 *temp,u8* GPS_Temp);
//void COM_CHANGE_TO_SERVER(u8* Device_ID,u8 Seq_No,u8* sign_seq1,u8* p_read,u16 data_len,u16 pack_len,u8 *temp,u8* GPS_Temp,u8 fenbao);
//void COM_SEARCH_TO_SERVER(u8* Device_ID,u8 Seq_No,u8* sign_seq1,u8* p_read,u16 data_len,u16 pack_len,u8 *temp,u8* GPS_Temp,u8 fenbao);


#endif

















