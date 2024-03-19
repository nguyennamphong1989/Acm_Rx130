/*
 * apl_status.c
 *
 *  Created on: Aug 1, 2022
 *      Author: Lover_boy_9x
 */
#include "apl_status.h"
#include "apl_config.h"
#include "timeCheck.h"
#include "control.h"
#include "sensor.h"
#include "Config_RIIC0.h"
#include "gpio.h"


#define HUM_THRESHOLD 80
#define HUM_DEBOUCH 2
extern volatile uint32_t g_sysTime;
extern bool enableCheckError_Air1, enableCheckError_Air2;
extern uint32_t timeTempbiggerThreshold_Air1, timeTempbiggerThreshold_Air2;
extern uint16_t error_sensor;
uint16_t error_air[4];
static AIRC_STATUS_t g_status_airc;
static autoState_t s_autoState = AUTO_STATE_INIT;
static alternateAC_t s_acAlternate;
static humState_t s_humState = HUM_STATE_INIT;
uint32_t s_timeSendControlAcs[AC_NUM];
extern uint32_t timeStartNormal[AC_NUM];

const ac_data_t S_AC_DATA_ON = {
	.ac_powerModeCmd = AC_POWER_ON,
	.ac_operatingModeCmd = AC_MODE_COOL,
	.ac_fanSpeedCmd = AC_FAN_AUTO,
	.ac_temperatureCmd = 25};

const ac_data_t S_AC_DATA_Normal = {
	.ac_powerModeCmd = AC_POWER_ON,
	.ac_operatingModeCmd = AC_MODE_AUTO,
	.ac_fanSpeedCmd = AC_FAN_AUTO,
	.ac_temperatureCmd = 25};
	
const ac_data_t S_AC_DATA_Enhance = {
	.ac_powerModeCmd = AC_POWER_ON,
	.ac_operatingModeCmd = AC_MODE_AUTO,
	.ac_fanSpeedCmd = AC_FAN_AUTO,
	.ac_temperatureCmd = 20};
	
const ac_data_t S_AC_DATA_Dry = {
	.ac_powerModeCmd = AC_POWER_ON,
	.ac_operatingModeCmd = AC_MODE_DRY,
	.ac_fanSpeedCmd = AC_FAN_AUTO,
	.ac_temperatureCmd = 25};

const ac_data_t S_AC_DATA_OFF = {
	.ac_powerModeCmd = AC_POWER_OFF,
	.ac_operatingModeCmd = AC_MODE_AUTO,
	.ac_fanSpeedCmd = AC_FAN_AUTO,
	.ac_temperatureCmd = 25};
	
ac_data_t s_needSendAcData[AC_NUM];
bool enterModeErrorAuto = false;
bool errorSensorAuto = false;
uint32_t timeStartAuto;
uint8_t count_air1, count_air2;
bool s_needCheckAcs[AC_NUM]; bool enterStateAuto_1, enterStateAuto_2 = false;
uint8_t setTemp[AC_NUM]; //Lấy nhiệt độ cài đặt ở thời điểm điều khiển
uint8_t air1TempAtSet; //Lấy nhiệt độ cửa gió ở thời điểm điều khiển
uint8_t air2TempAtSet;
uint8_t indoorTempAtSet; //Lấy nhiệt độ cửa gió ở thời điểm điều khiển
uint8_t samplecnt1;
uint8_t samplecnt2;
uint8_t air1_min;
uint8_t air2_min;
uint8_t air1_max;
uint8_t air2_max;
uint32_t time_samplingTemp[AC_NUM];
uint8_t previous1;
uint8_t previous2;
uint16_t higherTempCnt1;
uint16_t higherTempCnt2;
uint16_t lowerTempCnt1;
uint16_t lowerTempCnt2;

volatile static uint8_t countErrorAuto_1, countErrorAuto_2;
volatile static uint32_t timeCountErrorAuto_1, timeCountErrorAuto_2;
void APL_stt_processErrorAuto();
void APL_stt_setCurrentAlternateAcState(ac_data_t acData);
void Checkstt(uint8_t processingAC, uint8_t *acStatus)
{
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();

	if (processingAC == AIRC1_ID)
	{
		if(s_needSendAcData[processingAC].ac_powerModeCmd == AC_POWER_ON)
		{
			if((air1_max - air1_min > 2 && air1TempAtSet >= air1_min )
					|| (air1_max - air1_min <= 2 && air1_max <= 26 && p_stt->temp_indoor - air1_min >= 1))
				*acStatus= AC_POWER_ON;
			else
			{
				*acStatus = AC_POWER_OFF;
			}
		}
		else if(s_needSendAcData[processingAC].ac_powerModeCmd == AC_POWER_OFF)
		{
			if((air1_max - air1_min > 2 && air1TempAtSet < air1_max )
					|| (air1_max - air1_min <= 2 && air1_min >= 24 && air2_min <= air1_min ))
				*acStatus= AC_POWER_OFF;
			else
			{
				*acStatus = AC_POWER_ON;
			}
		}
	}
	if (processingAC == AIRC2_ID)
	{
		if(s_needSendAcData[processingAC].ac_powerModeCmd == AC_POWER_ON)
		{
			if((air2_max - air2_min > 2 && air2TempAtSet >= air2_min )
					|| (air2_max - air2_min <= 2 && air2_max <= 26 && p_stt->temp_indoor - air2_min >= 1))
				*acStatus= AC_POWER_ON;
			else
			{
				*acStatus = AC_POWER_OFF;
			}
		}
		else if(s_needSendAcData[processingAC].ac_powerModeCmd == AC_POWER_OFF)
		{
			if((air2_max - air2_min > 2 && air2TempAtSet < air2_max )
					|| (air2_max - air2_min <= 2 && air2_min >= 24 && air1_min <= air2_min))
				*acStatus= AC_POWER_OFF;
			else
			{
				*acStatus = AC_POWER_ON;
			}
		}
	}
}
void CheckError()
{
	uint8_t acStatus;
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();
	if (p_stt->mode_auto_en == MODE_AUTO) //che do auto
	{
		if (enterModeErrorAuto) //khi mode auto vao che do loi bat ca 2 dieu hoa che do 4
		{
			if (elapsedTime(g_sysTime, time_samplingTemp[AIRC1_ID]) > 60000) //1min lưu lại nhiệt độ min
			{
				time_samplingTemp[AIRC1_ID] = g_sysTime;
				if(g_status_airc.temp_air_1 < air1_min)
				{
					air1_min = g_status_airc.temp_air_1;
				}
				if(g_status_airc.temp_air_1 > air1_max)
				{
					air1_max = g_status_airc.temp_air_1;
				}

				if(g_status_airc.temp_air_1>=previous1)
				{
					higherTempCnt1++;
				}
				else
				{
					higherTempCnt1=0;
				}
				previous1=g_status_airc.temp_air_1;
			}
			if (elapsedTime(g_sysTime, time_samplingTemp[AIRC2_ID]) > 60000) //1min
			{
				time_samplingTemp[AIRC2_ID] = g_sysTime;
				if(g_status_airc.temp_air_2 < air2_min)
				{
					air2_min = g_status_airc.temp_air_2;
				}
				if(g_status_airc.temp_air_2 > air2_max)
				{
					air2_max = g_status_airc.temp_air_2;
				}

				if(g_status_airc.temp_air_2 >= previous2)
				{
					higherTempCnt2++;
				}
				else
				{
					higherTempCnt2=0;
				}
				previous2=g_status_airc.temp_air_2;
			}


			//Ra quyet dinh 10p/lan
			if (elapsedTime(g_sysTime, timeCountErrorAuto_1) > 1200000) //20 mins
			{
				timeCountErrorAuto_1 = g_sysTime;
				Checkstt(AIRC1_ID, &acStatus);// Kiểm tra status theo nhiệt độ
				g_status_airc.air1_sts = acStatus; //Cập nhật status ON/OFF thực tế
				if(acStatus==AC_POWER_OFF)
				{
					error_air[AIRC1_ID] |= 0xFF00; //Ghi lỗi AC1
					APL_stt_processErrorAuto();
				}
				else
				{
					error_air[AIRC1_ID] &= ~0xFF00; //Xoa loi
				}
			}

			if (elapsedTime(g_sysTime, timeCountErrorAuto_2) > 600000)//10min
			{
				timeCountErrorAuto_2 = g_sysTime;
				Checkstt(AIRC2_ID, &acStatus);// Kiểm tra status theo nhiệt độ
				g_status_airc.air2_sts = acStatus; //Cập nhật status ON/OFF thực tế
				if(acStatus==AC_POWER_OFF)
				{
					error_air[AIRC2_ID] |= 0xFF00; //Ghi lỗi AC1
					APL_stt_processErrorAuto();
				}
				else
				{
					error_air[AIRC2_ID] &= ~0xFF00; //Xoa loi
				}
			}
		}
		else //không trong ModeError
		{
			if (enterStateAuto_1 && enterStateAuto_2) // khi da on/off 2 dieu hoa thanh cong
			{
				// Sau khi điều khiển điều hòa thành công, check 1 phút 1 lần
				if (elapsedTime(g_sysTime, timeStartAuto) > 60000)
	            {
	            	timeStartAuto = g_sysTime;

//	            	//AC1
//	            	if(g_status_airc.temp_air_1>=previous1)
//	            	{
//	            		higherTempCnt1++;
//	            	}
//	            	else
//	            	{
//	            		higherTempCnt1=0;
//	            	}
//	            	previous1=g_status_airc.temp_air_1;
//
//	            	if(higherTempCnt1>20 && g_status_airc.temp_air_1> 28) //20 phut lien tiep nhiet do khong giam
//	            	{
//	            		if(s_needSendAcData[AIRC1_ID].ac_powerModeCmd == AC_POWER_ON)
//	            		{
//		                    error_air[AIRC1_ID] |= 0xFF00;
//		                    APL_stt_processErrorAuto();
//	            		}
//	            		else //AC_POWER_OFF
//	            		{
//	            			error_air[AIRC1_ID] &= ~0xFF00;// Xóa lỗi
//	            		}
//	            	}
//
//	            	//AC2
//	            	if(g_status_airc.temp_air_2>=previous2)
//	            	{
//	            		higherTempCnt2++;
//	            	}
//	            	else
//	            	{
//	            		higherTempCnt2=0;
//	            	}
//	            	previous2=g_status_airc.temp_air_2;
//
//	            	if(higherTempCnt2>20 && g_status_airc.temp_air_1> 28) //15 phut lien tiep nhiet do khong giam
//	            	{
//	            		if(s_needSendAcData[AIRC2_ID].ac_powerModeCmd == AC_POWER_ON)
//	            		{
//	            			error_air[AIRC2_ID] |= 0xFF00; //Ghi lỗi AC2
//	            			APL_stt_processErrorAuto();
//	            		}
//	            		else
//	            		{
//	            			error_air[AIRC2_ID] &= ~0xFF00;// Xóa lỗi
//	            		}
//	            	}
	            	//AC1
	            	if(s_needSendAcData[AIRC1_ID].ac_powerModeCmd == AC_POWER_ON)
	            	{
						if(g_status_airc.temp_air_1>=previous1)
						{
							higherTempCnt1++;
						}
						else
						{
							higherTempCnt1=0;
						}
						previous1=g_status_airc.temp_air_1;

						if(higherTempCnt1>20 && g_status_airc.temp_air_1> 25) //20 phut lien tiep nhiet do khong giam & nhiet do cua gio >25
						{
							error_air[AIRC1_ID] |= 0xFF00;
							APL_stt_processErrorAuto();
						}
						else
						{
							error_air[AIRC1_ID] &= ~0xFF00;// Xóa lỗi
						}

	            	}
	            	else if (s_needSendAcData[AIRC1_ID].ac_powerModeCmd == AC_POWER_OFF)
	            	{
						if(g_status_airc.temp_air_1<previous1)
						{
							lowerTempCnt1++; //tăng biến đếm thời gian nhiệt độ cửa gió giảm
						}
						else
						{
							lowerTempCnt1=0;
						}
						previous1=g_status_airc.temp_air_1;

						if(lowerTempCnt1 > 1) //nhiet độ cửa gió giảm 2 phút liên tiếp
						{
							error_air[AIRC1_ID] |= 0xFF00;
							APL_stt_processErrorAuto();
						}
						else
						{
							error_air[AIRC1_ID] &= ~0xFF00;// Xóa lỗi
						}
	            	}
	            	//AC2
	            	if(s_needSendAcData[AIRC2_ID].ac_powerModeCmd == AC_POWER_ON)
	            	{
						if(g_status_airc.temp_air_2>=previous2)
						{
							higherTempCnt2++;
						}
						else
						{
							higherTempCnt2=0;
						}
						previous2=g_status_airc.temp_air_2;

						if(higherTempCnt2>20 && g_status_airc.temp_air_2> 25) //20 phut lien tiep nhiet do khong giam & nhiet do cua gio >25
						{
							error_air[AIRC2_ID] |= 0xFF00;
							APL_stt_processErrorAuto();
						}
						else
						{
							error_air[AIRC2_ID] &= ~0xFF00;// Xóa lỗi
						}

	            	}
	            	else if (s_needSendAcData[AIRC2_ID].ac_powerModeCmd == AC_POWER_OFF)
	            	{
						if(g_status_airc.temp_air_2 < previous2)
						{
							lowerTempCnt2++; //tăng biến đếm thời gian nhiệt độ cửa gió giảm
						}
						else
						{
							lowerTempCnt2=0;
						}
						previous2=g_status_airc.temp_air_2;



						if(lowerTempCnt2 > 1) //nhiet độ cửa gió giảm 2 phút liên tiếp
						{
							error_air[AIRC2_ID] |= 0xFF00;
							APL_stt_processErrorAuto();
						}
						else
						{
							error_air[AIRC2_ID] &= ~0xFF00;// Xóa lỗi
						}
	            	}

	            }
			}
		}
	}
}
void APL_stt_init(void)
{
	memset(&g_status_airc, 0x00, sizeof(g_status_airc));

	// Default turn on all Fan/Airc on boot until receive command control from GW
	// osl_turn_airc1(ON);
	// osl_turn_airc2(ON);
	osl_turn_fan(OFF);

	R_BSP_SoftwareDelay(500, BSP_DELAY_MILLISECS);
	// osl_turn_airc1_latch_low(ON);
	// osl_turn_airc2_latch_low(ON);
	s_acAlternate.s_currentAcOn = AIRC1_ID; // in the furture get ramdom
	g_status_airc.air1_state = 2;
	g_status_airc.air2_state = 2;
	g_status_airc.air3_state = 2;
	g_status_airc.air4_state = 2;
	CFG_AIRC_t *p_cfg = APL_cfg_get();
	g_status_airc.thr_temp_1 = p_cfg->thr_temp_1;
	g_status_airc.thr_temp_2 = p_cfg->thr_temp_2;
	g_status_airc.thr_temp_3 = p_cfg->thr_temp_3;
	g_status_airc.thr_temp_4 = p_cfg->thr_temp_4;
	g_status_airc.thr_temp_delta = p_cfg->thr_temp_delta;
}
void APL_stt_controlBothAc(ac_data_t ac1,ac_data_t ac2) // Điều khiển 2 điều hòa
{

	s_needSendAcData[AIRC1_ID] = ac1;
	s_needSendAcData[AIRC2_ID] = ac2;
	samplecnt1 =0;
	samplecnt2 =0;
	air1TempAtSet = g_status_airc.temp_air_1;
	air2TempAtSet = g_status_airc.temp_air_2;

	air1_min = air1TempAtSet;
	air2_min = air2TempAtSet;

	air1_max = air1TempAtSet;
	air2_max = air2TempAtSet;

	previous1 = air1TempAtSet;
	previous2 = air2TempAtSet;

	time_samplingTemp[AIRC1_ID] = g_sysTime;
	time_samplingTemp[AIRC2_ID] = g_sysTime;
	indoorTempAtSet = g_status_airc.temp_indoor;

	osl_ctrlFullAc(AIRC1_ID, ac1); //Điều khiển điều hòa 1, cập nhật state = ac1
	g_status_airc.air1_sts = ac1.ac_powerModeCmd;
	osl_ctrlFullAc(AIRC2_ID, ac2); //Điều khiển điều hòa 2, cập nhật state = ac2
	g_status_airc.air2_sts = ac2.ac_powerModeCmd;

	higherTempCnt1=0;
	higherTempCnt2=0;
	lowerTempCnt1=0;
	lowerTempCnt2=0;



	// disable check ac status
	s_needCheckAcs[AIRC1_ID] = true;
	s_needCheckAcs[AIRC2_ID] = true;
	s_timeSendControlAcs[AIRC1_ID] = g_sysTime;
	s_timeSendControlAcs[AIRC2_ID] = g_sysTime;
	enterStateAuto_1 = enterStateAuto_2 = false;
	count_air1 = count_air2 = 0;
	if (s_needSendAcData[AIRC1_ID].ac_powerModeCmd == AC_POWER_ON)
	{
		setTemp[AIRC1_ID] = ac1.ac_temperatureCmd; //Lấy nhiệt độ cài đặt
		BSP_utils_startOperationTimeMeasurementById(AIRC1_ID);
	}
	else if (s_needSendAcData[AIRC1_ID].ac_powerModeCmd == AC_POWER_OFF)
	{
		setTemp[AIRC1_ID] = g_status_airc.temp_air_1; //Lấy nhiệt độ cửa gió thời điểm nhận lệnh OFF
		BSP_utils_stopOperationTimeMeasurementById(AIRC1_ID);
	}
	if (s_needSendAcData[AIRC2_ID].ac_powerModeCmd == AC_POWER_ON)
	{
		setTemp[AIRC2_ID] = ac2.ac_temperatureCmd; //Lấy nhiệt độ cài đặt
		BSP_utils_startOperationTimeMeasurementById(AIRC2_ID);
	}
	else if (s_needSendAcData[AIRC2_ID].ac_powerModeCmd == AC_POWER_OFF)
	{
		setTemp[AIRC2_ID] = g_status_airc.temp_air_2; //Lấy nhiệt độ cửa gió thời điểm nhận lệnh OFF
		BSP_utils_stopOperationTimeMeasurementById(AIRC2_ID);
	}
}


void APL_stt_processErrorAuto()
{
	enterModeErrorAuto = true; // mode loi auto
	s_acAlternate.isAlternate = false;
	_doCommand(AIRC1_ID, 4);
	_doCommand(AIRC2_ID, 4);
	osl_turn_fan(OFF);
	countErrorAuto_1 = countErrorAuto_2 = 0;
	timeCountErrorAuto_1 = timeCountErrorAuto_2 = g_sysTime;

	air1TempAtSet = g_status_airc.temp_air_1;
	air1_min = air1TempAtSet;
	air1_max = air1TempAtSet;
	air2TempAtSet = g_status_airc.temp_air_2;
	air2_min = air2TempAtSet;
	air2_max = air2TempAtSet;
	higherTempCnt1=0;
	higherTempCnt2=0;
	lowerTempCnt1=0;
	lowerTempCnt2=0;

	s_needSendAcData[AIRC1_ID].ac_powerModeCmd = AC_POWER_ON;
	s_needSendAcData[AIRC2_ID].ac_powerModeCmd = AC_POWER_ON;
}


void APL_stt_processRetry()
{
	static uint8_t processingAC = 1;
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();
	uint8_t acStatus;
	if (p_stt->mode_auto_en == MODE_NOMAL) // mode manual (dieu khien bang tay)
	{
		for (uint8_t i = 0; i < AC_NUM; i++)
		{
			if (elapsedTime(g_sysTime, timeStartNormal[i]) > 20000)
			{
				timeStartNormal[i] =  g_sysTime;
				if (i == AIRC1_ID)
				{
					AcStatus_getAcStatus(AIRC1_ID, &acStatus);
					if ((p_stt->air1_state == 1) || (p_stt->air1_state == 3) || (p_stt->air1_state == 4) || (p_stt->air1_state == 5))
					{
						if (acStatus != AC_POWER_ON)
						{
							error_air[AIRC1_ID] |= 0xFF00;
						}
						else
						{
							error_air[AIRC1_ID] &= ~0xFF00;
						}
					}
					else if (p_stt->air1_state == 2)
					{
						if (acStatus != AC_POWER_OFF)
						{
							error_air[AIRC1_ID] |= 0xFF00;
						}
						else
						{
							error_air[AIRC1_ID] &= ~0xFF00;
						}
					}
				}
				else if (i == AIRC2_ID)
				{
					AcStatus_getAcStatus(AIRC2_ID, &acStatus);
					if ((p_stt->air2_state == 1) || (p_stt->air2_state == 3) || (p_stt->air2_state == 4) || (p_stt->air2_state == 5))
					{
						if (acStatus != AC_POWER_ON)
						{
							error_air[AIRC2_ID] |= 0xFF00;
						}
						else
						{
							error_air[AIRC2_ID] &= ~0xFF00;
						}
					}
					else if (p_stt->air2_state == 2)
					{
						if (acStatus != AC_POWER_OFF)
						{
							error_air[AIRC2_ID] |= 0xFF00;
						}
						else
						{
							error_air[AIRC2_ID] &= ~0xFF00;
						}
					}
				}
			}
		}
	}
	else // mode auto
	{

		//sau khi nhận lệnh điều khiển 2 điều hòa APL_stt_controlBothAc, mỗi phút lấy mẫu nhiệt độ 1 lần
		if ((s_needCheckAcs[processingAC] //s_needCheckAcs set khi điều khiển 2 điều hòa APL_stt_controlBothAc
			&&  (elapsedTime(g_sysTime, time_samplingTemp[processingAC]) > (60*1000)))
				&& !enterModeErrorAuto)
		{
			time_samplingTemp[processingAC] = g_sysTime;
			if(processingAC == AIRC1_ID)
			{
				if(g_status_airc.temp_air_1 < air1_min)
				{
					air1_min = g_status_airc.temp_air_1;
				}
				if(g_status_airc.temp_air_1 > air1_max)
				{
					air1_max = g_status_airc.temp_air_1;
				}

				if(g_status_airc.temp_air_1>=previous1)
				{
					higherTempCnt1++;
				}
				else
				{
					higherTempCnt1=0;
				}
				previous1=g_status_airc.temp_air_1;
			}
			if(processingAC == AIRC2_ID)
			{
				if(g_status_airc.temp_air_2 < air2_min)
				{
					air2_min = g_status_airc.temp_air_2;
				}
				if(g_status_airc.temp_air_2 > air2_max)
				{
					air2_max = g_status_airc.temp_air_2;
				}

				if(g_status_airc.temp_air_2>=previous2)
				{
					higherTempCnt2++;
				}
				else
				{
					higherTempCnt2=0;
				}
				previous2=g_status_airc.temp_air_2;
			}
		}


		//sau khi nhận lệnh điều khiển 2 điều hòa APL_stt_controlBothAc, sau 20 phút kiểm tra nhiệt độ
		if ((s_needCheckAcs[processingAC] &&  (elapsedTime(g_sysTime, s_timeSendControlAcs[processingAC]) > (20*60*1000)))
				&& !enterModeErrorAuto)
		{

			Checkstt(processingAC, &acStatus);// Kiểm tra status theo nhiệt độ
			// Nếu điều khiển thành công
			if (acStatus == s_needSendAcData[processingAC].ac_powerModeCmd)
			{
				s_needCheckAcs[processingAC] = false;
				if (processingAC == AIRC1_ID)
				{
					enterStateAuto_1 = true;
				}
				else if (processingAC == AIRC2_ID)
				{
					enterStateAuto_2 = true;
				}
				if (enterStateAuto_1 && enterStateAuto_2)
				{
					timeStartAuto = g_sysTime;
					previous1 = g_status_airc.temp_air_1;
					previous2 = g_status_airc.temp_air_2;
					higherTempCnt1=0;
					higherTempCnt2=0;
					lowerTempCnt1=0;
					lowerTempCnt2=0;
				}
			}
			else // need retry
			{
				s_timeSendControlAcs[processingAC] = g_sysTime;
				if (processingAC == AIRC1_ID)
				{
					g_status_airc.air1_sts = acStatus; //Cập nhật status ON/OFF thực tế
					count_air1++;
					if (count_air1 >= 1)//không retry
					{
						error_air[AIRC1_ID] |= 0xFF00;
						s_needCheckAcs[processingAC] = false;
						APL_stt_processErrorAuto();

					}
					else //bật lại điều hòa lỗi
					{
						osl_ctrlFullAc(processingAC, s_needSendAcData[processingAC]);
						air1TempAtSet = g_status_airc.temp_air_1; //lấy nhiệt độ tại thời điểm điều khiển
						air1_min = air1TempAtSet;
						time_samplingTemp[AIRC1_ID] = g_sysTime;
					}
				}
				else if (processingAC == AIRC2_ID)
				{
					g_status_airc.air2_sts = acStatus; //Cập nhật status ON/OFF thực tế
					count_air2++;
					if (count_air2 >= 1)//không retry
					{
						error_air[AIRC2_ID] |= 0xFF00;
						s_needCheckAcs[processingAC] = false;
						APL_stt_processErrorAuto();
					}
					else
					{
						osl_ctrlFullAc(processingAC, s_needSendAcData[processingAC]);
						air2TempAtSet = g_status_airc.temp_air_2;//lấy nhiệt độ tại thời điểm điều khiển
						air2_min = air2TempAtSet;
						time_samplingTemp[AIRC2_ID] = g_sysTime;
					}
				}
			}
		}
		// next ac
		if (++ processingAC >= AC_NUM)
		{
			processingAC = 0;
		}

	}
}


void APL_stt_readSensors()
{
	Sensor_getADCvalue();
	g_status_airc.temp_air_1 = Sensor_getTempNTC(temp_w_1);
	g_status_airc.temp_air_2 = Sensor_getTempNTC(temp_w_2);
	g_status_airc.temp_air_3 = Sensor_getTempNTC(temp_w_3);
	g_status_airc.temp_air_4 = Sensor_getTempNTC(temp_w_4);
	g_status_airc.temp_outdoor = Sensor_getTempNTC(temp_outside);
	g_status_airc.air_current_1 = getMiliAmpe(AIRC1_ID);
	g_status_airc.air_current_2 = getMiliAmpe(AIRC2_ID);
	g_status_airc.air_current_3 = getMiliAmpe(AIRC3_ID);
	g_status_airc.air_current_4 = getMiliAmpe(AIRC4_ID);
	readSensorSHT30();
}

//Chế độ Điều hòa luân phiên
void APL_stt_processAcAlternate()
{
	if (enterModeErrorAuto) return;
	uint8_t currentRuntime;
	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
	if (s_acAlternate.isAlternate)
	{
		if (s_acAlternate.s_currentAcOn == AIRC1_ID)
		{
			currentRuntime = g_status_airc.acmRunTimeAirc1;
			if (currentRuntime >= p_cfg->acmRunTime_1)
			{
				s_acAlternate.s_currentAcOn = AIRC2_ID;
				APL_stt_setCurrentAlternateAcState(s_acAlternate.acData);
			}
		}
		else if (s_acAlternate.s_currentAcOn == AIRC2_ID)
		{
			currentRuntime = g_status_airc.acmRunTimeAirc2;
			if (currentRuntime >= p_cfg->acmRunTime_2)
			{
				s_acAlternate.s_currentAcOn = AIRC1_ID;
				APL_stt_setCurrentAlternateAcState(s_acAlternate.acData);
			}
		}
	}
}

void APL_stt_setCurrentAlternateAcState(ac_data_t acData)
{
	s_acAlternate.acData = acData;
	s_acAlternate.isAlternate = true;

	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();

	uint8_t currentRuntime;
	if (s_acAlternate.s_currentAcOn == AIRC1_ID)
	{
		currentRuntime = g_status_airc.acmRunTimeAirc1;
		if (currentRuntime < p_cfg->acmRunTime_1)
		{
			APL_stt_controlBothAc(s_acAlternate.acData,S_AC_DATA_OFF);
			return;
		}
	}
	else if (s_acAlternate.s_currentAcOn == AIRC2_ID)
	{
		currentRuntime = g_status_airc.acmRunTimeAirc2;
		if (currentRuntime < p_cfg->acmRunTime_2)
		{
			APL_stt_controlBothAc(S_AC_DATA_OFF,s_acAlternate.acData);
			return;
		}
	}
	BSP_utils_resetTime();
	s_acAlternate.s_currentAcOn = AIRC1_ID;
	APL_stt_controlBothAc(s_acAlternate.acData,S_AC_DATA_OFF);
}

void APL_stt_checkHumAndControl()
{
	//check humidity
	if (g_status_airc.humid_indoor >= (HUM_THRESHOLD))
	{
		s_humState = HUM_STATE_HIGH;
		// control AC
		APL_stt_setCurrentAlternateAcState(S_AC_DATA_Dry);
		osl_turn_fan(OFF);
	}
	else if (g_status_airc.humid_indoor < (HUM_THRESHOLD))
	{
		s_humState = HUM_STATE_LOW;
		// control AC
		APL_stt_controlBothAc(S_AC_DATA_OFF, S_AC_DATA_OFF);
		s_acAlternate.isAlternate = false;
		osl_turn_fan(ON);
	}
}

void APL_stt_processAutoAcInit()
{
	if (g_status_airc.temp_indoor > (g_status_airc.thr_temp_1))
	{
		s_autoState = AUTO_STATE_HIGH_TEMP;
		// control AC
		APL_stt_controlBothAc(S_AC_DATA_Enhance, S_AC_DATA_Enhance);
		s_acAlternate.isAlternate = false;
		osl_turn_fan(OFF);
	}
	else if (((g_status_airc.temp_indoor <= (g_status_airc.thr_temp_1)) &&
			 (g_status_airc.temp_indoor >= (g_status_airc.thr_temp_3))) || (((g_status_airc.temp_indoor < (g_status_airc.thr_temp_3))) && (g_status_airc.temp_outdoor >= g_status_airc.thr_temp_4)))
	{
		s_autoState = AUTO_STATE_MID_TEMP;
		// control AC
		APL_stt_setCurrentAlternateAcState(S_AC_DATA_Normal);
		osl_turn_fan(OFF);

	}
	else if (((g_status_airc.temp_indoor < (g_status_airc.thr_temp_3))) && (g_status_airc.temp_outdoor < g_status_airc.thr_temp_4))
	{
		s_autoState = AUTO_STATE_LOW_TEMP;
		// check control AC
		APL_stt_checkHumAndControl();
	}
}
//Chế độ auto, hoạt động theo thr_temp_1234
void APL_stt_processAutoAc()
{
	if (g_status_airc.mode_auto_en != MODE_AUTO)
		return;
	if (enterModeErrorAuto) return;
	if (g_status_airc.temp_indoor >= (g_status_airc.thr_temp_1 + 2) && //indoor >=32
		s_autoState != AUTO_STATE_HIGH_TEMP)
	{
		s_autoState = AUTO_STATE_HIGH_TEMP;
		// control AC
		APL_stt_controlBothAc(S_AC_DATA_Enhance,S_AC_DATA_Enhance);
		s_acAlternate.isAlternate = false;
		osl_turn_fan(OFF);
	}
	else if ((((g_status_airc.temp_indoor <= (g_status_airc.thr_temp_1 - 2)) && //24<=indoor<=28 || indoor<=20 && out>=20
			 (g_status_airc.temp_indoor >= (g_status_airc.thr_temp_3 + 2)))||
			((g_status_airc.temp_indoor <= (g_status_airc.thr_temp_3 - 2)) &&
					(g_status_airc.temp_outdoor >= (g_status_airc.thr_temp_4 + 2)))) &&
			 s_autoState != AUTO_STATE_MID_TEMP)
	{
		s_autoState = AUTO_STATE_MID_TEMP;
		// control AC
		APL_stt_setCurrentAlternateAcState(S_AC_DATA_Normal);
		osl_turn_fan(OFF);
	}
	else if ((g_status_airc.temp_indoor <= (g_status_airc.thr_temp_3 - 2))
			&& (g_status_airc.temp_outdoor <= (g_status_airc.thr_temp_4 - 2)) &&
			 s_autoState != AUTO_STATE_LOW_TEMP)
	{
		s_autoState = AUTO_STATE_LOW_TEMP;
		// check hum control AC
		APL_stt_checkHumAndControl();
	}
	else if (s_autoState == AUTO_STATE_LOW_TEMP)
	{
		//check humidity
		if (g_status_airc.humid_indoor > (HUM_THRESHOLD + HUM_DEBOUCH) &&
			s_humState != HUM_STATE_HIGH)
		{
			s_humState = HUM_STATE_HIGH;
			// control AC
			APL_stt_setCurrentAlternateAcState(S_AC_DATA_Dry);
			osl_turn_fan(OFF);
		}
		else if (g_status_airc.humid_indoor < (HUM_THRESHOLD - HUM_DEBOUCH) &&
				 s_humState != HUM_STATE_LOW)
		{
			s_humState = HUM_STATE_LOW;
			// control AC
			APL_stt_controlBothAc(S_AC_DATA_OFF, S_AC_DATA_OFF);
			s_acAlternate.isAlternate = false;
			osl_turn_fan(ON);
		}
	}
	// sau khi khởi động, nếu nhiệt độ nằm trong khoảng debouch, state vẫn là init, bỏ qua debouch.
	if(s_autoState == AUTO_STATE_INIT)
	{
		APL_stt_processAutoAcInit();
	}
}

void APL_stt_processAutoFan()
{
	if (g_status_airc.mode_auto_en != MODE_AUTO)
		return;
	if (g_status_airc.temp_indoor <= (g_status_airc.thr_temp_3) && (g_status_airc.temp_outdoor <= (g_status_airc.thr_temp_4 )))
	{
		osl_turn_fan(ON); TURN_ON_LED_FAN;
	}
	else
	{
		osl_turn_fan(OFF); TURN_OFF_LED_FAN;
	}
}
//Nhiệt độ cửa gió > T_threshold, sau 30p => Lỗi.
void APL_stt_checkErrorTemp()
{
	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
	if (enableCheckError_Air1)
	{
		if (g_status_airc.temp_air_1 < p_cfg->T_threshold)
		{
			timeTempbiggerThreshold_Air1 = g_sysTime;
		}
		if (elapsedTime(g_sysTime, timeTempbiggerThreshold_Air1) > 1000*60*20)//20 phút
		{
			error_air[AIRC1_ID] |= 0x00FF;
			if ((g_status_airc.mode_auto_en == MODE_AUTO) && (!enterModeErrorAuto))
			{
				APL_stt_processErrorAuto();
			}
		}
		else
		{
			error_air[AIRC1_ID] &= ~0x00FF;
		}
	}
	else
	{
		error_air[AIRC1_ID] &= ~0x00FF;
	}
	if (enableCheckError_Air2)
	{
		if (g_status_airc.temp_air_2 < p_cfg->T_threshold)
		{
			timeTempbiggerThreshold_Air2 = g_sysTime;
		}
		if (elapsedTime(g_sysTime, timeTempbiggerThreshold_Air2) > 1000*60*20)
		{
			error_air[AIRC2_ID] |= 0x00FF;
			if ((g_status_airc.mode_auto_en == MODE_AUTO) && (!enterModeErrorAuto))
			{
				APL_stt_processErrorAuto();
			}
		}
		else
		{
			error_air[AIRC2_ID] &= ~0x00FF;
		}
	}
	else
	{
		error_air[AIRC2_ID] &= ~0x00FF;
	}
}
void APL_stt_checkErrorSensor()
{
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();
	if (error_sensor & 0x310)
	{
		errorSensorAuto = true;
		if ((!enterModeErrorAuto) && (p_stt->mode_auto_en == MODE_AUTO))
		{
			APL_stt_processErrorAuto();
		}
	}
	else
	{
		errorSensorAuto = false;
	}
}

void APL_stt_checkErrorCurrentAuto() //xu ly dieu hoa loi, check trạng thái theo dòng điện
{
	uint8_t acStatus;
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();
	if (p_stt->mode_auto_en == MODE_AUTO) //che do auto
	{
		if (enterModeErrorAuto) //khi mode auto vao che do loi
		{
			if (elapsedTime(g_sysTime, timeCountErrorAuto_1) > 10000) //10s
			{
				AcStatus_getAcStatus(AIRC1_ID, &acStatus); //check trạng thái theo dòng điện
				if (acStatus != AC_POWER_ON) // Điều hòa tắt
				{
					if (countErrorAuto_1 <= 3) //gửi lệnh 4 3 lần
					{
						countErrorAuto_1++;
						_doCommand(AIRC1_ID, 4);
					}
					error_air[AIRC1_ID] |= 0xFF00; //Ghi lỗi AC1
				}
				else //Điều hòa bật
				{
					error_air[AIRC1_ID] &= ~0xFF00;// Xóa lỗi
					countErrorAuto_1 = 4;
				}
				timeCountErrorAuto_1 = g_sysTime;
			}

			if (elapsedTime(g_sysTime, timeCountErrorAuto_2) > 10000)
			{
				AcStatus_getAcStatus(AIRC2_ID, &acStatus);
				if (acStatus != AC_POWER_ON)
				{
					if (countErrorAuto_2 <= 3)
					{
						countErrorAuto_2++;
						_doCommand(AIRC2_ID, 4);
					}
					error_air[AIRC2_ID] |= 0xFF00;
				}
				else
				{
					error_air[AIRC2_ID] &= ~0xFF00;
					countErrorAuto_2 = 4;
				}
				timeCountErrorAuto_2 = g_sysTime;
			}
		}
		else //không trong ModeError
		{
			if (enterStateAuto_1 && enterStateAuto_2) // khi da on/off 2 dieu hoa thanh cong
			{
	            if (elapsedTime(g_sysTime, timeStartAuto) > 60000)//1 phút check 1 lần
	            {
	                timeStartAuto = g_sysTime;
	                AcStatus_getAcStatus(AIRC1_ID, &acStatus); //lấy state
	                if (s_needSendAcData[AIRC1_ID].ac_powerModeCmd != acStatus)//status != lệnh điều khien
	                {
	                    error_air[AIRC1_ID] |= 0xFF00;
	                    APL_stt_processErrorAuto();
	                }
	                AcStatus_getAcStatus(AIRC2_ID, &acStatus);
	                if (s_needSendAcData[AIRC2_ID].ac_powerModeCmd != acStatus)
	                {
	                    error_air[AIRC2_ID] |= 0xFF00;
						APL_stt_processErrorAuto();
	                }
	            }
			}
		}
	}
}
void APL_stt_clearErrorAuto()
{
	AIRC_STATUS_t *p_stt = NULL;
	p_stt = APL_stt_get();
	//Trong mode Auto, đang ModeError, sensor ko lỗi, 2 điều hòa ko lỗi => reset lỗi
	if ((p_stt->mode_auto_en == MODE_AUTO) && (enterModeErrorAuto && !errorSensorAuto)
			&& !error_air[AIRC1_ID] && !error_air[AIRC2_ID])
	{
		APL_stt_processAutoAcInit();
		enterModeErrorAuto = false;
	}
}
void APL_stt_manager(void)//3s check 1 lần
{
//	static uint8_t readSensorCnt = 20;
////	 read sensor every 1 min
//	if (readSensorCnt ++ > 20)
//	{
//		readSensorCnt = 0;
		APL_stt_readSensors();
		APL_stt_checkErrorSensor();
//	}
	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
// Update thresholds:
	if (g_status_airc.mode_auto_en != p_cfg->acmControlAuto && p_cfg->acmControlAuto == MODE_AUTO)
	{
		g_status_airc.thr_temp_1 = p_cfg->thr_temp_1;
		g_status_airc.thr_temp_2 = p_cfg->thr_temp_2;
		g_status_airc.thr_temp_3 = p_cfg->thr_temp_3;
		g_status_airc.thr_temp_4 = p_cfg->thr_temp_4;
		g_status_airc.thr_temp_delta = p_cfg->thr_temp_delta;
		g_status_airc.mode_auto_en = p_cfg->acmControlAuto;
		APL_cfg_Store();
		if (errorSensorAuto)
		{
			APL_stt_processErrorAuto();
		}
		else
		{
			enterModeErrorAuto = false;
			APL_stt_processAutoAcInit();
		}
	}
	else if (g_status_airc.mode_auto_en == p_cfg->acmControlAuto && p_cfg->acmControlAuto == MODE_AUTO)
	{
		if ((g_status_airc.thr_temp_1 == p_cfg->thr_temp_1) && (g_status_airc.thr_temp_2 == p_cfg->thr_temp_2)
				&& (g_status_airc.thr_temp_3 == p_cfg->thr_temp_3) && (g_status_airc.thr_temp_4 == p_cfg->thr_temp_4))
		{}
		else
		{
			g_status_airc.thr_temp_1 = p_cfg->thr_temp_1;
			g_status_airc.thr_temp_2 = p_cfg->thr_temp_2;
			g_status_airc.thr_temp_3 = p_cfg->thr_temp_3;
			g_status_airc.thr_temp_4 = p_cfg->thr_temp_4;
			g_status_airc.thr_temp_delta = p_cfg->thr_temp_delta;
			APL_cfg_Store();
			if (enterModeErrorAuto)
			{
				APL_stt_processErrorAuto();
			}
			else
			{
				APL_stt_processAutoAcInit();
			}
		}
	}
	//Manual Mode
	else if (g_status_airc.mode_auto_en != p_cfg->acmControlAuto && p_cfg->acmControlAuto == MODE_NOMAL)
	{
		g_status_airc.mode_auto_en = p_cfg->acmControlAuto;
		APL_cfg_Store();
		timeStartNormal[0] = timeStartNormal[1] = g_sysTime;
		s_acAlternate.isAlternate =  false;
		s_needCheckAcs[AIRC1_ID] = false;
		s_needCheckAcs[AIRC2_ID] = false;
	}
	else if (g_status_airc.mode_auto_en == p_cfg->acmControlAuto && p_cfg->acmControlAuto == MODE_NOMAL)
	{
		if ((g_status_airc.thr_temp_1 == p_cfg->thr_temp_1) && (g_status_airc.thr_temp_2 == p_cfg->thr_temp_2)
				&& (g_status_airc.thr_temp_3 == p_cfg->thr_temp_3) && (g_status_airc.thr_temp_4 == p_cfg->thr_temp_4))
		{}
		else
		{
			g_status_airc.thr_temp_1 = p_cfg->thr_temp_1;
			g_status_airc.thr_temp_2 = p_cfg->thr_temp_2;
			g_status_airc.thr_temp_3 = p_cfg->thr_temp_3;
			g_status_airc.thr_temp_4 = p_cfg->thr_temp_4;
			g_status_airc.thr_temp_delta = p_cfg->thr_temp_delta;
			APL_cfg_Store();
		}
	}
	// Update operation time for devices

	BSP_utils_updateOperationTimeAll();
	g_status_airc.acmRunTimeAirc1 = BSP_utils_getOperationHourById(AIRC1_ID);
	g_status_airc.acmRunTimeAirc2 = BSP_utils_getOperationHourById(AIRC2_ID);
	g_status_airc.acmRunTimeFan = BSP_utils_getOperationHourById(FAN_ID);

//	APL_stt_checkErrorCurrentAuto(); // check theo dong dien
//	APL_stt_checkErrorTemp(); // 20 kiểm tra nhiệt độ >T_Threshold
	CheckError();
	APL_stt_clearErrorAuto(); // xoa mode loi
	APL_stt_processRetry(); // retry dieu hoa 3 lan den khi thanh cong dua vao dong dien, neu sau 3 lan khong thanh cong thi chuyển mode auto error (bat 2 dieu hoa lenh 4)
	APL_stt_processAutoAc(); // Dieu khien dieu hoa theo mode AUTO
	APL_stt_processAcAlternate(); //Dieu khien theo che do luan phien
}
AIRC_STATUS_t *APL_stt_get(void)
{
	return &g_status_airc;
}
