/*
 * led7seg.c
 *
 *  Created on: Jul 29, 2022
 *      Author: Lover_boy_9x
 */
//thao tac bit
#define UTY_GET_BIT(n,k)    ((n>>k) & 0x01)
#define UTY_SET_BIT(n,k)    (n |= (0x01<<k))
#define UTY_CLEAR_BIT(n,k)  (n &= ~(0x01<<k))
#define UTY_TOGGLE_BIT(n,k) (n ^= (0x01<<k))

/*
 * led7seg.c
 *
 *  Created on: May 18, 2022
 *      Author: ASUS
 */


#include <led7seg.h>
#include "stdio.h"
#include "stdint.h"
#include "r_gpio_rx_if.h"

const uint8_t LED7SEG_1_ID = 0;
const uint8_t LED7SEG_2_ID = 1;
const uint8_t LED7SEG_3_ID = 2;
const uint8_t LED7SEG_4_ID = 3;
const uint8_t LED7SEG_5_ID = 4;
const uint8_t LED7SEG_6_ID = 5;
const uint8_t LED7SEG_7_ID = 6;
const uint8_t LED7SEG_8_ID = 7;


uint16_t s_value595 = 0xFFFF;
const uint8_t LED7_CODE[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90,
							0xff, 0x8c, 0xaf, 0xf7, 0x88, 0x83, 0x86, 0xab, 0xa1, 0xbf, 0xef, 0xe9, 0xe3, 0xa3, 0x8e, 0x89};

const s_value595_bit_t LED7_SEGMENT[MAX_SEGMENT] = {SEGA_PIN_bit, SEGB_PIN_bit, SEGC_PIN_bit, SEGD_PIN_bit, SEGE_PIN_bit, SEGF_PIN_bit, SEGG_PIN_bit, SEGH_PIN_bit};
const s_value595_bit_t LED7_DIGIT[MAX_DIGIT] = {DIGIT1_PIN_bit, DIGIT2_PIN_bit, DIGIT3_PIN_bit, DIGIT4_PIN_bit, DIGIT6_PIN_bit, DIGIT7_PIN_bit, DIGIT8_PIN_bit, DIGIT5_PIN_bit};
static uint8_t Led7_Buffer[MAX_DIGIT] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};


//static bool led7IsOn = false;
uint16_t number1, number2, number3, number4 = 0;


static void _decode(uint8_t code);
static void _show(uint8_t digit);



void Led7seg_onSegment(uint8_t segment)
{
	UTY_CLEAR_BIT(s_value595, LED7_SEGMENT[segment]);
}

void Led7seg_offSegment(uint8_t segment)
{
	UTY_SET_BIT(s_value595, LED7_SEGMENT[segment]);
}

void Led7seg_onAllSegment(void)
{
	uint8_t i;
	for(i = 0; i < MAX_SEGMENT; i++)
		UTY_CLEAR_BIT(s_value595, LED7_SEGMENT[i]);
}

void Led7seg_offAllSegment(void)
{
	uint8_t i;
	for(i = 0; i < MAX_SEGMENT; i++)
		UTY_SET_BIT(s_value595, LED7_SEGMENT[i]);
}

void Led7seg_onDigit(uint8_t digit)
{
	UTY_SET_BIT(s_value595, LED7_DIGIT[digit]);
}

void Led7seg_offDigit(uint8_t digit)
{
	UTY_CLEAR_BIT(s_value595, LED7_DIGIT[digit]);
}

void Led7seg_onAllDigit(void)
{
	uint8_t i;
	for(i = 0; i < MAX_DIGIT; i++)
		UTY_SET_BIT(s_value595, LED7_DIGIT[i]);
}

void Led7seg_offAllDigit(void)
{
	uint8_t i;
	for(i = 0; i < MAX_DIGIT; i++)
		UTY_CLEAR_BIT(s_value595, LED7_DIGIT[i]);
}

static uint8_t ledIndex = 0;
void Led7seg_IRQHandler()
{
	Led7seg_offAllSegment();
	_show((s_value595_bit_t)ledIndex);
	_decode(Led7_Buffer[ledIndex]);
    if(++ledIndex == MAX_DIGIT) ledIndex = 0;
    int8_t i = 0;
	for (i=15; i>=0; i--){
		if(((s_value595) & (1<<i)) != 0)
		{
			DS_HC595 = 1U;
		}
		else
		{
			DS_HC595 = 0U;
		}
		SHCP_HC595 = 0U;
		SHCP_HC595 = 1U;
	}
	STCP_HC595 = 0U;
	STCP_HC595 = 1U;

}

void Led7seg_showCharAt(Specials_char_t character, uint8_t digit)
{
	Led7_Buffer[digit] = LED7_CODE[character];
}
void Led7seg_setAllLedChar(Specials_char_t character)
{
	for(int i = 0; i< MAX_DIGIT; i++)
	{
		Led7_Buffer[i] = LED7_CODE[character];
	}
}



//void Led7seg_turnOnLed()
//{
//	led7IsOn = true;
//}

//void Led7seg_turnOffLed()
//{
//	led7IsOn = false;
//	for(uint8_t i = 0; i < 3; i++)
//	{
//		Led7_Buffer[i] = LED7_CODE[CHAR_OFF_LED];
//	}
//}


/* ==================================================================== */
/* ======================= All private function ======================= */
/* ==================================================================== */
static void _decode(uint8_t code)
{
	uint8_t segment;
	for(segment = 0; segment < 8; segment++)
	{
		if(code & (0x01<<segment)) Led7seg_offSegment((s_value595_bit_t)segment);
		else Led7seg_onSegment((s_value595_bit_t)segment);
	}
}

static void _show(uint8_t digit)
{
	uint8_t i;
	for(i = 0; i < MAX_DIGIT; i++)
	{
		if(i == digit) Led7seg_onDigit((s_value595_bit_t)i);
		else Led7seg_offDigit((s_value595_bit_t)i);
	}
}

void led7seg_start()
{
	OE_HC595 = 0U;
	int16_t i;
	for (i=15; i>=0; i--)
	{
		if(((s_value595) & ((uint16_t)(1<<i))) != 0)
		{
			DS_HC595 |= 1U;
		}
		else
		{
			DS_HC595 = 0U;
		}
		SHCP_HC595 = 0U;
		SHCP_HC595 = 1U;
	}
	STCP_HC595 = 0U;
	STCP_HC595 = 1U;
}
void BSP_led7seg_all_write(uint8_t value_led1, uint8_t value_led2, uint8_t value_led3, uint8_t value_led4)
{
    uint8_t led1_chuc, led1_dv;
    uint8_t led2_chuc, led2_dv;
    uint8_t led3_chuc, led3_dv;
    uint8_t led4_chuc, led4_dv;
    if (value_led1 > 99) value_led1 = 99;
    if (value_led2 > 99) value_led2 = 99;
    if (value_led3 > 99) value_led3 = 99;
    if (value_led4 > 99) value_led4 = 99;

    // Caculate:
    led1_chuc = value_led1/10; led1_dv = value_led1 % 10;
    led2_chuc = value_led2/10; led2_dv = value_led2 % 10;
    led3_chuc = value_led3/10; led3_dv = value_led3 % 10 ;
    led4_chuc = value_led4/10; led4_dv = value_led4 % 10;

    // Update:
    Led7seg_showCharAt((Specials_char_t)led1_chuc, LED7SEG_1_ID);
    Led7seg_showCharAt((Specials_char_t)led1_dv, LED7SEG_2_ID);
    Led7seg_showCharAt((Specials_char_t)led2_chuc, LED7SEG_3_ID);
    Led7seg_showCharAt((Specials_char_t)led2_dv, LED7SEG_4_ID);
    Led7seg_showCharAt((Specials_char_t)led3_chuc, LED7SEG_5_ID);
    Led7seg_showCharAt((Specials_char_t)led3_dv, LED7SEG_6_ID);
    Led7seg_showCharAt((Specials_char_t)led4_chuc, LED7SEG_7_ID);
    Led7seg_showCharAt((Specials_char_t)led4_dv, LED7SEG_8_ID);

}
