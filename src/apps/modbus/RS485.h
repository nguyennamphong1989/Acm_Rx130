/*
 * RS485.h
 *
 *  Created on: Aug 8, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_MODBUS_RS485_H_
#define APPS_MODBUS_RS485_H_
#include "stdbool.h"
#include "gpio.h"
#define MAX_LENGTH_REGISTER 256
#define MAX_QUEUE_DATA_UART     (256)
#define     PULL_DE_LOW			R_GPIO_PinWrite(GPIO_PORT_E_PIN_0, GPIO_LEVEL_LOW);
#define     PULL_DE_HIGH		R_GPIO_PinWrite(GPIO_PORT_E_PIN_0, GPIO_LEVEL_HIGH);
#define 	DELAY_RS485			R_BSP_SoftwareDelay(100, BSP_DELAY_MICROSECS);
bool UART_IsDoneFrame(void);
void Modbus_init();
void UART_sendFrame(uint8_t* frame, uint16_t len);
void BSP_rs485_Process();
void _write(uint8_t *_buf, uint8_t len);

#endif /* APPS_MODBUS_RS485_H_ */
