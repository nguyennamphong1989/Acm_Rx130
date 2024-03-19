/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : mcu_info.h
* Device(s)    : RX130
* Description  : Information about the MCU.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 01.12.2015 1.00     First release
*         : 01.10.2016 1.01     Added the following macro definition.
*                               - BSP_LPTSRCCLK_HZ
*         : 15.05.2017 1.02     Added the following macro definition.
*                               - BSP_MCU_RX130_512KB
*                               - BSP_PACKAGE_LFQFP100
*                               - BSP_ILOCO_HZ
*                               Added the following setting.
*                               - Setting of 100 pins.
*                               - Setting of 512-Kbyte ROM capacity.
*                               - Setting of 384-Kbyte ROM capacity.
*                               - Setting of 256-Kbyte ROM capacity.
*         : 01.11.2017 2.00     Added definition not select clock for LPT clock source.
*         : 28.02.2019 3.00     Added macro definition of MCU functions.
*                               Added the following macro definition.
*                               - BSP_MCU_CPU_VERSION
*                               - CPU_CYCLES_PER_LOOP
*                               Fixed coding style.
*         : 30.11.2021 3.01     Deleted the compile switch for BSP_CFG_MCU_PART_SERIES and BSP_CFG_MCU_PART_GROUP.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Gets MCU configuration information. */
#include "r_bsp_config.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* Multiple inclusion prevention macro */
#ifndef MCU_INFO
#define MCU_INFO

/* MCU CPU Version */
#define BSP_MCU_CPU_VERSION    (1)

/* CPU cycles. Known number of RXv1 CPU cycles required to execute the delay_wait() loop */
#define CPU_CYCLES_PER_LOOP    (5)

/* MCU Series. */
#define BSP_MCU_SERIES_RX100   (1)

/* This macro means that this MCU is part of the RX13x collection of MCUs (i.e. RX130). */
#define BSP_MCU_RX13_ALL       (1)

/* MCU Group name. */
#define BSP_MCU_RX130          (1)
#if (BSP_CFG_MCU_PART_MEMORY_SIZE == 0x6) || (BSP_CFG_MCU_PART_MEMORY_SIZE == 0x7) || \
    (BSP_CFG_MCU_PART_MEMORY_SIZE == 0x8) || (BSP_CFG_MCU_PART_PACKAGE == 0x5)
    #define BSP_MCU_RX130_512KB (1)
#endif

/* Package. */
#if   BSP_CFG_MCU_PART_PACKAGE == 0x5
    #define BSP_PACKAGE_LFQFP100    (1)
    #define BSP_PACKAGE_PINS        (100) 
#elif BSP_CFG_MCU_PART_PACKAGE == 0xB
    #define BSP_PACKAGE_LFQFP80    (1)
    #define BSP_PACKAGE_PINS        (80)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x0
    #define BSP_PACKAGE_LFQFP64    (1)
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x1
    #define BSP_PACKAGE_LQFP64     (1)
    #define BSP_PACKAGE_PINS        (64)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x3
    #define BSP_PACKAGE_LFQFP48    (1)
    #define BSP_PACKAGE_PINS        (48)
#elif BSP_CFG_MCU_PART_PACKAGE == 0x4
    #define BSP_PACKAGE_HWQFN48    (1)
    #define BSP_PACKAGE_PINS        (48)
#else
    #error "ERROR - BSP_CFG_MCU_PART_PACKAGE - Unknown package chosen in r_bsp_config.h"
#endif

/* Memory size of your MCU. */
#if   BSP_CFG_MCU_PART_MEMORY_SIZE == 0x3
    #define BSP_ROM_SIZE_BYTES              (65536L)
    #define BSP_RAM_SIZE_BYTES              (10240)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x5
    #define BSP_ROM_SIZE_BYTES              (131072L)
    #define BSP_RAM_SIZE_BYTES              (16384)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x6
    #define BSP_ROM_SIZE_BYTES              (262144L)
    #define BSP_RAM_SIZE_BYTES              (32768)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x7
    #define BSP_ROM_SIZE_BYTES              (393216L)
    #define BSP_RAM_SIZE_BYTES              (49152)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#elif BSP_CFG_MCU_PART_MEMORY_SIZE == 0x8
    #define BSP_ROM_SIZE_BYTES              (524288L)
    #define BSP_RAM_SIZE_BYTES              (49152)
    #define BSP_DATA_FLASH_SIZE_BYTES       (8192)
#else
    #error "ERROR - BSP_CFG_MCU_PART_MEMORY_SIZE - Unknown memory size chosen in r_bsp_config.h"
#endif

/* These macros define clock speeds for fixed speed clocks. */
#define BSP_LOCO_HZ                 (4000000)
#define BSP_HOCO_HZ                 (32000000)
#define BSP_SUB_CLOCK_HZ            (32768)
#define BSP_ILOCO_HZ                (15000)

/* Clock source select (CKSEL).
   0 = Low Speed On-Chip Oscillator  (LOCO)
   1 = High Speed On-Chip Oscillator (HOCO)
   2 = Main Clock Oscillator
   3 = Sub-Clock Oscillator
   4 = PLL Circuit
*/
#if   BSP_CFG_CLOCK_SOURCE == 0
    #define BSP_SELECTED_CLOCK_HZ   (BSP_LOCO_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 1
    #define BSP_SELECTED_CLOCK_HZ   (BSP_HOCO_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 2
    #define BSP_SELECTED_CLOCK_HZ   (BSP_CFG_XTAL_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 3
    #define BSP_SELECTED_CLOCK_HZ   (BSP_SUB_CLOCK_HZ)
#elif BSP_CFG_CLOCK_SOURCE == 4
    #define BSP_SELECTED_CLOCK_HZ   ((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL)
#else
    #error "ERROR - BSP_CFG_CLOCK_SOURCE - Unknown clock source chosen in r_bsp_config.h"
#endif

/* LPT clock speed in Hz */
#if   BSP_CFG_LPT_CLOCK_SOURCE == 0
    #define BSP_LPTSRCCLK_HZ        (BSP_SUB_CLOCK_HZ)
#elif BSP_CFG_LPT_CLOCK_SOURCE == 1
    #define BSP_LPTSRCCLK_HZ        (BSP_ILOCO_HZ)                              /* IWDTCLK typical frequency */
#elif BSP_CFG_LPT_CLOCK_SOURCE == 2
    /* LPT none use */    
#else
    #error "ERROR - BSP_CFG_LPT_CLOCK_SOURCE - Unknown lpt clock source chosen in r_bsp_config.h"
#endif

/* System clock speed in Hz. */
#define BSP_ICLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_ICK_DIV)
/* Peripheral Module Clock B speed in Hz. */
#define BSP_PCLKB_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKB_DIV)
/* Peripheral Module Clock D speed in Hz. */
#define BSP_PCLKD_HZ                (BSP_SELECTED_CLOCK_HZ / BSP_CFG_PCKD_DIV)
/* FlashIF clock speed in Hz. */
#define BSP_FCLK_HZ                 (BSP_SELECTED_CLOCK_HZ / BSP_CFG_FCK_DIV)

/* Null argument definitions. */
#define FIT_NO_FUNC                 ((void (*)(void *))0x10000000)  /* Reserved space on RX */
#define FIT_NO_PTR                  ((void *)0x10000000)            /* Reserved space on RX */

/* Mininum and maximum IPL levels available for this MCU. */
#define BSP_MCU_IPL_MAX             (0xF)
#define BSP_MCU_IPL_MIN             (0)

/* Maximum frequency on Middle-speed operating mode. */
#define BSP_MIDDLE_SPEED_MAX_FREQUENCY      (12000000)

/* MCU functions */
#define BSP_MCU_REGISTER_WRITE_PROTECTION
#define BSP_MCU_RCPC_PRC0
#define BSP_MCU_RCPC_PRC1
#define BSP_MCU_RCPC_PRC2
#define BSP_MCU_RCPC_PRC3
#define BSP_MCU_EXCEP_SUPERVISOR_INST_ISR
#define BSP_MCU_EXCEP_UNDEFINED_INST_ISR
#define BSP_MCU_NON_MASKABLE_ISR
#define BSP_MCU_UNDEFINED_INTERRUPT_SOURCE_ISR
#define BSP_MCU_BUS_ERROR_ISR

#define BSP_MCU_NMI_EXC_NMI_PIN
#define BSP_MCU_NMI_OSC_STOP_DETECT
#define BSP_MCU_NMI_IWDT_ERROR
#define BSP_MCU_NMI_LVD1
#define BSP_MCU_NMI_LVD2

#endif /* MCU_INFO */
