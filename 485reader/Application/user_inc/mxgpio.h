#ifndef MX_GPIO_
#define MX_GPIO_
#include "nrf_gpio.h"
#define pingguo
//���Ŷ�������Ŷ���.txt�ĵ�
#ifdef ZY_ZHUJI
//	/*-------------OLEDģ�����Ŷ���-------------------*/
//	//#define oled_sclk_pin_num 17  //OLEDʱ�� out
//	//#define oled_data_pin_num 16  //OLED�������� out 
//	//#define oled_c_d_pin_num 15   //OLED����/���� out
//	#define oled_cs_pin_num 14    //CS��Ƭѡ�ź� out

//	#define oled_zk_cs_pin_num 12 //�ֿ�FLASHƬѡ�ź�  out
//	#define oled_zk_fso_pin_num 13 //�ֿ�FLASH������� in
//	#define oled_power_num 18
//	/*-------------M25P16 2M FLASH���Ŷ���-----------*/
//	//#define m25pxx_clk_pin_num 17    //ʱ���ź� out
//	//#define m25pxx_datai_pin_num 16  //FLASH�������� out
//	//#define m25pxx_datao_pin_num 15  //FLASH������� in
//	#define m25pxx_cs_pin_num 0      //FLASHƬѡ�ź� out

//	/*-------------M25P16��OLED�������Ŷ���----------*/
//	#define OFlash_sclk_pin_num 17  //ʱ�� out
//	#define OFlash_datai_pin_num 16  //������������ out 
//	#define OFlash_cd_data_pin_num 15  //����������� ,���������֮���л�����OLED���������ó��������FLASH����ʱ���ó�����

//	/*-------------��ť�ӿ�-------------------------*/
//	#define KeyLED5V_pin_num 10
//	#define KEY_pin_num 11
//	/*------------���ذ�--------------------*/

//	#define LED0V_pin_num 7

//	/*------------������Դ��������---------------*/
//	#define GPS_power_pin_num 30      //out
//	#define GPS_txd_pin_num 3         //in
//	#define GPS_DATA 3
//	/*------------WIFI��Դ����---------------*/
//	#define Wifi_power_pin_num 2
//	/*------------4G��Դ����---------------*/
//	#define P_4G_pin_num 29
//	/*------------����ͷ����---------------*/
//	#define Camera_pin_mun 6

//	/*------------�������Ŷ���-------------*/
//	#define test25_pin_num 25
//	#define test1_pin_num 1

//	/*------------2.4g���Ű�-------------------------*/
//	//RW=0-����  RW=1-����		 
//	#define RFN_RW_pin_num 19
//	#define RFN_TX_MODE 	nrf_gpio_pin_set(RFN_RW_pin_num)	
//	#define RFN_RX_MODE 	nrf_gpio_pin_clear(RFN_RW_pin_num)	   //OUT
//	#define RFN_EN_pin_num 20
//	/*----------------����--------------------------*/
//	#define RX_PIN_NUMBER  8    // UART RX pin number.
//	#define TX_PIN_NUMBER 9    // UART TX pin number.
//	#define CTS_PIN_NUMBER 1   // UART Clear To Send pin number. Not used if HWFC is set to false
//	#define RTS_PIN_NUMBER 25    // Not used if HWFC is set to false 
//	#define HWFC           false // UART hardware flow control
#endif
	
#ifdef BIAN_JIE
//		/*-------------OLEDģ�����Ŷ���-------------------*/
//	#define oled_sclk_pin_num 1  //OLEDʱ�� out
//	#define oled_sdi_pin_num 0  //OLED�������� out 
//	#define oled_c_d_pin_num 30   //OLED����/���� out
//	#define oled_cs_pin_num 29    //CS��Ƭѡ�ź� out

//	#define oled_zk_cs_pin_num 25 //�ֿ�FLASHƬѡ�ź�  out
//	#define oled_zk_fso_pin_num 28 //�ֿ�FLASH������� in
//	#define oled_power_num 02
//	/*-----------MOSFET H-BRIDGE----------------*/
//	//P0.03 P0.04�ߣ�p0.18,P0.19�ͣ�����2->1��ͨ��P0.03 P0.04��p0.18,P0.19�ߣ�����1->2��ͨ
//	#define TC4426_PMOS1_PIN_NUM 19
//	#define TC4426_NMOS1_PIN_NUM 18
//	#define TC4426_PMOS1_ON NRF_GPIO->OUTSET = (1UL << TC4426_PMOS1_PIN_NUM)     //1
//	#define TC4426_PMOS1_OFF NRF_GPIO->OUTCLR = (1UL << TC4426_PMOS1_PIN_NUM) //0
//	#define TC4426_NMOS1_ON NRF_GPIO->OUTCLR = (1UL << TC4426_NMOS1_PIN_NUM)   //0
//	#define TC4426_NMOS1_OFF NRF_GPIO->OUTSET = (1UL << TC4426_NMOS1_PIN_NUM)   //1
//	
//	
//	//	  NRF_GPIO->OUTCLR = (1UL << 19);//-----------nrf_gpio_pin_clear
////	  NRF_GPIO->OUTSET = (1UL << 4);//-----------nrf_gpio_pin_set
////	
////	  NRF_GPIO->OUTCLR = (1UL << 18);//-----------nrf_gpio_pin_clear
////	  NRF_GPIO->OUTSET = (1UL << 3);//-----------nrf_gpio_pin_set
//  //�Զ�������ͨ������2�ŵ�1�ŵ�ͨ
//	#define TC4426_PMOS2_PIN_NUM 3
//	#define TC4426_NMOS2_PIN_NUM 4
//	#define TC4426_PMOS2_ON NRF_GPIO->OUTSET = (1UL << TC4426_PMOS2_PIN_NUM)  //TC4426��->PMOS�� ��ͨ
//	#define TC4426_PMOS2_OFF NRF_GPIO->OUTCLR = (1UL << TC4426_PMOS2_PIN_NUM) 
//	#define TC4426_NMOS2_ON NRF_GPIO->OUTCLR = (1UL << TC4426_NMOS2_PIN_NUM) 
//	#define TC4426_NMOS2_OFF NRF_GPIO->OUTSET = (1UL << TC4426_NMOS2_PIN_NUM)

//	#define PMOS2NMOS1ON()   {TC4426_NMOS2_OFF;TC4426_PMOS1_OFF;TC4426_PMOS2_ON;TC4426_NMOS1_ON;}//PMOS2NMOS1ON
//	#define PMOS1NMOS2ON()   {TC4426_PMOS2_OFF;TC4426_NMOS1_OFF;TC4426_NMOS2_ON;TC4426_PMOS1_ON;}//PMOS1NMOS2ON
//  #define ALLOFF() {TC4426_PMOS1_OFF;TC4426_PMOS2_OFF;TC4426_NMOS1_ON;TC4426_NMOS2_ON;}//ALLOFF
//	/*-----------��λ�������Ŷ��壬�͵�ƽ��Ч��ͬʱֻ����һ����ͨ----------------*/	
//	#define KEY1 nrf_gpio_pin_read(21)
//	#define KEY2 nrf_gpio_pin_read(22)
//	#define KEY3 nrf_gpio_pin_read(23)
//	#define KEY4 nrf_gpio_pin_read(24)
//	#define KEY1_PIN_NUM 21
//	#define KEY2_PIN_NUM 22
//	#define KEY3_PIN_NUM 23
//	#define KEY4_PIN_NUM 24
//	/*----------LED��----------------------------------*/
//	#define LED_PIN_NUM 17
#endif

//	/*-------------��ť�ӿ�-------------------------*/
//	#define KeyLED5V_pin_num 10
//	#define KEY_pin_num 11
//	/*------------���ذ�--------------------*/
//	#define LED0V_pin_num 7
//	/*------------�������Ŷ���-------------*/
//	#define test25_pin_num 25
//	#define test1_pin_num 1

	/*----------------����--------------------------*/
//	#define RX_PIN_NUMBER  8    // UART RX pin number.
//	#define TX_PIN_NUMBER 9    // UART TX pin number.
//	#define CTS_PIN_NUMBER 1   // UART Clear To Send pin number. Not used if HWFC is set to false
//	#define RTS_PIN_NUMBER 25    // Not used if HWFC is set to false 
//	#define HWFC           false // UART hardware flow control
#ifdef pingguo
	/*----------------����--------------------------*/
	#define RX_PIN_NUMBER  6    // UART RX pin number.
	#define TX_PIN_NUMBER 5   // UART TX pin number.
	#define CTS_PIN_NUMBER 1   // UART Clear To Send pin number. Not used if HWFC is set to false
	#define RTS_PIN_NUMBER 25    // Not used if HWFC is set to false 
	#define HWFC           false // UART hardware flow control

	
	
	/*------------2.4g���Ű�-------------------------*/
	//RW=0-����  RW=1-����		 
	#define RFN_RW_pin_num 19
	#define RFN_TX_MODE 	nrf_gpio_pin_set(RFN_RW_pin_num)	
	#define RFN_RX_MODE 	nrf_gpio_pin_clear(RFN_RW_pin_num)	   //OUT
	#define RFN_EN_pin_num 20
#endif

#endif

