/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_CMT0_user.c
* Component Version: 2.3.0
* Device(s)        : R5F51305AxFM
* Description      : This file implements device driver for Config_CMT0.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_CMT0.h"
/* Start user code for include. Do not edit comment generated here */
#include "Config_MTU3.h"
#include "Config_MTU0.h"
#include "Config_MTU2.h"
#include "Config_MTU4.h"
#include "stdint.h"
#include "stdbool.h"
#include "control.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint16_t *IRdata;
uint16_t IRlength;
uint16_t IRcount = 0;
static volatile uint8_t IR_txdone;
static uint8_t acIndex;
extern uint16_t IRidx;
extern volatile uint32_t g_sysTime;
extern uint32_t timeOutIRremote;
extern uint16_t learn_command_state;
extern uint16_t count;
extern volatile bool checkResetReetech;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_CMT0_Create_UserInit
* Description  : This function adds user code after initializing the CMT0 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT0_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_CMT0_cmi0_interrupt
* Description  : This function is CMI0 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_CMT0_CMI0
#pragma interrupt r_Config_CMT0_cmi0_interrupt(vect=VECT(CMT0,CMI0),fint)
#else
#pragma interrupt r_Config_CMT0_cmi0_interrupt(vect=VECT(CMT0,CMI0))
#endif
static void r_Config_CMT0_cmi0_interrupt(void)
{
    /* Start user code for r_Config_CMT0_cmi0_interrupt. Do not edit comment generated here */
	if (learn_command_state == LEARN_COMMAND_ENTER)
	{
		checkResetReetech = true;
		CMT0.CMCNT = 0;
		R_Config_CMT0_Stop();
		return;
	}
    if (!(IRcount%2))
    {
        if (acIndex == 0)
        {
            R_Config_MTU2_Stop();
            MTU2.TIOR.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
        }
        else if (acIndex == 1)
        {
            MTU.TRWER.BIT.RWE = 1U;
            R_Config_MTU4_Stop();
            MTU4.TIORH.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
            MTU.TRWER.BIT.RWE = 0U;
        }
        else if (acIndex == 2)
        {
            MTU.TRWER.BIT.RWE = 1U;
            R_Config_MTU3_Stop();
            MTU3.TIORH.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
            MTU.TRWER.BIT.RWE = 0U;
        }
        else if (acIndex == 3)
        {
            R_Config_MTU0_Stop();
            MTU0.TIORL.BYTE = _02_MTU_IOC_LH | _50_MTU_IOD_HL;
        }
    }
    else
    {
        if (acIndex == 0)  R_Config_MTU2_Start();
        else if (acIndex == 1) R_Config_MTU4_Start();
        else if (acIndex == 2) R_Config_MTU3_Start();
        else if (acIndex == 3) R_Config_MTU0_Start();
    }
    if (++IRcount < IRlength)
    {
        CMT0.CMCOR = (uint16_t) *(IRdata+IRcount);
        CMT0.CMCNT = 0U;
    }
    else
    {
        MTU.TRWER.BIT.RWE = 1U;
        R_Config_MTU3_Stop();
        MTU3.TIORH.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
        MTU.TRWER.BIT.RWE = 0U;

        R_Config_MTU2_Stop();
        MTU2.TIOR.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;

        MTU.TRWER.BIT.RWE = 1U;
        R_Config_MTU4_Stop();
        MTU4.TIORH.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
        MTU.TRWER.BIT.RWE = 0U;

        R_Config_MTU0_Stop();
        MTU0.TIORL.BYTE = _02_MTU_IOC_LH | _50_MTU_IOD_HL;

        R_Config_CMT0_Stop();
        IR_txdone = 1U;
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
void IR_Serial_Send(uint16_t* pulse, uint16_t len, uint8_t Index)
{
	if (Index >= 4) return;
	IR_txdone = 0U;
	IRdata = pulse;
	IRlength = len;
	if ((IRlength > 1000) || IRlength == 0)  return;
	IRcount = 0;

	acIndex = Index;
	CMT0.CMCOR = (uint16_t) *IRdata;
	CMT0.CMCNT = 0U;
	R_Config_CMT0_Start();
	switch (acIndex) {
		case 0:
			R_Config_MTU2_Start();
			break;
		case 1:
			R_Config_MTU4_Start();
			break;
		case 2:
			R_Config_MTU3_Start();
			break;
		case 3:
			R_Config_MTU0_Start();
			break;
		default:
			break;
	}
	while(0U == IR_txdone);
}
/* End user code. Do not edit comment generated here */
