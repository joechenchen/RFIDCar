#include "oled_show.h"
#include "hal_spi.h"
#include "oled_drv.h"
//CC�����ֿ�ģʽ��һ��OLED��ʾָ����Ҫ10ms�����Բ��ܽ�OLED��ʾ�����жϺ�����޸�OLED��ʾ��ʽ��������ʾʱ��
//�Զ����ֿ⣬���Ҳ���������Ļ���µ��������Ҫ80ms��ʱ�䣬������״̬���������£�������OLED���������Ըĳɾֲ����¡�
#define DIANCHI_MODE 1
uint8_t old_caucAddr2[4];
uint8_t old_aucRTC[5] ={0xaa,0xaa,0xaa};
uint8_t old_work_mode=0xff;
uint8_t old_gps_flag = 0xff;
uint8_t old_adc_flag = 0xff;
uint8_t old_adc_dd_flag = 0xff;
uint8_t dd_flag=0xff;  //�͵��־λ 1:��ʾ�͵�
uint8_t old_charge_flag = 0xff;//���״ָ̬ʾ
uint8_t old_adc_dl = 0xff;
uint8_t bat_pct;//�����ٷֱ�
uint8_t old_bat_pct=10;
uint8_t cmp_flag1;
uint8_t cmp_flag2;
uint8_t first_show1;
uint8_t first_show2;
uint8_t oled_show_flag = 1;//�����仯ʱ������ˢ��0������ʾ0LED,1����ʾOLED
extern uint8_t test_i;
typedef struct
{
	uint8_t str_id[9];
	uint8_t str_time[16];
	uint8_t str_adc[5];
}str_Type;

str_Type str_temp;
uint8_t my_strcmp(uint8_t *str1,uint8_t *str2,uint8_t strlen)
{
	uint8_t len;
	for(len=0;len<strlen;len++)
	{
		if(*(str1+len) != *(str2+len))
		{
			return 0;
		}
	}
	return 1;
}

void my_copy(uint8_t *str1,uint8_t *str2,uint8_t strlen)
{
	uint8_t len;
	for(len=0;len<strlen;len++)
	{
		*(str1+len) = *(str2+len);
	}
}

//����LCD��ʾ���Ƿ���GPS�źţ��Ƿ���ʾ�������͵�����־λ������ģʽ������ֵ���汾�ţ�ʱ�䡣
//ֻ��״̬�ı�ʱ������ʾOLED
void OLED_SHOW(uint8_t En_Lcd_Flag1,uint8_t GPS_Flag1,uint8_t ADC_FLAG1,uint8_t ADC_DD_FLAG1,uint8_t charge_flag1,uint8_t Work_Mode1,uint8_t ADC_DL1,uint8_t * caucAddr21,uint8_t* aucRTC1)
{
	if(En_Lcd_Flag1)
	{	
		En_Lcd_Flag1 = 0;
		if(old_gps_flag!= GPS_Flag1)
		{
			old_gps_flag = GPS_Flag1;
			oled_show_flag = 1;
			if(GPS_Flag1)
			{
		#if OLED_ZK==1
				LcdDisplay_char(0,6," GPS");
		#else
				OLED_ShowString(0,48," GPS",16);
		#endif
	//			LcdDisplay_char(0,6," GPS");
//				GPS_Flag1=0;
			}
			else
			{
				#if OLED_ZK==1
				LcdDisplay_char(0,6," ---");
				#else
				OLED_ShowString(0,48," ---",16);
				#endif
	//			LcdDisplay_char(0,6," ---");
			}	
		}
//		LcdDisplay_char(56,6,"V2.0_0.3");
		cmp_flag1 = my_strcmp(caucAddr21,old_caucAddr2,4);
		if(0 == cmp_flag1 || 0 == first_show1)//�汾�ű仯�����ߵ�һ��ʱ������OLED
		{
			first_show1 =1;//
			my_copy(old_caucAddr2,caucAddr21,4);
			oled_show_flag = 1;
		#if OLED_ZK==1
			LcdDisplay_char(56,6,"V2.0_0.3");
		#else
			OLED_ShowString(56,48,"V2.0_0.3",16);
		#endif
			switch(caucAddr21[0]>>4)
			{
				 case 0:str_temp.str_id[0] = '0';break;
				 case 1:str_temp.str_id[0] = '1';break;
				 case 2:str_temp.str_id[0] = '2';break;
				 case 3:str_temp.str_id[0] = '3';break;
				 case 4:str_temp.str_id[0] = '4';break;
				 case 5:str_temp.str_id[0] = '5';break;
				 case 6:str_temp.str_id[0] = '6';break;
				 case 7:str_temp.str_id[0] = '7';break;
				 case 8:str_temp.str_id[0] = '8';break;
				 case 9:str_temp.str_id[0] = '9';break;
				 case 10:str_temp.str_id[0] = 'A';break;
				 case 11:str_temp.str_id[0] = 'B';break;
				 case 12:str_temp.str_id[0] = 'C';break;
				 case 13:str_temp.str_id[0] = 'D';break;
				 case 14:str_temp.str_id[0] = 'E';break;
				 case 15:str_temp.str_id[0] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[0]&0x0f)
			{
				 case 0:str_temp.str_id[1] = '0';break;
				 case 1:str_temp.str_id[1] = '1';break;
				 case 2:str_temp.str_id[1] = '2';break;
				 case 3:str_temp.str_id[1] = '3';break;
				 case 4:str_temp.str_id[1] = '4';break;
				 case 5:str_temp.str_id[1] = '5';break;
				 case 6:str_temp.str_id[1] = '6';break;
				 case 7:str_temp.str_id[1] = '7';break;
				 case 8:str_temp.str_id[1] = '8';break;
				 case 9:str_temp.str_id[1] = '9';break;
				 case 10:str_temp.str_id[1] = 'A';break;
				 case 11:str_temp.str_id[1] = 'B';break;
				 case 12:str_temp.str_id[1] = 'C';break;
				 case 13:str_temp.str_id[1] = 'D';break;
				 case 14:str_temp.str_id[1] = 'E';break;
				 case 15:str_temp.str_id[1] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[1]>>4)
			{
				 case 0:str_temp.str_id[2] = '0';break;
				 case 1:str_temp.str_id[2] = '1';break;
				 case 2:str_temp.str_id[2] = '2';break;
				 case 3:str_temp.str_id[2] = '3';break;
				 case 4:str_temp.str_id[2] = '4';break;
				 case 5:str_temp.str_id[2] = '5';break;
				 case 6:str_temp.str_id[2] = '6';break;
				 case 7:str_temp.str_id[2] = '7';break;
				 case 8:str_temp.str_id[2] = '8';break;
				 case 9:str_temp.str_id[2] = '9';break;
				 case 10:str_temp.str_id[2] = 'A';break;
				 case 11:str_temp.str_id[2] = 'B';break;
				 case 12:str_temp.str_id[2] = 'C';break;
				 case 13:str_temp.str_id[2] = 'D';break;
				 case 14:str_temp.str_id[2] = 'E';break;
				 case 15:str_temp.str_id[2] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[1]&0x0f)
			{
				 case 0:str_temp.str_id[3] = '0';break;
				 case 1:str_temp.str_id[3] = '1';break;
				 case 2:str_temp.str_id[3] = '2';break;
				 case 3:str_temp.str_id[3] = '3';break;
				 case 4:str_temp.str_id[3] = '4';break;
				 case 5:str_temp.str_id[3] = '5';break;
				 case 6:str_temp.str_id[3] = '6';break;
				 case 7:str_temp.str_id[3] = '7';break;
				 case 8:str_temp.str_id[3] = '8';break;
				 case 9:str_temp.str_id[3] = '9';break;
				 case 10:str_temp.str_id[3] = 'A';break;
				 case 11:str_temp.str_id[3] = 'B';break;
				 case 12:str_temp.str_id[3] = 'C';break;
				 case 13:str_temp.str_id[3] = 'D';break;
				 case 14:str_temp.str_id[3] = 'E';break;
				 case 15:str_temp.str_id[3] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[2]>>4)
			{
				 case 0:str_temp.str_id[4] = '0';break;
				 case 1:str_temp.str_id[4] = '1';break;
				 case 2:str_temp.str_id[4] = '2';break;
				 case 3:str_temp.str_id[4] = '3';break;
				 case 4:str_temp.str_id[4] = '4';break;
				 case 5:str_temp.str_id[4] = '5';break;
				 case 6:str_temp.str_id[4] = '6';break;
				 case 7:str_temp.str_id[4] = '7';break;
				 case 8:str_temp.str_id[4] = '8';break;
				 case 9:str_temp.str_id[4] = '9';break;
				 case 10:str_temp.str_id[4] = 'A';break;
				 case 11:str_temp.str_id[4] = 'B';break;
				 case 12:str_temp.str_id[4] = 'C';break;
				 case 13:str_temp.str_id[4] = 'D';break;
				 case 14:str_temp.str_id[4] = 'E';break;
				 case 15:str_temp.str_id[4] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[2]&0x0f)
			{
				 case 0:str_temp.str_id[5] = '0';break;
				 case 1:str_temp.str_id[5] = '1';break;
				 case 2:str_temp.str_id[5] = '2';break;
				 case 3:str_temp.str_id[5] = '3';break;
				 case 4:str_temp.str_id[5] = '4';break;
				 case 5:str_temp.str_id[5] = '5';break;
				 case 6:str_temp.str_id[5] = '6';break;
				 case 7:str_temp.str_id[5] = '7';break;
				 case 8:str_temp.str_id[5] = '8';break;
				 case 9:str_temp.str_id[5] = '9';break;
				 case 10:str_temp.str_id[5] = 'A';break;
				 case 11:str_temp.str_id[5] = 'B';break;
				 case 12:str_temp.str_id[5] = 'C';break;
				 case 13:str_temp.str_id[5] = 'D';break;
				 case 14:str_temp.str_id[5] = 'E';break;
				 case 15:str_temp.str_id[5] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[3]>>4)
			{
				 case 0:str_temp.str_id[6] = '0';break;
				 case 1:str_temp.str_id[6] = '1';break;
				 case 2:str_temp.str_id[6] = '2';break;
				 case 3:str_temp.str_id[6] = '3';break;
				 case 4:str_temp.str_id[6] = '4';break;
				 case 5:str_temp.str_id[6] = '5';break;
				 case 6:str_temp.str_id[6] = '6';break;
				 case 7:str_temp.str_id[6] = '7';break;
				 case 8:str_temp.str_id[6] = '8';break;
				 case 9:str_temp.str_id[6] = '9';break;
				 case 10:str_temp.str_id[6] = 'A';break;
				 case 11:str_temp.str_id[6] = 'B';break;
				 case 12:str_temp.str_id[6] = 'C';break;
				 case 13:str_temp.str_id[6] = 'D';break;
				 case 14:str_temp.str_id[6] = 'E';break;
				 case 15:str_temp.str_id[6] = 'F';break;
				 default:break;
			}
			switch(caucAddr21[3]&0x0f)
			{
				 case 0:str_temp.str_id[7] = '0';break;
				 case 1:str_temp.str_id[7] = '1';break;
				 case 2:str_temp.str_id[7] = '2';break;
				 case 3:str_temp.str_id[7] = '3';break;
				 case 4:str_temp.str_id[7] = '4';break;
				 case 5:str_temp.str_id[7] = '5';break;
				 case 6:str_temp.str_id[7] = '6';break;
				 case 7:str_temp.str_id[7] = '7';break;
				 case 8:str_temp.str_id[7] = '8';break;
				 case 9:str_temp.str_id[7] = '9';break;
				 case 10:str_temp.str_id[7] = 'A';break;
				 case 11:str_temp.str_id[7] = 'B';break;
				 case 12:str_temp.str_id[7] = 'C';break;
				 case 13:str_temp.str_id[7] = 'D';break;
				 case 14:str_temp.str_id[7] = 'E';break;
				 case 15:str_temp.str_id[7] = 'F';break;
				 default:break;
			}	
			str_temp.str_id[8] = '\0';
		#if OLED_ZK==1
			LcdDisplay_char(8,2,str_temp.str_id);
		#else
			OLED_ShowString(8,16,str_temp.str_id,16);
		#endif
		}
//		if(old_adc_flag != ADC_FLAG1)
//		{
//			old_adc_flag = ADC_FLAG1;
			if(old_charge_flag != charge_flag1)
			{
				if(charge_flag1)
				{
					show_flash(112,16,1);
				}
				else
				{
					OLED_ShowString(112,16,"  ",16);
				}
				old_charge_flag = charge_flag1;
			}
			if(ADC_FLAG1)
			{
				if(old_adc_dd_flag != ADC_DD_FLAG1)
				{
					oled_show_flag = 1;
					old_adc_dd_flag = ADC_DD_FLAG1;
					if(ADC_DD_FLAG1)
					{
					#if OLED_ZK==1
						LcdDisplay_Chinese(88,2,"�͵�");
					#else
						OLED_ShowString(88,16,"Low   ",16);
					#endif
						dd_flag = 1;  //
					}
					else
					{
						dd_flag = 0;
					}
				}
				if(0 == dd_flag)
				{
					if(old_adc_dl!=ADC_DL1)
					{
						old_adc_dl = ADC_DL1;
						
			#if DIANCHI_MODE==1
						/*2\0-19 20%
							3\20-39 40%
							4\40-59 60%
							5\60-79 80%
							6\80-99 100%*/
						/*���ʱ
							1\0-19 10%
							2\20-39 20%
							3\40-59 40%
							4\60-79 60%
							5\80-99 80%
						  100 100%*/
						bat_pct = ADC_DL1/20+1;
						if(old_bat_pct !=bat_pct)
						{
								//���ģʽ
							old_bat_pct = bat_pct;
							#if OLED_ZK==1
		//						LcdDisplay_char(88,2,str_temp.str_adc);
							#else
								show_battery(88,16,bat_pct,1);
								oled_show_flag = 1;
							#endif
						}
						
			#else			
						oled_show_flag = 1;
						//�õ�ش���ٷֱ�
						if(ADC_DL1>=100)
						{
							 switch(ADC_DL1/100)
							 {
								 case 0:str_temp.str_adc[0] = '0';break;
								 case 1:str_temp.str_adc[0] = '1';break;
								 case 2:str_temp.str_adc[0] = '2';break;
								 case 3:str_temp.str_adc[0] = '3';break;
								 case 4:str_temp.str_adc[0] = '4';break;
								 case 5:str_temp.str_adc[0] = '5';break;
								 case 6:str_temp.str_adc[0] = '6';break;
								 case 7:str_temp.str_adc[0] = '7';break;
								 case 8:str_temp.str_adc[0] = '8';break;
								 case 9:str_temp.str_adc[0] = '9';break;
								 default:break;
							 }
						}
						else
						{
							str_temp.str_adc[0] = ' ';
						}
						switch(ADC_DL1%100/10)
						{
						 case 0:str_temp.str_adc[1] = '0';break;
						 case 1:str_temp.str_adc[1] = '1';break;
						 case 2:str_temp.str_adc[1] = '2';break;
						 case 3:str_temp.str_adc[1] = '3';break;
						 case 4:str_temp.str_adc[1] = '4';break;
						 case 5:str_temp.str_adc[1] = '5';break;
						 case 6:str_temp.str_adc[1] = '6';break;
						 case 7:str_temp.str_adc[1] = '7';break;
						 case 8:str_temp.str_adc[1] = '8';break;
						 case 9:str_temp.str_adc[1] = '9';break;
						 default:break;
						}
						switch(ADC_DL1%10)
						{
						 case 0:str_temp.str_adc[2] = '0';break;
						 case 1:str_temp.str_adc[2] = '1';break;
						 case 2:str_temp.str_adc[2] = '2';break;
						 case 3:str_temp.str_adc[2] = '3';break;
						 case 4:str_temp.str_adc[2] = '4';break;
						 case 5:str_temp.str_adc[2] = '5';break;
						 case 6:str_temp.str_adc[2] = '6';break;
						 case 7:str_temp.str_adc[2] = '7';break;
						 case 8:str_temp.str_adc[2] = '8';break;
						 case 9:str_temp.str_adc[2] = '9';break;
						 default:break;
						}		
						str_temp.str_adc[3] = '%';
						str_temp.str_adc[4] = '\0';
					#if OLED_ZK==1
						LcdDisplay_char(88,2,str_temp.str_adc);
					#else
						OLED_ShowString(88,16,str_temp.str_adc,16);
					#endif
			#endif
					}		
				}
			}
			else
			{
//			#if OLED_ZK==1
//				LcdDisplay_char(88,2,"---%");
//			#else
//				OLED_ShowString(88,16,"---%",16);
//			#endif
			}	
//		}			
		cmp_flag2 = my_strcmp(aucRTC1,old_aucRTC,5);
		if(0 == cmp_flag2 || 0 == first_show2)//ʱ��仯�����ߵ�һ��ʱ������OLED
		{		       
			first_show2 = 1;
			oled_show_flag = 1;
			my_copy(old_aucRTC,aucRTC1,5);
			str_temp.str_time[0] = ' ';
			switch(aucRTC1[0]>>4)
			{
				case 0:str_temp.str_time[1] = '0';break;
				case 1:str_temp.str_time[1] = '1';break;
				case 2:str_temp.str_time[1] = '2';break;
				case 3:str_temp.str_time[1] = '3';break;
				case 4:str_temp.str_time[1] = '4';break;
				case 5:str_temp.str_time[1] = '5';break;
				case 6:str_temp.str_time[1] = '6';break;
				case 7:str_temp.str_time[1] = '7';break;
				case 8:str_temp.str_time[1] = '8';break;
				case 9:str_temp.str_time[1] = '9';break;
				default:break;
			}
			switch(aucRTC1[0]&0x0f)
			{
				case 0:str_temp.str_time[2] = '0';break;
				case 1:str_temp.str_time[2] = '1';break;
				case 2:str_temp.str_time[2] = '2';break;
				case 3:str_temp.str_time[2] = '3';break;
				case 4:str_temp.str_time[2] = '4';break;
				case 5:str_temp.str_time[2] = '5';break;
				case 6:str_temp.str_time[2] = '6';break;
				case 7:str_temp.str_time[2] = '7';break;
				case 8:str_temp.str_time[2] = '8';break;
				case 9:str_temp.str_time[2] = '9';break;
				default:break;
			}
			str_temp.str_time[3] = '-';
			switch(aucRTC1[1]>>4)
			{
				case 0:str_temp.str_time[4] = '0';break;
				case 1:str_temp.str_time[4] = '1';break;
				case 2:str_temp.str_time[4] = '2';break;
				case 3:str_temp.str_time[4] = '3';break;
				case 4:str_temp.str_time[4] = '4';break;
				case 5:str_temp.str_time[4] = '5';break;
				case 6:str_temp.str_time[4] = '6';break;
				case 7:str_temp.str_time[4] = '7';break;
				case 8:str_temp.str_time[4] = '8';break;
				case 9:str_temp.str_time[4] = '9';break;
				default:break;
			}
			switch(aucRTC1[1]&0x0f)
			{
				case 0:str_temp.str_time[5] = '0';break;
				case 1:str_temp.str_time[5] = '1';break;
				case 2:str_temp.str_time[5] = '2';break;
				case 3:str_temp.str_time[5] = '3';break;
				case 4:str_temp.str_time[5] = '4';break;
				case 5:str_temp.str_time[5] = '5';break;
				case 6:str_temp.str_time[5] = '6';break;
				case 7:str_temp.str_time[5] = '7';break;
				case 8:str_temp.str_time[5] = '8';break;
				case 9:str_temp.str_time[5] = '9';break;
				default:break;
			}
			str_temp.str_time[6] = '-';
			switch(aucRTC1[2]>>4)
			{
				case 0:str_temp.str_time[7] = '0';break;
				case 1:str_temp.str_time[7] = '1';break;
				case 2:str_temp.str_time[7] = '2';break;
				case 3:str_temp.str_time[7] = '3';break;
				case 4:str_temp.str_time[7] = '4';break;
				case 5:str_temp.str_time[7] = '5';break;
				case 6:str_temp.str_time[7] = '6';break;
				case 7:str_temp.str_time[7] = '7';break;
				case 8:str_temp.str_time[7] = '8';break;
				case 9:str_temp.str_time[7] = '9';break;
				default:break;
			}
			switch(aucRTC1[2]&0x0f)
			{
				case 0:str_temp.str_time[8] = '0';break;
				case 1:str_temp.str_time[8] = '1';break;
				case 2:str_temp.str_time[8] = '2';break;
				case 3:str_temp.str_time[8] = '3';break;
				case 4:str_temp.str_time[8] = '4';break;
				case 5:str_temp.str_time[8] = '5';break;
				case 6:str_temp.str_time[8] = '6';break;
				case 7:str_temp.str_time[8] = '7';break;
				case 8:str_temp.str_time[8] = '8';break;
				case 9:str_temp.str_time[8] = '9';break;
				default:break;
			}
			str_temp.str_time[9] = ' ';
			switch(aucRTC1[3]>>4)
			{
				case 0:str_temp.str_time[10] = '0';break;
				case 1:str_temp.str_time[10] = '1';break;
				case 2:str_temp.str_time[10] = '2';break;
				case 3:str_temp.str_time[10] = '3';break;
				case 4:str_temp.str_time[10] = '4';break;
				case 5:str_temp.str_time[10] = '5';break;
				case 6:str_temp.str_time[10] = '6';break;
				case 7:str_temp.str_time[10] = '7';break;
				case 8:str_temp.str_time[10] = '8';break;
				case 9:str_temp.str_time[10] = '9';break;
				default:break;
			}
			switch(aucRTC1[3]&0x0f)
			{
				case 0:str_temp.str_time[11] = '0';break;
				case 1:str_temp.str_time[11] = '1';break;
				case 2:str_temp.str_time[11] = '2';break;
				case 3:str_temp.str_time[11] = '3';break;
				case 4:str_temp.str_time[11] = '4';break;
				case 5:str_temp.str_time[11] = '5';break;
				case 6:str_temp.str_time[11] = '6';break;
				case 7:str_temp.str_time[11] = '7';break;
				case 8:str_temp.str_time[11] = '8';break;
				case 9:str_temp.str_time[11] = '9';break;
				default:break;
			}
			str_temp.str_time[12] = ':';
			switch(aucRTC1[4]>>4)
			{
				case 0:str_temp.str_time[13] = '0';break;
				case 1:str_temp.str_time[13] = '1';break;
				case 2:str_temp.str_time[13] = '2';break;
				case 3:str_temp.str_time[13] = '3';break;
				case 4:str_temp.str_time[13] = '4';break;
				case 5:str_temp.str_time[13] = '5';break;
				case 6:str_temp.str_time[13] = '6';break;
				case 7:str_temp.str_time[13] = '7';break;
				case 8:str_temp.str_time[13] = '8';break;
				case 9:str_temp.str_time[13] = '9';break;
				default:break;
			}
			switch(aucRTC1[4]&0x0f)
			{
				case 0:str_temp.str_time[14] = '0';break;
				case 1:str_temp.str_time[14] = '1';break;
				case 2:str_temp.str_time[14] = '2';break;
				case 3:str_temp.str_time[14] = '3';break;
				case 4:str_temp.str_time[14] = '4';break;
				case 5:str_temp.str_time[14] = '5';break;
				case 6:str_temp.str_time[14] = '6';break;
				case 7:str_temp.str_time[14] = '7';break;
				case 8:str_temp.str_time[14] = '8';break;
				case 9:str_temp.str_time[14] = '9';break;
				default:break;
			}
			str_temp.str_time[15] = '\0';
		#if OLED_ZK==1
			LcdDisplay_char(0,4,str_temp.str_time);
		#else
			OLED_ShowString(0,32,str_temp.str_time,16);
		#endif
		}
	  if(old_work_mode!=Work_Mode1)
		{
			old_work_mode = Work_Mode1;
			oled_show_flag = 1;
		#if OLED_ZK==1
			switch(Work_Mode1)
			{			
				case POWER_STATE://������
						 LcdDisplay_Chinese(48,0,"����");
						 LcdDisplay_char(80,0,"...");
						 break;
				case CONNECT_STATE://������
						 LcdDisplay_char(0,6," ---");
						 LcdDisplay_Chinese(32,0,"������");
						 LcdDisplay_char(80,0,"...");
						 break;
				case ONLINE_STATE://������	
						 LcdDisplay_char(32,0,"  ");	
						 LcdDisplay_Chinese(48,0,"����");
						 LcdDisplay_char(80,0,"   ");				
						 break;
				case OFFLINE_STATE://����
						 LcdDisplay_char(32,0,"  ");
						 LcdDisplay_Chinese(48,0,"����");
						 LcdDisplay_char(80,0,"   ");				
						 break;
				case SHUTDOWN_STATE://�ػ�
						 LcdDisplay_char(32,0,"  ");
						 LcdDisplay_Chinese(48,0,"�ػ�");
						 LcdDisplay_char(80,0,"...");	
				default:break;
			}
			#else
			switch(Work_Mode1)
			{			
				case POWER_STATE://������
							OLED_ShowString(32,0,"Power On...",16);
//						 LcdDisplay_Chinese(48,0,"����");
//						 LcdDisplay_char(80,0,"...");
						 break;
				case CONNECT_STATE://������
							OLED_ShowString(32,0,"Connect... ",16);
//						 LcdDisplay_char(0,6," ---");
//						 LcdDisplay_Chinese(32,0,"������");
//						 LcdDisplay_char(80,0,"...");
						 break;
				case ONLINE_STATE://������	
							OLED_ShowString(32,0,"Online     ",16);
//						 LcdDisplay_char(32,0,"  ");	
//						 LcdDisplay_Chinese(48,0,"����");
//						 LcdDisplay_char(80,0,"   ");				
						 break;
				case OFFLINE_STATE://����
							OLED_ShowString(32,0,"Offline... ",16);
//						 LcdDisplay_char(32,0,"  ");
//						 LcdDisplay_Chinese(48,0,"����");
//						 LcdDisplay_char(80,0,"   ");				
						 break;
				case SHUTDOWN_STATE://�ػ�
							OLED_ShowString(32,0,"ShutDn...  ",16);
//						 LcdDisplay_char(32,0,"  ");
//						 LcdDisplay_Chinese(48,0,"�ػ�");
//						 LcdDisplay_char(80,0,"...");	
				default:break;
			}
			#endif
		}
		#if OLED_ZK == 0
		if(1 == oled_show_flag)
		{
			OLED_Refresh_Gram();
			oled_show_flag = 0;
		}
		#endif
	}
}



