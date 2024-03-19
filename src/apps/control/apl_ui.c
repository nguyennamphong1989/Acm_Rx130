/*
 * apl_ui.c
 *
 *  Created on: Aug 20, 2022
 *      Author: Lover_boy_9x
 */
#include <apl_ui.h>
#include "apl_status.h"
#include "apl_config.h"
#include "led7seg.h"
#define MAX_WAIT_TIME_FOR_SETTING (15000)
///////////////////////////////////////LOCAL CONSTANTS//////////////////////////////////////////////
typedef enum
{
    MODE_SET_NONE = 0U,
    MODE_SET_T1,
    MODE_SET_T2,
    MODE_SET_T3,
    MODE_SET_T4
} SettingMode;

///////////////////////////////////////LOCAL VARIABLES//////////////////////////////////////////////
static SettingMode g_setting_mode = MODE_SET_NONE;
uint8_t g_is_setting_enabled = 0;
static uint16_t g_setting_time_counter = 0;
extern void APL_cfg_thresholds_temp_update(uint8_t thr_temp_1, uint8_t thr_temp_2, uint8_t thr_temp_3,
                                           uint8_t thr_temp_4, uint8_t thr_temp_delta);
extern const uint8_t LED7SEG_1_ID;
extern const uint8_t LED7SEG_2_ID;
extern const uint8_t LED7SEG_3_ID;
extern const uint8_t LED7SEG_4_ID;
extern const uint8_t LED7SEG_5_ID;
extern const uint8_t LED7SEG_6_ID;
extern const uint8_t LED7SEG_7_ID;
extern const uint8_t LED7SEG_8_ID;
extern volatile uint32_t g_sysTime; extern uint32_t timeBuzzeron;
void Control_handleButtonUserPress(btn_id_t id)
{
	static int8_t adjustVal = 0;
	static uint8_t temp_val = 0;
	AIRC_STATUS_t *p_stt = APL_stt_get();
	if (id == BTN_ID_1)
	{
		if (g_setting_mode == MODE_SET_NONE)
		{
			g_is_setting_enabled = 1;   // mark to durring setting mode
			g_setting_time_counter = 0; // reset counter
			g_setting_mode = MODE_SET_T1;
			temp_val = p_stt->thr_temp_1;  adjustVal = 0;
			// turn unused led
			Led7seg_showCharAt(CHAR_OFF_LED, LED7SEG_1_ID);
			Led7seg_showCharAt(CHAR_OFF_LED, LED7SEG_5_ID);
			Led7seg_showCharAt(CHAR_OFF_LED, LED7SEG_6_ID);
			Led7seg_showCharAt(CHAR_OFF_LED, LED7SEG_7_ID);
			Led7seg_showCharAt(CHAR_OFF_LED, LED7SEG_8_ID);
			// update value to display on led
			Led7seg_showCharAt(CHAR_1, LED7SEG_2_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_1 / 10), LED7SEG_3_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_1 % 10), LED7SEG_4_ID);
		}
		else if (g_setting_mode == MODE_SET_T1)
		{
			g_setting_time_counter = 0; // reset counter
			g_setting_mode = MODE_SET_T2;
			temp_val = p_stt->thr_temp_2; adjustVal = 0;
			Led7seg_showCharAt(CHAR_2, LED7SEG_2_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_2 / 10), LED7SEG_3_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_2 % 10), LED7SEG_4_ID);
			// Need to save T1 value
		}
		else if (g_setting_mode == MODE_SET_T2)
		{
			g_setting_time_counter = 0; // reset counter
			g_setting_mode = MODE_SET_T3;
			temp_val = p_stt->thr_temp_3; adjustVal = 0;
			Led7seg_showCharAt(CHAR_3, LED7SEG_2_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_3 / 10), LED7SEG_3_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_3 % 10), LED7SEG_4_ID);
			// Need to save T3 value
		}
		else if (g_setting_mode == MODE_SET_T3)
		{
			g_setting_time_counter = 0; // reset counter
			g_setting_mode = MODE_SET_T4;
			temp_val = p_stt->thr_temp_4; adjustVal = 0;
			Led7seg_showCharAt(CHAR_4, LED7SEG_2_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_4 / 10), LED7SEG_3_ID);
			Led7seg_showCharAt((Specials_char_t)(p_stt->thr_temp_4 % 10), LED7SEG_4_ID);
			// Need to save T2 value
		}
		else
		{
			g_setting_mode = MODE_SET_NONE;
			g_is_setting_enabled = 0;
			g_setting_time_counter = 0; // reset counter
			// Need to save T4 value
			// Set temperature to display
			BSP_led7seg_all_write(p_stt->temp_indoor, p_stt->temp_outdoor, p_stt->hour, p_stt->minute);
			// turn on all LED
		}
		timeBuzzeron = g_sysTime;
			BUZZER_ON;
	}
	else if (id == BTN_ID_2)
	{

		if (g_setting_mode == MODE_SET_T1)
		{
			APL_cfg_thresholds_temp_update(p_stt->thr_temp_1 + adjustVal, p_stt->thr_temp_2, p_stt->thr_temp_3,
										   p_stt->thr_temp_4, p_stt->thr_temp_delta);
		}
		else if (g_setting_mode == MODE_SET_T2)
		{
			APL_cfg_thresholds_temp_update(p_stt->thr_temp_1, p_stt->thr_temp_2 + adjustVal, p_stt->thr_temp_3,
										   p_stt->thr_temp_4, p_stt->thr_temp_delta);
		}
		else if (g_setting_mode == MODE_SET_T3)
		{
			APL_cfg_thresholds_temp_update(p_stt->thr_temp_1, p_stt->thr_temp_2, p_stt->thr_temp_3 + adjustVal,
										   p_stt->thr_temp_4, p_stt->thr_temp_delta);
		}
		else if (g_setting_mode == MODE_SET_T4)
		{
			APL_cfg_thresholds_temp_update(p_stt->thr_temp_1, p_stt->thr_temp_2, p_stt->thr_temp_3,
										   p_stt->thr_temp_4 + adjustVal, p_stt->thr_temp_delta);
			g_setting_mode = MODE_SET_NONE;
			// Set temperature to display
			BSP_led7seg_all_write(p_stt->temp_indoor, p_stt->temp_outdoor, p_stt->hour, p_stt->minute);
			// turn on all LED
		}
		else
		{
			// T.B.D
			// osl_turn_airc1(ON);
		}
		// Reset adjust value
		adjustVal = 0;
		temp_val = 0;
		timeBuzzeron = g_sysTime;
			BUZZER_ON;
	}
	else if (id == BTN_ID_3)
	{
		g_setting_time_counter = 0; // reset counter
		if (g_setting_mode != MODE_SET_NONE)
		{
			if ((temp_val + adjustVal) < 99)
			{
				adjustVal++;
				// Display value to 7 segment
				Led7seg_showCharAt((Specials_char_t)((temp_val + adjustVal) / 10), LED7SEG_3_ID);
				Led7seg_showCharAt((Specials_char_t)((temp_val + adjustVal) % 10), LED7SEG_4_ID);
			}
		}
		timeBuzzeron = g_sysTime;
			BUZZER_ON;
	}
	else if (id == BTN_ID_4)
	{

		g_setting_time_counter = 0; // reset counter
		if (g_setting_mode != MODE_SET_NONE)
		{
			if ((temp_val + adjustVal) > 0)
			{
				adjustVal--;
				// Display value to 7 segment
				Led7seg_showCharAt((Specials_char_t)((temp_val + adjustVal) / 10), LED7SEG_3_ID);
				Led7seg_showCharAt((Specials_char_t)((temp_val + adjustVal) % 10), LED7SEG_4_ID);
			}
		}
		timeBuzzeron = g_sysTime;
			BUZZER_ON;

	}
}
/**	@brief update LED state based on current device status.
*/
static void led_7seg_process(void)
{
    AIRC_STATUS_t *p_stt = APL_stt_get();
    static uint16_t old_temp_indoor = 0xFF, /*old_humid_indoor = 0xFF,*/ old_temp_outdoor = 0xFF;
    static uint8_t old_hour = 0xFF, old_minute = 0xFF;
    static uint16_t cnt_30s = 2000;

    // Update period 30s:
    if (++cnt_30s > 2000)
    {
        cnt_30s = 0U;
        // if ((old_temp_indoor != p_stt->temp_indoor) || (old_humid_indoor != p_stt->humid_indoor) || (old_temp_outdoor != p_stt->temp_outdoor)) {
        if ((old_temp_indoor != p_stt->temp_indoor) || (old_temp_outdoor != p_stt->temp_outdoor) || (old_hour != p_stt->hour) || (old_minute != p_stt->minute))
        {
            old_hour = p_stt->hour;
            old_minute = p_stt->minute;
            old_temp_indoor = p_stt->temp_indoor;
            // old_humid_indoor = p_stt->humid_indoor;
            old_temp_outdoor = p_stt->temp_outdoor;
        }
        // Update display data if it not in setting mode
		if (g_is_setting_enabled == 0)
		{
			// OSL_led7seg_write_all(old_temp_indoor, old_temp_outdoor, 0x00, old_humid_indoor);
			BSP_led7seg_all_write(old_temp_indoor, old_temp_outdoor, old_hour, old_minute);
		}
    }
}
void ui_manager(void)
{
    // LED7SEG process:
	btn_eventTask();
	led_7seg_process();

    /* In the case that user in setting mode, check timeout to comeback to normal display.
       if user do not operating any more. */
    if (g_is_setting_enabled != 0)
    {
        if (++g_setting_time_counter >= MAX_WAIT_TIME_FOR_SETTING)
        { // timeout case
            // Reset to default
            g_is_setting_enabled = 0;       // disable counting flag
            g_setting_time_counter = 0;     // Clear counter
            g_setting_mode = MODE_SET_NONE; // Reset to default setting mode

            // Reset to defaut view
            AIRC_STATUS_t *p_stt = APL_stt_get();
           // turn on all LED
            BSP_led7seg_all_write(p_stt->temp_indoor, p_stt->temp_outdoor, p_stt->hour, p_stt->minute);
        }
    }
}
