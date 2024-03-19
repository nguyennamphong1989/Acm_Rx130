/*
 * control.c
 *
 *  Created on: Aug 3, 2022
 *      Author: Lover_boy_9x
 */

#include "control.h"
#include "Config_CMT0.h"
#include "timeCheck.h"
#include "deviceFlash.h"
#include "Config_ICU.h"
#include "gpio.h"
#include "apl_status.h"
#define COMMAND_NUM		10
extern volatile uint32_t g_sysTime;
uint32_t timeStartLearnCommand;
uint16_t enable_learncommand = 0xFFFF;
uint16_t learn_command_state = 0;
uint16_t modbus_do_command = 0;
extern uint16_t buffer_IRrx[1000];
extern uint16_t IRidx;
extern volatile uint32_t g_sysTime;
extern uint32_t timeOutIRremote;
uint32_t time1s = 0;
extern uint16_t count;

extern const ac_data_t S_AC_DATA_ON;
extern const ac_data_t S_AC_DATA_OFF;
extern const ac_data_t S_AC_DATA_Dry;
extern const ac_data_t S_AC_DATA_Normal;
extern const ac_data_t S_AC_DATA_Enhance;


void updateStateAC(uint8_t acIndex, uint8_t state)
{
	AIRC_STATUS_t *p_status = APL_stt_get();
	if (acIndex < 4)
	{
		if (acIndex == AIRC1_ID) p_status->air1_state = state;
		else if (acIndex == AIRC2_ID) p_status->air2_state = state;
		else if (acIndex == AIRC3_ID) p_status->air3_state = state;
		else if (acIndex == AIRC4_ID) p_status->air4_state = state;
	}
}


void osl_ctrlFullAc(uint8_t acIndex, ac_data_t acData)
{
	if (acIndex >= 4) return;
	if (memcmp(&acData, &S_AC_DATA_ON, sizeof(ac_data_t)) == 0)
	{
		_doCommand(acIndex, 1);
	}
	else if (memcmp(&acData, &S_AC_DATA_OFF, sizeof(ac_data_t)) == 0)
	{
		_doCommand(acIndex, 2);
	}
	else if (memcmp(&acData, &S_AC_DATA_Dry, sizeof(ac_data_t)) == 0)
	{
		_doCommand(acIndex, 3);
	}
	else if (memcmp(&acData, &S_AC_DATA_Normal, sizeof(ac_data_t)) == 0)
	{
		_doCommand(acIndex, 4);
	}
	else if (memcmp(&acData, &S_AC_DATA_Enhance, sizeof(ac_data_t)) == 0)
	{
		_doCommand(acIndex, 5);
	}
}


uint16_t p[1000];
uint16_t length;

void _doCommand(uint8_t acIndex, uint8_t commandIndex)
{
	uint8_t idx_ac = acIndex + 1;
	if (idx_ac > 4) return;
	if (learn_command_state == LEARN_COMMAND_ENTER) return;
	if (acIndex == AIRC1_ID)
	{
		if ((commandIndex == 1) || (commandIndex == 3) || (commandIndex == 4) || (commandIndex == 5))
		{
			TURN_ON_LED_AIRC1;
			BSP_utils_startOperationTimeMeasurementById(AIRC1_ID);
		}
		else if (commandIndex == 2)
		{
			TURN_OFF_LED_AIRC1;
			BSP_utils_stopOperationTimeMeasurementById(AIRC1_ID);
		}
	}
	else if (acIndex == AIRC2_ID)
	{
		if ((commandIndex == 1) || (commandIndex == 3) || (commandIndex == 4) || (commandIndex == 5))
		{
			TURN_ON_LED_AIRC2;
			BSP_utils_startOperationTimeMeasurementById(AIRC2_ID);
		}
		else if (commandIndex == 2)
		{
			TURN_OFF_LED_AIRC2;
			BSP_utils_stopOperationTimeMeasurementById(AIRC2_ID);
		}
	}
	updateStateAC(acIndex, commandIndex);//update AC state as command
    uint32_t add = (idx_ac * 10 + commandIndex) * 0x10000;
	deviceFlash_readData(add, (uint8_t*)&length, 2);
	if ((length > 1000) || (length <= 50)) return;  // pulse so long or so short
	deviceFlash_readData(add + 0x1000, (uint8_t*)p, 2 * length);
	uint8_t checkErase = 0;
	for (uint8_t i = 0; i < 10; i++)
	{
		if (*(p+i) > 30000)
		{
			checkErase++;
		}
	}
	if (checkErase > 5) return;
	length +=1; p[length -1] = 0x7FFF;
	IR_Serial_Send(p, length, acIndex);
}

volatile bool checkResetReetech = false;
void Control_learnCommand()
{
	AIRC_STATUS_t *p_status = APL_stt_get();

	static uint16_t commandisLearning = 0xFFFF;

	if (enable_learncommand != 0xFFFF)
	{
		uint8_t idx_air_local = (uint8_t)(enable_learncommand >> 8);
		uint8_t idx_command_local = (uint8_t)enable_learncommand;
		if ((idx_air_local > 4) || (idx_air_local == 0))
		{
			if (learn_command_state == LEARN_COMMAND_ENTER)
			{
				enable_learncommand = commandisLearning;
			}
			else
			{
				enable_learncommand = 0xFFFF;
				return;
			}
		}
		if ((idx_command_local > COMMAND_NUM) || (idx_command_local == 0))
		{
			if (learn_command_state == LEARN_COMMAND_ENTER)
			{
				enable_learncommand = commandisLearning;
			}
			else
			{
				enable_learncommand = 0xFFFF;
				return;
			}
		}
		if (learn_command_state != LEARN_COMMAND_ENTER)
		{
			commandisLearning = enable_learncommand;
			learn_command_state = LEARN_COMMAND_ENTER;
			timeStartLearnCommand = g_sysTime;
			IRidx = 0; timeOutIRremote = g_sysTime; count = 0;
			checkResetReetech = false;
			R_Config_CMT0_Stop();
			CMT0.CMCNT = 0;
            CMT0.CMCOR = _FFFF_CMT0_CMCOR_VALUE;
			R_Config_ICU_IRQ5_Start();
		}
		else
		{
			if (enable_learncommand == commandisLearning)
			{
				if (elapsedTime(g_sysTime, timeStartLearnCommand) > 20000)
				{
					learn_command_state = LEARN_COMMAND_TIMEOUT;
					enable_learncommand = 0xFFFF;
					R_Config_ICU_IRQ5_Stop();
					R_Config_CMT0_Stop();
				}
				else
				{
					if (elapsedTime(g_sysTime, timeOutIRremote) > 80)
					{
						IRidx = 0;
						if (count > 50)
						{
							learn_command_state = LEARN_COMMAND_DONE;
							enable_learncommand = 0xFFFF;
							R_Config_ICU_IRQ5_Stop();
							R_Config_CMT0_Stop();
						    uint32_t add = (idx_air_local * 10 + idx_command_local) * 0x10000;
							deviceFlash_erase64k(add);
							deviceFlash_writeData(add, &count, 2);
							deviceFlash_writeData(add + 0x1000, buffer_IRrx, 2 * count + 2);
						}
					}
				}
			}
			else
			{
				commandisLearning = enable_learncommand;
				timeStartLearnCommand = g_sysTime;
				IRidx = 0; timeOutIRremote = g_sysTime; count = 0;
				checkResetReetech = false;
				R_Config_CMT0_Stop();
				CMT0.CMCNT = 0;
				CMT0.CMCOR = _FFFF_CMT0_CMCOR_VALUE;
				R_Config_ICU_IRQ5_Start();
			}
		}
	}
	else
	{
		if (learn_command_state == LEARN_COMMAND_ENTER)
		{
			learn_command_state = LEARN_COMMAND_INIT;
			R_Config_ICU_IRQ5_Stop();
			R_Config_CMT0_Stop();
		}
	}
}


uint32_t timeStartNormal[AC_NUM];
void Control_carryOnCommand()
{
	AIRC_STATUS_t *p_status = APL_stt_get();
	if (modbus_do_command)
	{
		uint8_t idx_air_local = (uint8_t)(modbus_do_command >> 8);
		uint8_t idx_command_local = (uint8_t)modbus_do_command;
		modbus_do_command = 0;
		if (p_status->mode_auto_en ==  MODE_AUTO) return;
		if ((idx_air_local > 4) || (idx_air_local == 0))
		{
			return;
		}
		if ((idx_command_local > COMMAND_NUM) || (idx_command_local == 0))
		{
			return;
		}
		_doCommand(idx_air_local - 1, idx_command_local);
		timeStartNormal[idx_air_local - 1] = g_sysTime;
	}
}


void osl_turn_fan(uint8_t state)
{
	if (state > ON) return;
	AIRC_STATUS_t *p_status = APL_stt_get();
	p_status->fan_command = state;
	OSL_led_write(LED_FAN_ID, state);
	BSP_fan_dout_write(state);
	// enable/disable operation time for FAN
	if (state == OFF)
	{
		BSP_utils_stopOperationTimeMeasurementById(FAN_ID);
		TURN_OFF_LED_FAN;
	}
	else
	{
		BSP_utils_startOperationTimeMeasurementById(FAN_ID);
		TURN_ON_LED_FAN;
	}
};


void OSL_led_write(uint32_t led_id, uint8_t state)
{
	if (led_id == LED_AIRC1_ID) {

		if (!state) {
			TURN_OFF_LED_AIRC1;
		} else {
			TURN_ON_LED_AIRC1;
		}
	} else if (led_id == LED_AIRC2_ID) {
		if (!state) {
			TURN_OFF_LED_AIRC2;
		} else {
			TURN_ON_LED_AIRC2;
		}
	} else if (led_id == LED_AIRC3_ID) {
		if (!state) {
			TURN_OFF_LED_AIRC3;
		} else {
			TURN_ON_LED_AIRC3;
		}
	} else if (led_id == LED_AIRC4_ID) {
		if (!state) {
			TURN_OFF_LED_AIRC4;
		} else {
			TURN_ON_LED_AIRC4;
		}
	} else if (led_id == LED_FAN_ID) {
		if (!state) {
			TURN_OFF_LED_FAN;
		} else {
			TURN_ON_LED_FAN;
		}
	} else if (led_id == LED_RUN_ID) {
		if (!state) {
			TURN_OFF_LED_PW;
		} else {
			TURN_ON_LED_PW;
		}
	}
}


void BSP_fan_dout_write(uint8_t state)
{
	if (state != 0) {
		TURN_ON_FAN;
	} else {
		TURN_OFF_FAN;
	}
}

