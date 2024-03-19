
/******************************************************************************
*
* Embedded software team.
* (c) Copyright 2018.
* ALL RIGHTS RESERVED.
*
***************************************************************************/
/**
 *
 * @file         timeCheck.h
 *
 * @author    	quanvu
 *
 * @version   1.0
 *
 * @date
 *
 * @brief     Brief description of the file
 *
 * Detailed Description of the file. If not used, remove the separator above.
 *
 */

#ifndef APPS_TIMER_TIMECHECK_H_
#define APPS_TIMER_TIMECHECK_H_


/******************************************************************************
* Includes
******************************************************************************/


#include "stdbool.h"
#include "stdint.h"
#define MAX_DEVICE_NUMBER               (5)
// struct for utils
typedef struct {
	uint16_t hours;
	uint8_t minutes;
	uint8_t seconds;
} t_operation_time;

typedef struct{
    uint8_t operateStatus;
    uint32_t counter;
    t_operation_time operTime;
} t_operation_unit_manage;

/******************************************************************************
* Constants
******************************************************************************/



/******************************************************************************
* Macros
******************************************************************************/



/******************************************************************************
* Types
******************************************************************************/


/**
 * @brief Use brief, otherwise the index won't have a brief explanation.
 *
 * Detailed explanation.
 */




/******************************************************************************
* Global variables
******************************************************************************/


/******************************************************************************
* Global functions
******************************************************************************/
uint32_t elapsedTime(uint32_t newTime,uint32_t oldTime);
bool timeIsAfter(uint32_t newTime,uint32_t oldTime);
void Utility_generateRamdomStr(char* str, uint8_t len, int rand_seed);
void tools_softDelay(uint32_t ms);
/******************************************************************************
* Inline functions
******************************************************************************/
// Utils
void BSP_utils_init(void);
void BSP_utils_updateCounterForDevices(void);
void BSP_utils_startOperationTimeMeasurementById(uint8_t id);
void BSP_utils_stopOperationTimeMeasurementById(uint8_t id);
void BSP_utils_updateOperationTimeAll(void);
void BSP_utils_getOperationTimeById(uint8_t id, t_operation_time *time);
uint16_t BSP_utils_getOperationHourById(uint8_t id);
uint16_t BSP_utils_getOperationMinuteById(uint8_t id);

int8_t BSP_utils_getOperationSttById(uint8_t id);

void BSP_utils_resetTime();


#endif /* APPS_TIMER_TIMECHECK_H_ */
