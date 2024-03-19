/***********************************************************************
*
*  FILE        : Acm_Rx130.c
*  DATE        : 2022-07-27
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include "led7seg.h"
#include "r_smc_entry.h"
#include "gpio.h"
#include "button.h"
#include "deviceFlash.h"
#include "internal_flash.h"
#include "apl_status.h"
#include "apl_config.h"
#include "RS485.h"
#include "timeCheck.h"
#include "control.h"
#include "apl_ui.h"
#include "r_fw_up_rx_if.h"
#include "Acm_Rx130.h"
#include "sensor.h"

void main(void);
uint16_t Mfr = 0;
uint16_t ID = 0;
extern volatile uint8_t g_run1msFlag;
extern volatile uint32_t g_sysTime;
extern uint32_t timeBuzzeron;
static uint32_t time3000ms = 0;
extern volatile bool enterUpdateFirmWare;

//static const uint8_t s_string_menu0[]           = "This program is the sample firmware.\r\n";
//static const uint8_t s_string_menu1[]           = "Push Enter key to execute firmware update.\r\n";
//static const uint8_t s_string_input[]           = "> ";
//static const uint8_t s_string_crlf[]            = "\r\n";
//static const uint8_t s_string_reset[]           = "Switch Start-Up area and do software reset.\r\n";
//
//static const uint8_t s_string_flash_err[]       = "Flash module error.\r\n";
//static const uint8_t s_string_switch_err[]      = "Switching Start-Up area error.\r\n";
//static const uint8_t s_string_init_update_err[] = "Initialize update error.\r\n";
//static const uint8_t s_string_fin_update_err[]  = "Finalize update error.\r\n";
//static const uint8_t s_string_resetvect_err[]   = "Reset vector of the firmware update is invalid.\r\n";

void main_enterUpdate()
{
	fw_up_return_t ret_fw_up;
    ret_fw_up = fw_up_open();
    if (FW_UP_SUCCESS != ret_fw_up)
	{
		/* Casting the pointer to a uint16_t type is valid
		 * because the width is a string expressible with 16 bits. */
//		send_string_sci(s_string_init_update_err, (uint16_t)(sizeof(s_string_init_update_err)));
	}
	else
	{
		/* Casting the pointer to a uint16_t type is valid
		 * because the width is a string expressible with 16 bits. */
//		send_string_sci(s_string_reset, (uint16_t)(sizeof(s_string_reset)));

		/* Casting the pointer to a uint16_t type is valid
		 * because the width is a string expressible with 16 bits. */
//		send_string_sci(s_string_crlf, (uint16_t)(sizeof(s_string_crlf)));
		ret_fw_up = switch_start_up_and_reset();

		switch (ret_fw_up)
		{
			case FW_UP_ERR_NOT_OPEN:

				/* Casting the pointer to a uint16_t type is valid
				 * because the width is a string expressible with 16 bits. */
				/*send_string_sci(s_string_init_update_err, (uint16_t)(sizeof(s_string_init_update_err)));*/
				break;
			case FW_UP_ERR_INVALID_RESETVECT:

				/* Casting the pointer to a uint16_t type is valid
				 * because the width is a string expressible with 16 bits. */
				/*send_string_sci(s_string_resetvect_err, (uint16_t)(sizeof(s_string_resetvect_err)));*/
				break;
			case FW_UP_ERR_SWITCH_AREA:

				/* Casting the pointer to a uint16_t type is valid
				 * because the width is a string expressible with 16 bits. */
				/*send_string_sci(s_string_switch_err, (uint16_t)(sizeof(s_string_switch_err)));*/
				break;
			default:

				/** Do nothing */
			break;
		}

		ret_fw_up = fw_up_close();

		if (FW_UP_SUCCESS != ret_fw_up)
		{
			/* Casting the pointer to a uint16_t type is valid
			 * because the width is a string expressible with 16 bits. */
			/*send_string_sci(s_string_fin_update_err, (uint16_t)(sizeof(s_string_fin_update_err)));*/
		}
	}

}
uint8_t countTo2_current = 0;
void main(void)
{
	TURN_ON_LED_PW;
	R_Config_CMT1_Start();
	R_Config_RSPI0_Start();
	R_Config_RIIC0_Start();
	btn_Init();
	led7seg_start();
	Modbus_init();
	deviceFlash_readMfrID(&Mfr, &ID);
	BSP_utils_init();
	APL_cfg_init();
	APL_stt_init();
	Led7seg_setAllLedChar(CHAR_0);
	BUZZER_ON;
	R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);
	BUZZER_OFF;

	while(1U)
	{
		if (enterUpdateFirmWare)
		{
			main_enterUpdate();
		}
		if (g_run1msFlag)
		{
			g_run1msFlag = 0;
			if (++countTo2_current == 2)
			{
				processCurrent();
				countTo2_current = 0;
			}
			if (++time3000ms >= 3000)
			{
				time3000ms = 0;
				APL_stt_manager();
			}
			ui_manager();
			BSP_rs485_Process();
		}
		Control_learnCommand();
		Control_carryOnCommand();
		if (elapsedTime(g_sysTime, timeBuzzeron) > 100)
		{
			BUZZER_OFF;
		}
	}
}
