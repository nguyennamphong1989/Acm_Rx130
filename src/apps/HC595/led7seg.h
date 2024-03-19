/*
 * HC595.h
 *
 *  Created on: Jul 29, 2022
 *      Author: Lover_boy_9x
 */

#ifndef APPS_HC595_LED7SEG_H_
#define APPS_HC595_LED7SEG_H_

#include "r_cg_macrodriver.h"
#define MAX_SEGMENT			8
#define MAX_DIGIT			8

//define chan HC595
#define SHCP_HC595 			PORTH.PODR.BIT.B0
#define STCP_HC595 			PORT5.PODR.BIT.B5
#define DS_HC595 			PORT5.PODR.BIT.B4
#define OE_HC595			PORTH.PODR.BIT.B1


typedef enum
{
	SEGA_PIN_bit = 0,
	SEGB_PIN_bit,
	SEGC_PIN_bit,
	SEGD_PIN_bit,
	SEGE_PIN_bit,
	SEGF_PIN_bit,
	SEGG_PIN_bit,
	SEGH_PIN_bit,
	DIGIT1_PIN_bit,
	DIGIT2_PIN_bit,
	DIGIT3_PIN_bit,
	DIGIT4_PIN_bit,
	DIGIT5_PIN_bit,
	DIGIT6_PIN_bit,
	DIGIT7_PIN_bit,
	DIGIT8_PIN_bit
} s_value595_bit_t;

typedef enum
{
	CHAR_0  = 0, //
	CHAR_1  = 1, //
	CHAR_2  = 2, //
	CHAR_3  = 3, //
	CHAR_4  = 4, //
	CHAR_5  = 5, //
	CHAR_6  = 6, //
	CHAR_7  = 7, //
	CHAR_8  = 8, //
	CHAR_9  = 9, //
	CHAR_OFF_LED  = 10, //
	CHAR_P_code	 = 11, // // P
	CHAR_r_code   = 12, // // r
	CHAR___code   = 13, // // _
	CHAR_A_code   = 14, // // A
	CHAR_b_code   = 15, // // b
	CHAR_E_code   = 16, // // E
	CHAR_n_code   = 17, // // n
	CHAR_d_code   = 18, // // d
	CHAR_negative = 19, // // -
	CHAR_i_code   = 20,
	CHAR_t_code   = 21,
	CHAR_u_code   = 22,
	CHAR_o_code   = 23,
	CHAR_F_code   = 24,
	CHAR_H_code   = 25
} Specials_char_t;
typedef enum
{
	NUMBER_1 = 0,
	NUMBER_2,
	NUMBER_3,
	NUMBER_4,
	MAX_NUMBER
} value_number_t;
typedef struct
{
	uint8_t idx_led_donvi;
	uint8_t idx_led_chuc;
} idx_number_t;
void Led7seg_onSegment(uint8_t segment);
void Led7seg_offSegment(uint8_t segment);
void Led7seg_onAllSegment(void);
void Led7seg_offAllSegment(void);
void Led7seg_onDigit(uint8_t digit);
void Led7seg_offDigit(uint8_t digit);
void Led7seg_onAllDigit(void);
void Led7seg_offAllDigit(void);
void Led7seg_IRQHandler();
void Led7seg_showCharAt(Specials_char_t character, uint8_t digit);
void Led7seg_setAllLedChar(Specials_char_t character);
void Led7seg_turnOnLed();
void Led7seg_turnOffLed();

void led7seg_start();

void BSP_led7seg_all_write(uint8_t value_led1, uint8_t value_led2, uint8_t value_led3, uint8_t value_led4);

#endif /* APPS_HC595_LED7SEG_H_ */
