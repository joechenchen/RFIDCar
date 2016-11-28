//升级相关定义
//版本V0.1.1
#include "stdint.h"
#include "nrf.h"
//升级相关配置
#define 	BOOT_BEGIN_EXE	0x4UL
#define   APP_BEGIN 0x1A000UL				//140K大小,代码区
#define 	ID_BEGIN	0x3D000					//1KB,硬件信息区,前4字节放ID号
#define 	PARA_BEGIN 0x3D400UL			//11KB,参数区
#define 	VER_LEN		20
#ifndef __UPDATEDATA
	#define __UPDATEDATA
			//uint8_t		AppFlag 						__attribute__((at(0x20000200)));												//指示当前CODE是应用程序还是BootLoader
#define AppFlag (NRF_POWER->GPREGRET)
			uint8_t		ToggleSerial				__attribute__((at(0x20000201)));												//传递给Bootloader的指令流水号
const uint8_t 	AppVer[VER_LEN] 		__attribute__((at(APP_BEGIN + 200))) = "NTN207CV0.0.3";//只能升级不能降级，版本左边优先级最高
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
/*应用程序需支持
1、配置晶振之前，加入指令AppFlag = __ON_APP;
2、程序需支持启用升级指令

步骤1：
	【PC下发】aa aa xx xx xx xx ss 0d 00 00 5a 5a
	【回应】aa aa xx xx xx xx ss 8c ll ll 00... 5a 5a
	说明：xx位置放ID号
			  ss位置放流水号
				ll位置响应数据的长度
				...放应用程序的版本号，以\0结束
步骤2：
	【PC下发】aa aa xx xx xx xx ss 09 00 00 5a 5a
	【操作】先关闭看门狗，再调用GotoBootLoader(x);输入x值为PC下发指令中的指令流水ss。
3、编译时，keil配置中Target中的IROM1配置start:0x1A000，size:0x24800
*/
//FILE_END
