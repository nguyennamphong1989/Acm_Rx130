/*
 * flash.h
 *
 *  Created on: May 25, 2022
 *      Author: ASUS
 */

#ifndef APPS_FLASH_FLASH_H_
#define APPS_FLASH_FLASH_H_

#include "r_flash_rx_if.h"
#include "r_flash_rx_config.h"


#define Time_Loi 		FLASH_DF_BLOCK_0
#define Time_May 		FLASH_DF_BLOCK_1
//#define TDS_PARAM_BLOCK     	FLASH_DF_BLOCK_2
//#define FILLTER_EXPIRE_BLOCK 	FLASH_DF_BLOCK_3
//#define CURENT_DATA_BLOCK 		FLASH_DF_BLOCK_4
//#define PRODUCT_ID_DATA_BLOCK 		FLASH_DF_BLOCK_5

#define MAX_LEN_FLASH       (1024) //1Kb
#define BYTE_SAVE_LEN      (2)
#define NUMBER_BYTE_VALID_MAX  (MAX_LEN_FLASH - BYTE_SAVE_LEN)

union data_len
{
	uint16_t len;
	uint8_t  len_byte[BYTE_SAVE_LEN];
};


void flash_app_init();
void flash_app_eraseBlock(flash_block_address_t blockAdress);
bool flash_app_writeBlock(uint8_t * data, flash_block_address_t blockAdress,uint16_t dataSize);
bool flash_app_readData(uint8_t* dataRead, flash_block_address_t blockAdress,uint16_t dataSize);
void flash_app_factoryReset();
void flash_data_write(flash_block_address_t blockAdd, void *tx_DATA, uint32_t len);
void flash_data_read(flash_block_address_t blockAdd, void *rx_DATA, uint32_t len);

#endif /* APPS_FLASH_FLASH_H_ */
