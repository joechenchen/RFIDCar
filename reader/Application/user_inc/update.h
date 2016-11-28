//������ض���
//�汾V0.1.1
#include "stdint.h"
#include "nrf.h"
//�����������
#define 	BOOT_BEGIN_EXE	0x4UL
#define   APP_BEGIN 0x1A000UL				//140K��С,������
#define 	ID_BEGIN	0x3D000					//1KB,Ӳ����Ϣ��,ǰ4�ֽڷ�ID��
#define 	PARA_BEGIN 0x3D400UL			//11KB,������
#define 	VER_LEN		20
#ifndef __UPDATEDATA
	#define __UPDATEDATA
			//uint8_t		AppFlag 						__attribute__((at(0x20000200)));												//ָʾ��ǰCODE��Ӧ�ó�����BootLoader
#define AppFlag (NRF_POWER->GPREGRET)
			uint8_t		ToggleSerial				__attribute__((at(0x20000201)));												//���ݸ�Bootloader��ָ����ˮ��
const uint8_t 	AppVer[VER_LEN] 		__attribute__((at(APP_BEGIN + 200))) = "NTN207CV0.0.3";//ֻ���������ܽ������汾������ȼ����
const uint8_t 	APPVer_MB[VER_LEN] 	__attribute__((at(APP_BEGIN + 220))) = "APPMBV0.0.1";

#define __ON_BOOT												0
#define __ON_APP												0xa5
#define __CMD1													0x5a
#define __CMD2													0xb5

#define		JumpToBoot()  (*(void (*)(void))(*(uint32_t*)BOOT_BEGIN_EXE))()
#define		GotoBootLoader(x)	{\
	uint8_t i;\
	ToggleSerial = x;\
	for(i = POWER_CLOCK_IRQn;i <= SWI5_IRQn;i++)\
		NVIC_DisableIRQ( i );\
	AppFlag = __CMD1 ;\
	JumpToBoot();\
	while(1);\
	}
#endif
/*Ӧ�ó�����֧��
1�����þ���֮ǰ������ָ��AppFlag = __ON_APP;
2��������֧����������ָ��

����1��
	��PC�·���aa aa xx xx xx xx ss 0d 00 00 5a 5a
	����Ӧ��aa aa xx xx xx xx ss 8c ll ll 00... 5a 5a
	˵����xxλ�÷�ID��
			  ssλ�÷���ˮ��
				llλ����Ӧ���ݵĳ���
				...��Ӧ�ó���İ汾�ţ���\0����
����2��
	��PC�·���aa aa xx xx xx xx ss 09 00 00 5a 5a
	���������ȹرտ��Ź����ٵ���GotoBootLoader(x);����xֵΪPC�·�ָ���е�ָ����ˮss��
3������ʱ��keil������Target�е�IROM1����start:0x1A000��size:0x24800
*/
//FILE_END
