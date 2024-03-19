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
* File Name        : Config_CMT1_user.c
* Component Version: 2.3.0
* Device(s)        : R5F51305AxFM
* Description      : This file implements device driver for Config_CMT1.
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
#include "Config_CMT1.h"
/* Start user code for include. Do not edit comment generated here */
#include "led7seg.h"
#include "Config_PORT.h"
#include "timeCheck.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
volatile uint32_t g_sysTime = 0;
volatile uint8_t g_run1msFlag = 0;
volatile uint8_t s_200usTick = 0;

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_CMT1_Create_UserInit
* Description  : This function adds user code after initializing the CMT1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_CMT1_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_CMT1_cmi1_interrupt
* Description  : This function is CMI1 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_CMT1_CMI1
#pragma interrupt r_Config_CMT1_cmi1_interrupt(vect=VECT(CMT1,CMI1),fint)
#else
#pragma interrupt r_Config_CMT1_cmi1_interrupt(vect=VECT(CMT1,CMI1))
#endif
static void r_Config_CMT1_cmi1_interrupt(void)
{
    /* Start user code for r_Config_CMT1_cmi1_interrupt. Do not edit comment generated here */
	static uint32_t cnt_1s = 0;
	static uint8_t countWDG = 0;
	static bool toggleWDG = false;
	if(++s_200usTick == 5)
	{
		s_200usTick = 0;
		g_sysTime ++;
		g_run1msFlag = 1;
		Led7seg_IRQHandler();
	}
	if (++countWDG >= 50)
	{
		if (toggleWDG)
		{
			toggleWDG = false;
			PORT3.PODR.BIT.B2 = 0U;
		}
		else
		{
			toggleWDG = true;
			PORT3.PODR.BIT.B2 = 1U;
		}
		countWDG = 0;
	}
	if (++cnt_1s >= 5000) {
		cnt_1s = 0;
		BSP_utils_updateCounterForDevices();
	}
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
