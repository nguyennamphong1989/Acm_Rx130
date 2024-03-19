/*
 * button.h
 *
 *  Created on: Jul 29, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_BUTTON_BUTTON_H_
#define APPS_BUTTON_BUTTON_H_

#include "stdint.h"
#include "stdlib.h"
#include "gpio.h"
#define TIME_DEBOUNCE 30  		//ms
#define TIME_HOLDING_1 3000		//ms
#define TIME_HOLDING_2 5000		//ms
#define TIME_HOLDING_3 15000	//ms

typedef enum
{
	BTN_ID_1 = 0,
	BTN_ID_2,
	BTN_ID_3,
	BTN_ID_4,
	NUM_BUTTON
} btn_id_t;



/*--------------------------lib (not edit)---------------------------------------*/
#define BTN_PIN_PRESSED(gpioNum, active) (R_GPIO_PinRead(gpioNum) == active)
#define BTN_PIN_RELEASED(gpioNum, active) (R_GPIO_PinRead(gpioNum) != active)
typedef enum
{
	ACTIVE_LOW = 0,
	ACTIVE_HIGH = 1
} btn_press_active_t;

typedef enum
{
	BTN_STATE_RELEASE = 0,
	BTN_STATE_DEBOUNCE,
	BTN_STATE_PRESS,
	BTN_STATE_HOLD1,
	BTN_STATE_HOLD2,
	BTN_STATE_HOLD3,
} btn_state_t;


typedef struct
{
	uint8_t id_btn;
	btn_state_t state_btn;
	uint32_t timeStartPress;			//thoi diem nhan
	uint32_t timePress;
}	btn_profile_t;


typedef enum button_event
{
    BTN_EVENT_RELEASED ,        ///< Button is in the released state
    BTN_EVENT_PRESSED ,         ///< Button is in the pressed state
    BTN_EVENT_HOLD ,            ///< Button released within valid timing
    BTN_EVENT_MULTI_TOUCH,     ///< More than one touch sensor is being touched
    BTN_EVENT_REQUEST_DELAY,   ///< Lower level asking for a delay
} btn_event_t;

/*doi voi time_user:
  EVENT_PRESSD: timeStartPress
  EVENT_RELEASED: timePress
  EVENT_HOLD:    timeHold (hold1, hold2, hold3)
*/
typedef void(*ptr_btn_cb_t)(btn_event_t event, uint8_t btn_id, uint32_t time_user);


typedef struct
{
	uint8_t num_btn;
	btn_press_active_t active_level;
	gpio_port_pin_t *gpio_num_pin_btn;			//tro den mang cac gpio_num ung voi cac pin button
	uint32_t timeDebounce;
	uint32_t timeHolding1;
	uint32_t timeHolding2;
	uint32_t timeHolding3;
	ptr_btn_cb_t btn_cb;			//ten func callback user tu dinh nghia

} btn_setting_t;


typedef struct
{
	btn_setting_t * btn_setting;
	btn_profile_t *btnProfile;
	//btn_event_t btn_event;
} btn_t;
/*----------------------------------------------------------*/
void btn_Pressed_cb(btn_id_t btn);
void btn_Released_cb(btn_id_t btn, uint32_t timePress);
void btn_Hold_cb(btn_id_t btn, uint32_t timeHold);
void btn_Init();
void btn_eventTask();

#endif /* APPS_BUTTON_BUTTON_H_ */
