/*
 * rw_flash.h
 *
 *  Created on: May 30, 2023
 *      Author: Lover_boy_9x
 */

#ifndef APPS_MODBUS_RW_FLASH_H_
#define APPS_MODBUS_RW_FLASH_H_
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "RS485.h"
#include "control.h"
#include "deviceFlash.h"
#define MSG_HEADER 0x01
#define MSG_TEMINATE 0x02
#define MAX_LEN_BUFFER_SEND 100
typedef enum
{
	MSG_IDX_READ_NUM_WORD,
	MSG_IDX_RESPONE_READ_NUM_WORD,
	MSG_IDX_READ_WORD,
	MSG_IDX_RESPONE_READ_WORD,
	MSG_IDX_WRITE_NUM_WORD,
	MSG_IDX_RESPONE_WRITE_NUM_WORD,
	MSG_IDX_WRITE_WORD,
	MSG_IDX_RESPONE_WRITE_WORD,
	MSG_IDX_MAX
} msgIdx_list_t;
void uartVoice_sendFrame(uint8_t codemsg, uint8_t* data, uint16_t len);
void uartVoice_processFlashFormat(uint8_t *buffer_rx, uint16_t length);

#endif /* APPS_MODBUS_RW_FLASH_H_ */
