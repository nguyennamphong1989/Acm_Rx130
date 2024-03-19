/*
 * button.c
 *
 *  Created on: Jul 29, 2022
 *      Author: Lover_boy_9x
 */


#include "button.h"
#include "apl_config.h"
#include "gpio.h"
#include "timeCheck.h"
#include "string.h"
#include "apl_ui.h"
extern volatile uint32_t g_sysTime;
static gpio_port_pin_t gpio_num_btn[NUM_BUTTON] = {BTN1_Pin, BTN2_Pin, BTN3_Pin, BTN4_Pin}; /*chi so phan tu chinh la btn_id*/

/*-----------------------lib button (not edit)-------------------------------------*/
static btn_t btn;
static btn_setting_t btnCfg;
static btn_profile_t btnProfile[NUM_BUTTON];
void btn_eventTask()
{
	for(int i = 0; i < btn.btn_setting->num_btn; i++)
		{
			if(btn.btnProfile[i].state_btn != BTN_STATE_RELEASE && btn.btnProfile[i].state_btn != BTN_STATE_DEBOUNCE)
			{
				if(BTN_PIN_RELEASED(btn.btn_setting->gpio_num_pin_btn[i], btn.btn_setting->active_level))
				{
					btn.btnProfile[i].timePress = g_sysTime - btn.btnProfile[i].timeStartPress;
					btn.btn_setting->btn_cb(BTN_EVENT_RELEASED, i, btn.btnProfile[i].timePress);                         //event released
					btn.btnProfile[i].state_btn = BTN_STATE_RELEASE;
				}
			}
			switch(btn.btnProfile[i].state_btn)
			{
				case BTN_STATE_RELEASE:
				{
					if(BTN_PIN_PRESSED(btn.btn_setting->gpio_num_pin_btn[i], btn.btn_setting->active_level))
					{
						btn.btnProfile[i].state_btn = BTN_STATE_DEBOUNCE;
						btn.btnProfile[i].timeStartPress = g_sysTime;
					}
					break;
				}
				case BTN_STATE_DEBOUNCE:
				{
					if(elapsedTime(g_sysTime,btn.btnProfile[i].timeStartPress) > btn.btn_setting->timeDebounce)
					{
						if(BTN_PIN_PRESSED(btn.btn_setting->gpio_num_pin_btn[i], btn.btn_setting->active_level))
						{
							btn.btn_setting->btn_cb(BTN_EVENT_PRESSED, i, btn.btnProfile[i].timeStartPress);
							btn.btnProfile[i].state_btn = BTN_STATE_PRESS;       //event pressed
						}
						else btn.btnProfile[i].state_btn = BTN_STATE_RELEASE;
					}
					break;
				}
				case BTN_STATE_PRESS:
					if(elapsedTime(g_sysTime, btn.btnProfile[i].timeStartPress) > btn.btn_setting->timeHolding1)
					{
						btn.btnProfile[i].state_btn = BTN_STATE_HOLD1;
						btn.btn_setting->btn_cb(BTN_EVENT_HOLD, i, btn.btn_setting->timeHolding1);
					}
					break;
				case BTN_STATE_HOLD1:
					if(elapsedTime(g_sysTime, btn.btnProfile[i].timeStartPress) > btn.btn_setting->timeHolding2)
					{
						btn.btnProfile[i].state_btn = BTN_STATE_HOLD2;
						btn.btn_setting->btn_cb(BTN_EVENT_HOLD, i, btn.btn_setting->timeHolding2);
					}
				break;
				case BTN_STATE_HOLD2:
					if(elapsedTime(g_sysTime, btn.btnProfile[i].timeStartPress) > btn.btn_setting->timeHolding3)
					{
						btn.btnProfile[i].state_btn = BTN_STATE_HOLD3;
						btn.btn_setting->btn_cb(BTN_EVENT_HOLD, i, btn.btn_setting->timeHolding3);
					}
						break;
				case BTN_STATE_HOLD3:
				break;
			}
		}
}
static void btn_config(btn_setting_t *btnSet)
{
	btn.btn_setting =  btnSet;
	btn.btnProfile =  &btnProfile[0];
    for(int i = 0; i < btnSet->num_btn; i++)
    {
        btn.btnProfile[i].id_btn = i;
        btn.btnProfile[i].state_btn = BTN_STATE_RELEASE;
        btn.btnProfile[i].timeStartPress = 0;
        btn.btnProfile[i].timePress = 0;
    }
}

static void btn_callBack(btn_event_t event, uint8_t btn_id, uint32_t time_user)
{
    switch(event)
    {
        case BTN_EVENT_PRESSED:
        {
            btn_Pressed_cb((btn_id_t)btn_id);
            break;
        }
        case BTN_EVENT_RELEASED:
        {
            btn_Released_cb((btn_id_t)btn_id, time_user);
            break;
        }
        case BTN_EVENT_HOLD:
        {
            btn_Hold_cb((btn_id_t)btn_id, time_user);
            break;
        }

    }


}

/*================================================*/
void btn_Init()
{
    /*user config*/
    // config GPIO input

    /*-------------------------------*/
       //luu y: o day khai bao bien cuc bo, sau khi ham return thi bien se bi mat,
                            //trong ham btn_config can copy gia tri nay, neu khong trong btn_eventTask se k co bien,

    btnCfg.num_btn = NUM_BUTTON;
    btnCfg.active_level = ACTIVE_LOW;
    btnCfg.gpio_num_pin_btn = gpio_num_btn;
    btnCfg.timeDebounce = 30;       //ms
    btnCfg.timeHolding1 = TIME_HOLDING_1;
    btnCfg.timeHolding2 = TIME_HOLDING_2;
    btnCfg.timeHolding3 = TIME_HOLDING_3;
    btnCfg.btn_cb = btn_callBack;

    btn_config(&btnCfg);
}

/*-----------------------------------------------------------*/
void btn_Pressed_cb(btn_id_t btn_id)
{
	Control_handleButtonUserPress(btn_id);
}
uint32_t timeBuzzeron = 0;

void btn_Released_cb(btn_id_t btn_id, uint32_t timePress)
{
//	Control_handleButtonUserRelease(btn_id,timePress);
}


void btn_Hold_cb(btn_id_t btn_id, uint32_t timeHold)
{
	//Control_handleButtonUserHold(btn_id, timeHold);
}
