/*
 * api_config.h
 *
 *  Created on: Jul 28, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_APL_CONFIG_API_CONFIG_H_
#define APPS_APL_CONFIG_API_CONFIG_H_


#include "stdint.h"
// ADC Threshold ranges:
#define AIRC_THR_TEMP_1_DEFALT					35
#define AIRC_THR_TEMP_2_DEFALT					30
#define AIRC_THR_TEMP_3_DEFALT		    		26
#define AIRC_THR_TEMP_4_DEFALT			    	18
#define	AIRC_AUTO_ENABLE							1
#define	THR_TEMP_DELTA_DEFALT						2
// Temp max/min
#define AIRC_TEMP_MAX						99
#define AIRC_TEMP_MIN						0
//////////////////////////////////////TYPES////////////////////////////////////
/* All structure fields are aligned by 1-byte */

typedef struct {

  uint8_t acmControlAuto;	        // Trạng thái chế độ chạy tự động trên 7688
  uint16_t acmAlternativeTime;	  // Thời gian chạy luân phiên mỗi điều hoà (theo giờ)
  uint16_t acmRunTime_1;	          // Thời gian bật điều hòa (theo giờ trong ngày)
  uint16_t acmRunTime_2;
  uint16_t acmRestTime;	        // Thời gian tắt điều hòa
  uint8_t acmGenAllow;          //	Lựa chọn chạy điều hòa bằng điện máy phát hoặc không
  uint16_t acmVacThreshold;      // Ngưỡng điện áp tối thiểu cho phép chạy điều hòa
  uint8_t acmMinHumid;          // Ngưỡng độ ẩm tối thiểu
  uint8_t acmMaxHumid;          // Ngưỡng độ ẩm tối đa
  uint8_t acmExpectedTemp;      // Nhiệt độ mong muốn
  uint8_t acmExpectedHumid;     // Độ ẩm mong muốn

  uint8_t thr_temp_1;  // threshold to run air 1
  uint8_t thr_temp_2;  // threshold to run air 2
  uint8_t thr_temp_3;  // threshold to turn off air 1
  uint8_t thr_temp_4;  // threshold to turn off air 2
  uint8_t thr_temp_delta;
  uint8_t airc_count;
  uint16_t minCurrent;
  uint8_t T_threshold;
} CFG_AIRC_t;  // 20 bytes

////////////////////////////////////PROTOTYPES/////////////////////////////////
void APL_cfg_init(void);
//void plusOne();
//void APL_cfg_factory_reset(void);
CFG_AIRC_t *APL_cfg_get(void);
//void APL_cfg_audo_mode_update(uint8_t enable);
void APL_cfg_Store();
//void APL_cfg_thresholds_temp_update(uint8_t thr_temp_1, uint8_t thr_temp_2, uint8_t thr_temp_3, uint8_t thr_temp_4,uint8_t thr_temp_delta, uint8_t airc_count);


#endif /* APPS_APL_CONFIG_API_CONFIG_H_ */
