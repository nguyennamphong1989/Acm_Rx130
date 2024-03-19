/*
 * apl_config.c
 *
 *  Created on: Jul 28, 2022
 *      Author: Lover_boy_9x
 */

#include "apl_config.h"
#include "apl_status.h"
#include "stdbool.h"
#include "internal_flash.h"
static CFG_AIRC_t g_cfg_airc;
void APL_cfg_init(void)
{

	flash_app_init();
	CFG_AIRC_t *p_cfg = &g_cfg_airc;
	bool readOk = flash_app_readData((uint8_t*)&g_cfg_airc, FLASH_DF_BLOCK_0,sizeof(g_cfg_airc));
	if (!readOk)
	{
		p_cfg->thr_temp_1 = AIRC_THR_TEMP_1_DEFALT;
		p_cfg->thr_temp_2 = AIRC_THR_TEMP_2_DEFALT;
		p_cfg->thr_temp_3 = AIRC_THR_TEMP_3_DEFALT;
		p_cfg->thr_temp_4 = AIRC_THR_TEMP_4_DEFALT;
		p_cfg->acmControlAuto = AIRC_AUTO_ENABLE;
		p_cfg->thr_temp_delta = 2;
		p_cfg->T_threshold = 30;
		p_cfg->minCurrent = 400;
		p_cfg->acmRunTime_1 = 4;
		p_cfg->acmRunTime_2 = 4;
	}
	flash_app_writeBlock((uint8_t*)&g_cfg_airc, FLASH_DF_BLOCK_0,sizeof(g_cfg_airc));
}
void APL_cfg_thresholds_temp_update(uint8_t thr_temp_1, uint8_t thr_temp_2, uint8_t thr_temp_3, uint8_t thr_temp_4, uint8_t thr_temp_delta)
{
	CFG_AIRC_t *p_cfg = &g_cfg_airc;
	// Verify params:
	if ((thr_temp_1 > AIRC_TEMP_MAX) ||
		(thr_temp_2 > AIRC_TEMP_MAX) ||
		(thr_temp_3 > AIRC_TEMP_MAX) ||
		(thr_temp_4 > AIRC_TEMP_MAX)) {
		goto end_proc;
	}

	// Check value changed: only update & store if having change.
	if ((p_cfg->thr_temp_1 != thr_temp_1) || (p_cfg->thr_temp_2 != thr_temp_2)
		 || (p_cfg->thr_temp_3 != thr_temp_3) || (p_cfg->thr_temp_4 != thr_temp_4) || (p_cfg->thr_temp_delta != thr_temp_delta)) {

		// Update:
		p_cfg->thr_temp_1 = thr_temp_1;
		p_cfg->thr_temp_2 = thr_temp_2;
		p_cfg->thr_temp_3 = thr_temp_3;
		p_cfg->thr_temp_4 = thr_temp_4;
		p_cfg->thr_temp_delta = thr_temp_delta;

		// Store:
		flash_app_writeBlock((uint8_t*)&g_cfg_airc, FLASH_DF_BLOCK_0,sizeof(g_cfg_airc));
	}

	end_proc:;
}
void APL_cfg_Store()
{

	AIRC_STATUS_t *p_stt = APL_stt_get();
	// Store:
	flash_app_writeBlock((uint8_t*)&g_cfg_airc, FLASH_DF_BLOCK_0,sizeof(g_cfg_airc));
}

CFG_AIRC_t *APL_cfg_get(void)
{
	return &g_cfg_airc;
}
