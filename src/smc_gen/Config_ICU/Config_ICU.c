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
* File Name        : Config_ICU.c
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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_ICU_Create
* Description  : This function initializes the ICU module
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ICU_Create(void)
{
    /* Disable IRQ interrupts */
    ICU.IER[0x08].BYTE = _00_ICU_IRQ0_DISABLE | _00_ICU_IRQ1_DISABLE | _00_ICU_IRQ2_DISABLE | _00_ICU_IRQ3_DISABLE |
                         _00_ICU_IRQ4_DISABLE | _00_ICU_IRQ5_DISABLE | _00_ICU_IRQ6_DISABLE | _00_ICU_IRQ7_DISABLE;

    /* Disable software interrupt */
    IEN(ICU,SWINT) = 0U;

    /* Disable IRQ0~7 digital filter */
    ICU.IRQFLTE0.BYTE &= ~(_20_ICU_IRQ5_FILTER_ENABLE);

    /* Set IRQ5 pin */
    MPC.P15PFS.BYTE = 0x40U;
    PORT1.PDR.BYTE &= 0xDFU;
    PORT1.PMR.BYTE &= 0xDFU;

    /* Set IRQ detection type */
    ICU.IRQCR[5].BYTE = _0C_ICU_IRQ_EDGE_BOTH;
    IR(ICU,IRQ5) = 0U;

    /* Set IRQ5 priority level */
    IPR(ICU,IRQ5) = _0F_ICU_PRIORITY_LEVEL15;

    R_Config_ICU_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_ICU_IRQ5_Start
* Description  : This function enables IRQ5 interrupt
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ICU_IRQ5_Start(void)
{
    /* Enable IRQ5 interrupt */
    IEN(ICU,IRQ5) = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_ICU_IRQ5_Stop
* Description  : This function disables IRQ5 interrupt
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_ICU_IRQ5_Stop(void)
{
    /* Disable IRQ5 interrupt */
    IEN(ICU,IRQ5) = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
