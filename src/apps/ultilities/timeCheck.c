/******************************************************************************
*
* Embedded software team.
* (c) Copyright 2018.
* ALL RIGHTS RESERVED.
*
***************************************************************************//*!
*
* @file        timeCheck.c
*
* @author    quanvu
*
* @version   1.0
*
* @date
*
* @brief
*
*******************************************************************************
*
* Detailed Description of the file. If not used, remove the separator above.
*
******************************************************************************/

#include "timeCheck.h"
#include "stdlib.h"
#include "apl_status.h"
extern volatile uint32_t g_sysTime;
/******************************************************************************
* External objects
******************************************************************************/


/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/



/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/


/******************************************************************************
* Local variables
******************************************************************************/



/******************************************************************************
* Local functions
******************************************************************************/


/******************************************************************************
* Global functions
******************************************************************************/

/**
 * @brief One line documentation
 *
 * A more detailed documentation
 *
 * @param arg1 the first function argument
 * @param arg2 the second function argument
 *
 * @return descrition for the function return value
 */


uint32_t elapsedTime(uint32_t newTime,uint32_t oldTime)
{
	if(newTime >= oldTime)
	{
		return (newTime - oldTime);
	}else
	{
		return (newTime + (0xffffffff - oldTime +1));
	}
}

bool timeIsAfter(uint32_t newTime,uint32_t oldTime)
{
	return (elapsedTime(newTime , oldTime) < 100000)? true:false;
}


void Utility_generateRamdomStr(char* str, uint8_t len, int rand_seed)
{
	srand(rand_seed);
	for (uint8_t i = 0; i < len; i++)
	{
		str[i] = 'A' + (rand() % 26);
	}
	str[len] = '\0';
}

extern volatile uint32_t g_sysTime;
void tools_softDelay(uint32_t ms)
{
	uint32_t lastTime = g_sysTime;
	while(elapsedTime(g_sysTime, lastTime)<ms);
}
/**
 * @brief One line documentation
 * interrup every 1 ms
 * A more detailed documentation
 *
 * @param arg1 the first function argument
 * @param arg2 the second function argument
 *
 * @return descrition for the function return value
 */

// group of mannagement time by order AIRC1, AIRC2, FAN
t_operation_unit_manage operationTimeManagement[MAX_DEVICE_NUMBER];

//======================== Private functions =========================
/* @brief: Update operation time by counter
 *
 */
static t_operation_time secToTime(uint32_t count) {
    t_operation_time time;
    time.hours = count / 3600;
    time.minutes = (count % 3600) / 60;
    time.seconds = count % 60;

    return time;
}

/* @brief: Update operation time by counter
 *
 */
static void updateOperationTime(t_operation_time *currTime, uint32_t *count) {
    // check input parameter
	if (!currTime || !count) {
        return;
    }

    // convert count to time format
    t_operation_time tmpTime = secToTime(*count);

    // update operation time
    currTime->hours += tmpTime.hours;
    currTime->minutes += tmpTime.minutes;
    currTime->seconds += tmpTime.seconds;

    // calibration time
    if((currTime->seconds / 60) != 0) {
        currTime->minutes += (currTime->seconds / 60);
        currTime->seconds = currTime->seconds % 60;
    }
    if((currTime->minutes / 60) != 0) {
        currTime->hours += (currTime->minutes / 60);
        currTime->minutes = currTime->minutes % 60;
    }
#ifdef _DB_TIME_
    printf("\r\n Time is %d:%d:%d", currTime->hours, currTime->minutes, currTime->seconds);
#endif
    // clear counter
    *count = 0;
}


/* @brief: Reset operation time to default 00h:00m:00s
 *
 */
void resetOperationTimeById(uint8_t id) {
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return;
    }
    // reset operation time
    operationTimeManagement[id].operTime.hours = 0;
    operationTimeManagement[id].operTime.minutes = 0;
    operationTimeManagement[id].operTime.seconds = 0;
}

/* @brief: Update operation time by id
 *
 */
void updateOperationTimeById(uint8_t id) {
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return;
    }

    // Update operation time
    updateOperationTime(&operationTimeManagement[id].operTime,
                        &operationTimeManagement[id].counter);
}

//========================== Public functions ==================================
/* @brief: Initialize for operation time management
 *
 */
void BSP_utils_init() {
    for (uint8_t i = 0; i < MAX_DEVICE_NUMBER; i++) {
        operationTimeManagement[i].counter = 0;
        operationTimeManagement[i].operateStatus = 0;
        operationTimeManagement[i].operTime.hours = 0;
        operationTimeManagement[i].operTime.minutes = 0;
        operationTimeManagement[i].operTime.seconds = 0;
    }
}
//========================== Public functions ==================================
/* @brief: Reset for operation time management
 *
 */
void BSP_utils_resetTime()
{
	for (uint8_t i = 0; i < MAX_DEVICE_NUMBER; i++) {
		operationTimeManagement[i].counter = 0;
	    resetOperationTimeById(i);
	}
}
/**
  * @brief  update counter for measurement devices
  * @param  None
  * @retval None
  */
void BSP_utils_updateCounterForDevices()
{
    for (uint8_t i = 0; i < MAX_DEVICE_NUMBER; i++) {
        if(operationTimeManagement[i].operateStatus == 1U) {
            operationTimeManagement[i].counter ++;
            // operationTimeManagement[i].counter += 1000; // test
        }
    }
}


bool enableCheckError_Air1, enableCheckError_Air2 = false;
uint32_t timeTempbiggerThreshold_Air1, timeTempbiggerThreshold_Air2;


/* @brief: start time measurement
 *
 */
void BSP_utils_startOperationTimeMeasurementById(uint8_t id) {
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return;
    }

    // enable measurement
    if (operationTimeManagement[id].operateStatus == 0U)
    {
		if (id == AIRC1_ID)
		{
			enableCheckError_Air1 = true;
			timeTempbiggerThreshold_Air1 = g_sysTime;
		}
		else if (id == AIRC2_ID)
		{
			enableCheckError_Air2 = true;
			timeTempbiggerThreshold_Air2 = g_sysTime;
		}
    }
	
    operationTimeManagement[id].operateStatus = 1U;
}


/* @brief: stop time measurement
 *
 */
void BSP_utils_stopOperationTimeMeasurementById(uint8_t id) {
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return;
    }
    operationTimeManagement[id].operateStatus = 0U;
    resetOperationTimeById(id);
    if (id == AIRC1_ID)
    {
    	enableCheckError_Air1 = false;
    }

    else if (id == AIRC2_ID)
    {
    	enableCheckError_Air2 = false;
    }
}

/* @brief: get opertation status of device by id
 *
 */
int8_t BSP_utils_getOperationSttById(uint8_t id) {
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return -1;
    }
    return operationTimeManagement[id].operateStatus;
}

/* @brief: get opertation time of device by id
 *
 */
void BSP_utils_getOperationTimeById(uint8_t id, t_operation_time *time) {
    // check valid device id number
    if ((id > MAX_DEVICE_NUMBER) || (!time)) {
        return;
    }

    // get operation time
    time->hours = operationTimeManagement[id].operTime.hours;
    time->minutes = operationTimeManagement[id].operTime.minutes;
    time->seconds = operationTimeManagement[id].operTime.seconds;
}

/* @brief: update opertation time for all devices
 *
 */
void BSP_utils_updateOperationTimeAll(void) {
    for(uint8_t i = 0; i < MAX_DEVICE_NUMBER; i++) {
        updateOperationTimeById(i);
    }
}

/* @brief: get operation hour by device id
 *
 */
uint16_t BSP_utils_getOperationHourById(uint8_t id) {
    uint16_t hours = 0;
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return hours;
    }

    t_operation_time time;
    BSP_utils_getOperationTimeById(id, &time);
    hours = time.hours;

    return hours;
}

/* @brief: get operation minute by device id
 *
 */
uint16_t BSP_utils_getOperationMinuteById(uint8_t id) {
    uint16_t minutes = 0;
    // check valid device id number
    if (id > MAX_DEVICE_NUMBER) {
        return minutes;
    }

    t_operation_time time;
    BSP_utils_getOperationTimeById(id, &time);
    minutes = time.hours * 60 + time.minutes;

    return minutes;
}

