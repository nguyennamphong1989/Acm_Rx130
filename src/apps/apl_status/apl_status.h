/*
 * apl_status.h
 *
 *  Created on: Aug 1, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_API_STATUS_API_STATUS_H_
#define APPS_API_STATUS_API_STATUS_H_
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "r_cg_macrodriver.h"
#define AIRC1_ID								0
#define AIRC2_ID								1
#define AIRC3_ID								2
#define AIRC4_ID								3
#define AC_NUM									2
#define FAN_ID									4
#define MODE_NOMAL										0U
#define MODE_AUTO										1U
typedef struct {
    uint16_t temp_indoor;
    uint16_t temp_outdoor;
    uint16_t humid_indoor;
    uint8_t air1_state;		// airc1_state theo lệnh điều khiển
    uint8_t air2_state;		// airc2_state
    uint8_t air3_state;
    uint8_t air4_state;
	uint8_t fan_command;		// fan_state
	uint8_t thr_temp_1;
	uint8_t thr_temp_2;
	uint8_t thr_temp_3;
	uint8_t thr_temp_4;
	uint8_t thr_temp_delta;
	uint16_t temp_air_1;	//nhiệt độ cửa gió
	uint16_t temp_air_2;
	uint16_t temp_air_3;
	uint16_t temp_air_4;
  	uint8_t mode_auto_en;
    uint8_t hour;
    uint8_t minute;
    uint8_t acmRunTimeAirc1;	// Thời gian điều hòa 1 chạy
    uint8_t acmRunTimeAirc2;	// Thời gian điều hòa 2 chạy
    uint8_t acmRunTimeAirc3;
    uint8_t acmRunTimeAirc4;
    uint16_t acmRunTimeFan;	  // Thời gian quạt chạy
    uint16_t air_current_1;
    uint16_t air_current_2;
    uint16_t air_current_3;
    uint16_t air_current_4;

    uint16_t air1_sts; // Lưu trạng thái thực tế của điều hòa
    uint16_t air2_sts;
    uint16_t air3_sts;
    uint16_t air4_sts;



} AIRC_STATUS_t;     // 14 x 2 bytes

enum {
   OFF = 0U, ON
};

typedef enum
{
	AUTO_STATE_INIT,  // trạng thái khởi tạo
	AUTO_STATE_LOW_TEMP, // trạng thái nhiệt độ thấp
	AUTO_STATE_MID_TEMP, // trạng thái nhiệt độ trung bình
	AUTO_STATE_HIGH_TEMP // trạng thái nhiệt độ cao
} autoState_t;

typedef struct
{
    uint8_t ac_powerModeCmd;
    uint8_t ac_operatingModeCmd;
    uint8_t ac_fanSpeedCmd;
    uint8_t ac_temperatureCmd;
} ac_data_t;

enum {
    AC_POWER_OFF = 0,
    AC_POWER_ON = 1,
};

enum {
    AC_FAN_AUTO = 0,
    AC_FAN_1 = 1,
    AC_FAN_2 = 2,
    AC_FAN_3 = 3,
    AC_FAN_4 = 4,
    AC_FAN_5 = 5
};

enum {
    AC_MODE_AUTO = 1,
    AC_MODE_HEAT = 2,
    AC_MODE_COOL = 3,
    AC_MODE_DRY = 4,
    AC_MODE_FAN_ONLY = 5,
    AC_MODE_MAINT = 6
};

typedef enum
{
	HUM_STATE_INIT,
	HUM_STATE_LOW,
	HUM_STATE_HIGH
} humState_t;

typedef struct
{
	bool isAlternate;
	uint8_t s_currentAcOn;
	ac_data_t acData;
} alternateAC_t;

void APL_stt_init(void);
void APL_stt_readSensors();
//void APL_stt_error_state_update(uint8_t err1_state, uint8_t err2_state);
void APL_stt_manager(void);
AIRC_STATUS_t *APL_stt_get(void);
#endif /* APPS_API_STATUS_API_STATUS_H_ */
