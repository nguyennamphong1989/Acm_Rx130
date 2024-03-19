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
* File Name        : Config_MTU2.c
* Component Version: 1.12.0
* Device(s)        : R5F51305AxFM
* Description      : This file implements device driver for Config_MTU2.
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
#include "Config_MTU2.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU2_Create
* Description  : This function initializes the MTU2 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU2_Create(void)
{
    /* Release MTU channel 2 from stop state */
    MSTP(MTU2) = 0U;

    /* Stop MTU channel 2 counter */
    MTU.TSTR.BIT.CST2 = 0U;

    /* MTU channel 2 is used as PWM mode 1 */
    MTU.TSYR.BIT.SYNC2 = 0U;
    MTU2.TCR.BYTE = _00_MTU_PCLK_1 | _20_MTU_CKCL_A;
    MTU2.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU2.TMDR.BYTE = _02_MTU_PWM1;
    MTU2.TIOR.BYTE = _02_MTU_IOA_LH | _50_MTU_IOB_HL;
    MTU2.TGRA = _0349_TGRA2_VALUE;
    MTU2.TGRB = _01A4_TGRB2_VALUE;

    /* Set MTIOC2A pin */
    MPC.PB5PFS.BYTE = 0x01U;
    PORTB.PMR.BYTE |= 0x20U;

    R_Config_MTU2_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU2_Start
* Description  : This function starts the MTU2 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU2_Start(void)
{
    /* Start MTU channel 2 counter */
    MTU.TSTR.BIT.CST2 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU2_Stop
* Description  : This function stops the MTU2 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU2_Stop(void)
{
    /* Stop MTU channel 2 counter */
    MTU.TSTR.BIT.CST2 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
