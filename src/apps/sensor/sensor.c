/*
 * sensor.c
 *
 *  Created on: Aug 5, 2022
 *      Author: Lover_boy_9x
 */

#include "sensor.h"
#include "Config_S12AD0.h"
#include "stdint.h"
#include "stdio.h"
#include "apl_status.h"
#include "apl_config.h"
uint16_t error_sensor;
ad_channel_t adcChannel[NUM_CHANNEL_USER_ADC] = {
	ADCHANNEL1,
	ADCHANNEL2,
	ADCHANNEL3,
	ADCHANNEL4,
	ADCHANNEL5,
	ADCHANNEL7,
	ADCHANNEL19, 
	ADCHANNEL20, 
	ADCHANNEL21
};
uint16_t adcValue[NUM_CHANNEL_USER_ADC] = {0};
uint32_t Res2 = 10000;
extern volatile uint8_t g_adc_flag;
uint16_t tableTempRes[100][2] =
{
    {0,  32740}, {1,  31110}, {2, 29580},  {3,  28120}, {4, 26750},
    {5,  25450}, {6,  24220}, {7, 23060},  {8,  21960}, {9, 20920},
    {10, 19940}, {11, 19000}, {12, 18120}, {13, 17280},{14, 16490},
    {15, 15730}, {16, 15020}, {17, 14340}, {18, 13690},{19, 13080},
    {20, 12500}, {21, 11950}, {22, 11420}, {23, 10930},{24, 10450},
    {25, 10000}, {26, 9570},  {27, 9160},  {28, 8770}, {29, 8400},
    {30, 8050},  {31, 7720},  {32, 7400},  {33, 7090}, {34, 6800},
    {35, 6520},  {36, 6260},  {37, 6010},  {38, 5760}, {39, 5530},
    {40, 5320},  {41, 5110},  {42, 4910},  {43, 4710}, {44, 4530},
    {45, 4360},  {46, 4190},  {47, 4030},  {48, 3880}, {49, 3730},
    {50, 3590},  {51, 3460},  {52, 3330},  {53, 3200}, {54, 3090},
    {55, 2970},  {56, 2870},  {57, 2760},  {58, 2660}, {59, 2570},
    {60, 2480},  {61, 2390},  {62, 2300},  {63, 2220}, {64, 2150},
    {65, 2070},  {66, 2000},  {67, 1930},  {68, 1870}, {69, 1800},
    {70, 1740},  {71, 1680},  {72, 1630},  {73, 1570}, {74, 1520},
    {75, 1470},  {76, 1420},  {77, 1380},  {78, 1330}, {79, 1290},
    {80, 1250},  {81, 1210},  {82, 1170},  {83, 1130}, {84, 1100},
    {85, 1060},  {86, 1030},  {87, 1000},  {88,  970}, {89,  940},
    {90,  910},  {91,  880},  {92,  850},  {93,  830}, {94,  800},
    {95,  780},  {96,  760},  {97,  740},  {98,  710}, {99,  690},
};
uint16_t Res_inmin;
uint16_t Res_inmax;
uint32_t Res1;
uint32_t Res3;
uint16_t Temp_outmin;
uint16_t Temp_outmax;
float temp_ntc[NUMBER_TEMP_NTC];
ch_idx_sensor_t array_idx_sensor[NUMBER_TEMP_NTC] = {
	CH_IDX_SENSOR_temp_in_w_1, 
	CH_IDX_SENSOR_temp_in_w_2,
	CH_IDX_SENSOR_temp_in_w_3, 
	CH_IDX_SENSOR_temp_in_w_4, 
	CH_IDX_SENSOR_Temp_ouside 
};


float map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    return (float)((x - in_min)*(out_max - out_min)/(in_max - in_min)+out_min);
}


void Sensor_getADCvalue()
{
	g_adc_flag = 0U;
	R_Config_S12AD0_Start();
	while (0U == g_adc_flag) {};
	for(uint8_t i = 0; i < NUM_CHANNEL_USER_ADC; i++)
	{
		R_Config_S12AD0_Get_ValueResult(adcChannel[i],&adcValue[i]);
	}
}


uint32_t Sensor_getResistor(ch_idx_sensor_t ch_idx_ntc)
{
    //	luu y phep chia cho 0, tuy cpu co xu li phep chia cho 0 hay k, nhu msp430 hien tai chia cho 0 tu tra ve FFFFFFFF
    //	tuy nhien nen tu kiem tra de tranh loi
    if((uint32_t)adcValue[ch_idx_ntc]  == 0)
    {
        return 0xFFFFFFFF; //return bao nhieu thi tuy ung dung, o day retrun FFFFFFFF de tinh ra nhiet do = 0
    }
    return ((Res2*4095)/(uint32_t)adcValue[ch_idx_ntc]) - Res2;
}


float Sensor_getTempNTC(temp_idx_t temp_idx)
{
	uint8_t j;
	if((uint16_t)Sensor_getResistor(array_idx_sensor[temp_idx]) <= tableTempRes[99][1])
	{
		error_sensor |= (1<<temp_idx);
		return  tableTempRes[99][0];
	}
	else
	{
		error_sensor &= ~(1<<temp_idx);
	}
	for(j = 0; j < 100; j++)
	{
		if(j == 0)
		{
			if(Sensor_getResistor(array_idx_sensor[temp_idx]) >= tableTempRes[j][1])
			{
				temp_ntc[temp_idx] = 0;
				error_sensor |= (1<<temp_idx);
				break;
			}
		}
		else
		{
			if(Sensor_getResistor(array_idx_sensor[temp_idx]) >= tableTempRes[j][1])
			{
				Res1 = Sensor_getResistor(array_idx_sensor[temp_idx]);
				Res_inmin = tableTempRes[j][1];
				Res_inmax = tableTempRes[j-1][1];
				Temp_outmin = tableTempRes[j][0];
				Temp_outmax = tableTempRes[j-1][0];
				temp_ntc[temp_idx] = map(Res1,Res_inmin,Res_inmax,Temp_outmin,Temp_outmax);
				break;
			}
		}
	}
	return temp_ntc[temp_idx];
}


uint8_t ADC_index_current[4] = {CH_IDX_SENSOR_Biendong_1, CH_IDX_SENSOR_Biendong_2, CH_IDX_SENSOR_Biendong_3, CH_IDX_SENSOR_Biendong_4};

//uint16_t calibTableCurrent_4[2][10] = {
//		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
//		{0, 608, 804, 1046, 1755, 2236, 2375, 2437, 2508, 2555}
//
//};
//uint16_t calibTableCurrent_3[2][10] = {
//		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
//		{0, 660, 866, 1090, 1793, 2252, 2388, 2450, 2515, 2565}
//
//};
//uint16_t calibTableCurrent_2[2][10] = {
//		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
//		{0, 574, 750, 950, 1720, 2222, 2368, 2430, 2499, 2555}
//
//};
//uint16_t calibTableCurrent_1[2][10] = {
//		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
//		{0, 620, 804, 1020, 1756, 2236, 2376, 2443, 2510, 2553}
//
//};
uint16_t calibTableCurrent_4[2][10] = {
		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
		{0, 615, 806, 1026, 1755, 2236, 2377, 2440, 2508, 2557}

};
uint16_t calibTableCurrent_3[2][10] = {
		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
		{0, 615, 806, 1026, 1755, 2236, 2377, 2440, 2508, 2557}

};
uint16_t calibTableCurrent_2[2][10] = {
		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
		{0, 615, 806, 1026, 1755, 2236, 2377, 2440, 2508, 2557}

};
uint16_t calibTableCurrent_1[2][10] = {
		{0, 180, 220, 250, 410, 640, 850, 1000, 1210, 1410},
		{0, 615, 806, 1026, 1755, 2236, 2377, 2440, 2508, 2557}

};

int32_t util_interpolateInt(int32_t y1, int32_t x1, int32_t y2, int32_t x2, int32_t x)
{
    if(x2 == x1) return 0;
    return ((y2- y1)*(x - x1)/(x2 - x1) + y1); //return y
}
uint16_t vol;
uint16_t currentTable[2][10] = {0};
uint32_t adcValueCurrent[4];
uint16_t adcValueCurrent_TB[4];
void processCurrent()
{
	static uint8_t countCurrent = 0;
	uint8_t i = 0;
	Sensor_getADCvalue();
	for (i = 0; i < 4; i++)
	{
		adcValueCurrent[i] += adcValue[ADC_index_current[i]];
	}
	countCurrent++;
	if (countCurrent == 20)
	{
		countCurrent = 0;
		for (i = 0; i < 4; i++)
		{
			adcValueCurrent_TB[i] = adcValueCurrent[i]/20;
			adcValueCurrent[i] = 0;
		}
	}
}
uint16_t getMiliAmpe(uint8_t acIndex)
{
	vol = adcValueCurrent_TB[acIndex];

	if (acIndex == AIRC1_ID) memcpy(currentTable, calibTableCurrent_1, sizeof(currentTable));
	else if(acIndex == AIRC2_ID) memcpy(currentTable, calibTableCurrent_2, sizeof(currentTable));
	else if(acIndex == AIRC3_ID) memcpy(currentTable, calibTableCurrent_3, sizeof(currentTable));
	else if(acIndex == AIRC4_ID) memcpy(currentTable, calibTableCurrent_4, sizeof(currentTable));
	else {return 0;}
	if(vol <= currentTable[1][0])
	{
		return currentTable[0][0];
	}
	if(vol >= currentTable[1][9])
	{
		return currentTable[0][9];
	}
	for(uint8_t i = 0; i < 9; i++)
	{
		if(vol < currentTable[1][i+1] && vol >= currentTable[1][i])
		{
			return util_interpolateInt(currentTable[0][i], currentTable[1][i],currentTable[0][i+1],currentTable[1][i+1], vol);
		}
	}
	return 0;
}
int8_t AcStatus_getAcStatus(uint8_t acIndex, uint8_t *acStatus)
{
	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
	AIRC_STATUS_t *p_status = APL_stt_get();
    if (acIndex > AIRC4_ID)
    {
        return -1;
    }
    uint16_t currentLocal;
    if (acIndex == AIRC1_ID) currentLocal = p_status->air_current_1;
    else if (acIndex == AIRC2_ID) currentLocal = p_status->air_current_2;
    else if (acIndex == AIRC3_ID) currentLocal = p_status->air_current_3;
    else if (acIndex == AIRC4_ID) currentLocal = p_status->air_current_4;
    if (currentLocal > p_cfg->minCurrent)
    {
        *acStatus = AC_POWER_ON;
    }
    else
    {
        *acStatus = AC_POWER_OFF;
    }
    return 0;
}
