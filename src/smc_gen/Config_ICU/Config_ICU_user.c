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
* File Name        : Config_ICU_user.c
* Component Version: 2.3.0
* Device(s)        : R5F51305AxFM
* Description      : This file implements device driver for Config_ICU.
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
#include "Config_ICU.h"
/* Start user code for include. Do not edit comment generated here */
#include "Config_CMT0.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint8_t testICU  = 0;
uint16_t buffer_IRrx[1000] = {0},  *poinerIR = buffer_IRrx;
uint16_t IRidx = 0;
uint16_t count = 0;
extern volatile uint32_t g_sysTime;
uint32_t timeOutIRremote = 0;
extern volatile bool checkResetReetech;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_ICU_Create_UserInit
* Description  : This function adds user code after initializing the ICU module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ICU_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_ICU_irq5_interrupt
* Description  : This function is IRQ5 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_ICU_IRQ5
#pragma interrupt r_Config_ICU_irq5_interrupt(vect=VECT(ICU,IRQ5),fint)
#else
#pragma interrupt r_Config_ICU_irq5_interrupt(vect=VECT(ICU,IRQ5))
#endif
static void r_Config_ICU_irq5_interrupt(void)
{
    /* Start user code for r_Config_ICU_irq5_interrupt. Do not edit comment generated here */
	if (checkResetReetech == true)
	{
		IRidx = 0; timeOutIRremote = g_sysTime; count = 0;
		checkResetReetech = false;
	}
	timeOutIRremote = g_sysTime;
	R_Config_CMT0_Stop();
	if((IRidx > 0) && (IRidx <= 1000))
	{
		*(poinerIR + IRidx - 1) = CMT0.CMCNT;
	}
	IRidx++; count = IRidx - 1;
	CMT0.CMCNT = 0U;
	R_Config_CMT0_Start();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

