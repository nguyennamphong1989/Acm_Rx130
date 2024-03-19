/*
 * sensor.h
 *
 *  Created on: Aug 5, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_SENSOR_SENSOR_H_
#define APPS_SENSOR_SENSOR_H_
#include "Config_S12AD0.h"
#include "stdint.h"
void processCurrent();
#define NUM_CHANNEL_USER_ADC 9
typedef enum
{
	CH_IDX_SENSOR_Biendong_4 = 0,
	CH_IDX_SENSOR_Biendong_3,
	CH_IDX_SENSOR_Biendong_2,
	CH_IDX_SENSOR_Biendong_1,
	CH_IDX_SENSOR_Temp_ouside,
	CH_IDX_SENSOR_temp_in_w_4,
	CH_IDX_SENSOR_temp_in_w_3,
	CH_IDX_SENSOR_temp_in_w_2,
	CH_IDX_SENSOR_temp_in_w_1
} ch_idx_sensor_t;
typedef enum
{
	temp_w_1 = 0,
	temp_w_2,
	temp_w_3,
	temp_w_4,
	temp_outside
} temp_idx_t ;
#define  NUMBER_TEMP_NTC		5
void Sensor_getADCvalue();
float Sensor_getTempNTC(temp_idx_t temp_idx);
uint32_t Sensor_getResistor(ch_idx_sensor_t ch_idx_ntc);
uint16_t getMiliAmpe(uint8_t acIndex);
int8_t AcStatus_getAcStatus(uint8_t acIndex, uint8_t *acStatus);
#endif /* APPS_SENSOR_SENSOR_H_ */
