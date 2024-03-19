/*
 * RS485.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Lover_boy_9x
 */
#include "stdint.h"
#include "string.h"
#include "RS485.h"
#include "stdbool.h"
#include "Config_SCI12.h"
#include "apl_status.h"
#include "apl_config.h"
#include "sensor.h"
#include "control.h"
#include "rw_flash.h"
volatile uint8_t cntTimeRev = 20;
extern volatile uint8_t g_rx_char;
extern uint8_t buffer_rx[MAX_QUEUE_DATA_UART];
extern uint16_t buffer_index;
extern uint16_t enable_learncommand;
extern uint16_t learn_command_state;
extern uint16_t modbus_do_command;
extern uint16_t error_air[4];
extern uint16_t adcValue[NUM_CHANNEL_USER_ADC];
extern uint8_t g_is_setting_enabled;
extern uint16_t error_sensor;
uint16_t bootloader_status = 0x0000;
uint16_t bootloader_control = 0x0000;
uint16_t bootloader_version = 0x0103;
volatile bool enterUpdateFirmWare = false;
volatile bool enableReadFlash = false;
uint16_t  UART_ReadData(char * data,uint16_t maxlen)
{
	uint16_t ret = 0;
	if(data  == NULL)
	{
		return ret;
	}
	memcpy((void*)data,(const void*)(buffer_rx), buffer_index);
	ret = buffer_index;
	buffer_index = 0;
	return ret;
}


void UART_sendFrame(uint8_t* frame, uint16_t len)
{
	R_SCI12_AsyncTransmit(frame, len);
}


void Modbus_init()
{
	R_Config_SCI12_Start();
	R_Config_SCI12_Serial_Receive((uint8_t *)&g_rx_char, 1);
}


uint16_t ModRTU_CRC(uint8_t *buf, int len)
{
	uint16_t crc = 0xFFFF;

	for (int pos = 0; pos < len; pos++) {
		crc ^= (uint16_t)*(buf+pos);          // XOR byte into least sig. byte of crc

		for (int i = 8; i != 0; i--) {    // Loop over each bit
			if ((crc & 0x0001) != 0) {      // If the LSB is set
				crc >>= 1;                    // Shift right and XOR 0xA001
				crc ^= 0xA001;
			}
			else                            // Else LSB is not set
				crc >>= 1;                    // Just shift right
		}
	}
	// Note, this number has low and high bytes swapped, so use it accordingly (or swap bytes)
	return crc;
}


uint16_t _bytesToWord(uint8_t high, uint8_t low) {
	return (high << 8) | low;
}


typedef int32_t (*WordRead)(uint16_t);
int32_t _holdingRegisterRead(uint16_t address)
{
	AIRC_STATUS_t *p_status = APL_stt_get();
	const CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
	if 		(address == 0x0000) return  p_status->temp_indoor;
	else if (address == 0x0001) return  p_status->temp_outdoor;
	else if (address == 0x0002) return  p_status->humid_indoor;
	else if (address == 0x0003) return  p_status->temp_air_1;
	else if (address == 0x0004) return  p_status->temp_air_2;
	else if (address == 0x0005) return  p_status->temp_air_3;
	else if (address == 0x0006) return  p_status->temp_air_4;
	else if (address == 0x0007) return  p_status->air_current_1;
	else if (address == 0x0008) return  p_status->air_current_2;
	else if (address == 0x0009) return  p_status->air_current_3;
	else if (address == 0x000A) return  p_status->air_current_4;

	else if (address == 0x000B) return  enable_learncommand;
	else if (address == 0x000C) return  learn_command_state;

	else if (address == 0x0010) return	modbus_do_command;
	else if (address == 0x0011) return  p_status->air1_state;
	else if (address == 0x0012) return  p_status->air2_state;
	else if (address == 0x0013) return  p_status->air3_state;
	else if (address == 0x0014) return  p_status->air4_state;
	else if (address == 0x0015) return  p_status->fan_command;
	else if (address == 0x0016) return  p_status->acmRunTimeAirc1;
	else if (address == 0x0017) return  p_status->acmRunTimeAirc2;
	else if (address == 0x0018) return  p_status->acmRunTimeAirc3;
	else if (address == 0x0019) return  p_status->acmRunTimeAirc4;
	else if (address == 0x001A) return  p_status->acmRunTimeFan;

	else if (address == 0x0020) return  p_cfg->thr_temp_1;
	else if (address == 0x0021) return  p_cfg->thr_temp_2;
	else if (address == 0x0022) return  p_cfg->thr_temp_3;
	else if (address == 0x0023) return  p_cfg->thr_temp_4;
	else if (address == 0x0024) return  p_cfg->thr_temp_delta;
	else if (address == 0x0025) return  p_cfg->acmControlAuto;
	else if (address == 0x0026) return  p_status->hour;
	else if (address == 0x0027) return  p_status->minute;

	else if (address == 0x0028) return  p_cfg->minCurrent;
	else if (address == 0x0029) return  p_cfg->T_threshold;
	else if (address == 0x002A) return  error_air[0];
	else if (address == 0x002B) return  error_air[1];
	else if (address == 0x002C) return  error_air[2];
	else if (address == 0x002D) return  error_air[3];
	else if (address == 0x002E) return  p_cfg->acmRunTime_1;
	else if (address == 0x002F) return  p_cfg->acmRunTime_2;
	else if (address == 0x0030) return  error_sensor;

	else if (address == 0x0031) return  p_status->air1_sts;
	else if (address == 0x0032) return  p_status->air2_sts;
	else if (address == 0x0033) return  p_status->air3_sts;
	else if (address == 0x0034) return  p_status->air4_sts;

	else if (address == 0x0064) return  bootloader_status;
	else if (address == 0x0065) return  bootloader_control;
	else if (address == 0x0066) return  bootloader_version;
	else if (address == 0x003C) return 	enableReadFlash;
	else return false;
}


bool _holdingRegisterWrite(uint16_t address, uint16_t value) {
	AIRC_STATUS_t *p_status = APL_stt_get();
	CFG_AIRC_t *p_cfg = NULL;
	p_cfg = APL_cfg_get();
	if (address == 0x000B) enable_learncommand = value;
	else if (address == 0x0010) modbus_do_command = value;
	else if (address == 0x0026) p_status->hour = value;
	else if (address == 0x0027) p_status->minute = value;
	else if (address == 0x0020) {if (g_is_setting_enabled == 0) {p_cfg->thr_temp_1 = value;}}
	else if (address == 0x0021) {if (g_is_setting_enabled == 0) {p_cfg->thr_temp_2 = value;}}
	else if (address == 0x0022) {if (g_is_setting_enabled == 0) {p_cfg->thr_temp_3 = value;}}
	else if (address == 0x0023) {if (g_is_setting_enabled == 0) {p_cfg->thr_temp_4 = value;}}
	//else if (address == 0x0024) p_cfg->thr_temp_delta = value;
	else if (address == 0x0025) {p_cfg->acmControlAuto = value;}
	else if (address == 0x0028) {p_cfg->minCurrent = value; APL_cfg_Store();}
	else if (address == 0x0029) {p_cfg->T_threshold = value; APL_cfg_Store();}
	else if (address == 0x002E) {p_cfg->acmRunTime_1 = value; APL_cfg_Store();}
	else if (address == 0x002F) {p_cfg->acmRunTime_2 = value; APL_cfg_Store();}
	else if (address == 0x0065) {bootloader_control = value; if (bootloader_control == 0x0001){enterUpdateFirmWare = true;}}
	else if (address == 0x0015)
	{
		if (value > ON) return true;
		if ((p_status->fan_command != value) && (p_status->mode_auto_en == MODE_NOMAL))
		{
			osl_turn_fan(value);
		}
	}
	else if (address == 0x003C)
	{
		enableReadFlash = value;
	}
	return true;
}


uint8_t highByte(uint16_t num)
{
	return (uint8_t)(num>>8);
}


uint8_t lowByte(uint16_t num)
{
	return (uint8_t)(num);
}


void _write(uint8_t *_buf, uint8_t len) {
	if (_buf[0] != 0) {
		uint16_t crc = ModRTU_CRC(_buf, len);
		_buf[len] = lowByte(crc);
		_buf[len + 1] = highByte(crc);
		PULL_DE_HIGH;
		DELAY_RS485;
		UART_sendFrame(_buf, len + 2);
		DELAY_RS485;
		PULL_DE_LOW;
	}
}


void _exceptionResponse(uint8_t *_buf, uint8_t code) {
	_buf[1] |= 0x80;
	_buf[2] = code;
	_write(_buf, 3);
}


void ModbusRTUSlave_processWordRead(uint8_t *_buf, uint16_t numWords, WordRead wordRead) {
	uint16_t startAddress = _bytesToWord(_buf[2], _buf[3]);
	uint16_t quantity = _bytesToWord(_buf[4], _buf[5]);
	if (quantity == 0 || quantity > ((MAX_QUEUE_DATA_UART - 6) >> 1)) _exceptionResponse(_buf, 3);
	else if ((startAddress + quantity) > numWords)  _exceptionResponse(_buf, 2);
	else
	{
		for (uint8_t j = 0; j < quantity; j++) {
			int32_t value = wordRead(startAddress + j);
			if (value < 0) {
				_exceptionResponse(_buf, 4);
				return;
			}
			_buf[3 + (j * 2)] = highByte(value);
			_buf[4 + (j * 2)] = lowByte(value);
		}
		_buf[2] = quantity * 2;
		_write(_buf, 3 + _buf[2] );
	}
}


void UART_processFrameFormat(uint8_t *buffer_rx, uint16_t length)
{
	if (((buffer_rx[0] == 0x10) || (buffer_rx[0] == 0)) && (ModRTU_CRC(buffer_rx, length - 2) == _bytesToWord(buffer_rx[length - 1], buffer_rx[length -2])))
	{
		if ((length > 4) && (length < 256))
		{
			switch (buffer_rx[1]){
			case 3:  /* Read Holding Registers */
				ModbusRTUSlave_processWordRead(buffer_rx, MAX_LENGTH_REGISTER, _holdingRegisterRead);
			break;
			case 6: /* Write Single Holding Register */
			{
				uint16_t address = _bytesToWord(buffer_rx[2], buffer_rx[3]);
				uint16_t value = _bytesToWord(buffer_rx[4], buffer_rx[5]);
				if (address >= MAX_LENGTH_REGISTER) _exceptionResponse(buffer_rx, 2);
				else if (!_holdingRegisterWrite(address, value)) _exceptionResponse(buffer_rx, 4);
				else _write(buffer_rx, 6);
			}
			break;
			case 16: /* Write Multiple Holding Registers */
			{
				uint16_t startAddress = _bytesToWord(buffer_rx[2], buffer_rx[3]);
				uint16_t quantity = _bytesToWord(buffer_rx[4], buffer_rx[5]);
				if (quantity == 0 || quantity > ((MAX_QUEUE_DATA_UART - 10) >> 1) || buffer_rx[6] != (quantity * 2)) _exceptionResponse(buffer_rx, 3);
				else if (startAddress + quantity > MAX_LENGTH_REGISTER) _exceptionResponse(buffer_rx, 2);
				else {
					for (uint8_t j = 0; j < quantity; j++) {
						if (!_holdingRegisterWrite(startAddress + j, _bytesToWord(buffer_rx[j * 2 + 7], buffer_rx[j * 2 + 8]))) {
							_exceptionResponse(buffer_rx, 4);
							return;
						}
					}
					_write(buffer_rx, 6);
				}
			}
			break;
			}
		}
	}
	else if (enableReadFlash)
	{
		uartVoice_processFlashFormat(buffer_rx, length);
	}
}


void BSP_rs485_Process()
{
	if(UART_IsDoneFrame())
	{
		char data[MAX_QUEUE_DATA_UART] = {0};
		uint16_t len = 0;
		len = UART_ReadData(data,MAX_QUEUE_DATA_UART);
		UART_processFrameFormat((uint8_t*)data, len);
	}
}


bool UART_IsDoneFrame(void)
{
	if(cntTimeRev)
	{
		cntTimeRev--;
		if(cntTimeRev == 0)
		{
			return true;
		}
	}
	return false;
}

