/*
 * control.h
 *
 *  Created on: Aug 3, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_CONTROL_CONTROL_H_
#define APPS_CONTROL_CONTROL_H_
#include "apl_status.h"
#include "stdint.h"
typedef enum
{

	LEARN_COMMAND_INIT = 0,
	LEARN_COMMAND_ENTER,
	LEARN_COMMAND_DONE,
	LEARN_COMMAND_TIMEOUT
} learn_command_state_t;
// LED
#define LED_AIRC1_ID								0
#define LED_AIRC2_ID								1
#define LED_AIRC3_ID								2
#define LED_AIRC4_ID								3
#define LED_FAN_ID									4
#define LED_RUN_ID									5
void osl_ctrlFullAc(uint8_t acIndex, ac_data_t acData);
void Control_learnCommand();
void Control_carryOnCommand();
void OSL_led_write(uint32_t led_id, uint8_t state);
void osl_turn_fan(uint8_t state);
void BSP_fan_dout_write(uint8_t state);
void _doCommand(uint8_t acIndex, uint8_t commandIndex);
#endif /* APPS_CONTROL_CONTROL_H_ */
