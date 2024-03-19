/*
 * flash.c
 *
 *  Created on: May 25, 2022
 *      Author: ASUS
 */


#include "internal_flash.h"
#include <stdio.h>
#include <string.h>
#include "stdlib.h"

void flash_app_eraseAll()
{
	flash_err_t err;
	flash_res_t result;
    /* Erase all of data flash */
    err = R_FLASH_Erase(FLASH_DF_BLOCK_0, FLASH_NUM_BLOCKS_DF);
    if (err != FLASH_SUCCESS)
    {
        while(1) ;
    }

    /* Verify erased */
    err = R_FLASH_BlankCheck(FLASH_DF_BLOCK_0, FLASH_DF_FULL_SIZE, &result);
    if ((err != FLASH_SUCCESS) || (result != FLASH_RES_BLANK))
    {
        while(1) ;
    }
}
/**
 * @brief One line documentation
 *
 * A more detailed documentation
 *
 * @param arg1 the first function argument
 * @param arg2 the second function argument
 *
 * @return descrition for the function return value
 */
void flash_app_init()
{
    flash_err_t err;

    /* Open driver */
    err = R_FLASH_Open();
    if (err != FLASH_SUCCESS)
        while(1);
//    flash_app_eraseAll();
}

void flash_app_factoryReset()
{


}
void flash_app_eraseBlock(flash_block_address_t blockAdress)
{
	flash_err_t err;
	flash_res_t result;
    err = R_FLASH_Erase(blockAdress, 1);
    if (err != FLASH_SUCCESS)
    {
        while(1) ;
    }

    /* Verify erased */
    err = R_FLASH_BlankCheck(blockAdress, FLASH_DF_BLOCK_SIZE, &result);
    if ((err != FLASH_SUCCESS) || (result != FLASH_RES_BLANK))
    {
        while(1) ;
    }
}

bool flash_app_writeBlock(uint8_t * data, flash_block_address_t blockAdress,uint16_t dataSize)
{
    uint32_t    addr, i;
    flash_err_t err;
    union data_len len;
    uint8_t * data_write;
    if(dataSize>= NUMBER_BYTE_VALID_MAX) return false;
    flash_app_eraseBlock(blockAdress);
    data_write =  malloc(dataSize +BYTE_SAVE_LEN);
    if(data_write == NULL ) return false;
    len.len = dataSize;
    memcpy(data_write,len.len_byte,BYTE_SAVE_LEN);
    memcpy(data_write+BYTE_SAVE_LEN,data,dataSize);
    addr = blockAdress;
    err = R_FLASH_Write((uint32_t)data_write, addr, FLASH_DF_BLOCK_SIZE);
    if(err != FLASH_SUCCESS)
    {
        while(1) ;
    }

    /* Verify data write */
    for (i=0; i < dataSize+BYTE_SAVE_LEN; i++)
    {
        if (*(data_write +i) != *((uint8_t *)(addr + i)))
            while(1);
    }
    free(data_write);
    return true;
}

bool flash_app_readData(uint8_t* dataRead, flash_block_address_t blockAdress,uint16_t dataSize)
{
    flash_err_t err;
    flash_res_t result;
    union data_len len;
    int i;
    if(dataSize>= NUMBER_BYTE_VALID_MAX) return false;
    err = R_FLASH_BlankCheck(blockAdress, FLASH_DF_BLOCK_SIZE, &result);
    if (err != FLASH_SUCCESS)
        while(1) ;
    if(result == FLASH_RES_BLANK)
    	return false;
    for (i=0;i<BYTE_SAVE_LEN;i++)
    {
    	len.len_byte[i] = *((uint8_t *)(blockAdress + i));;
    }
    if (len.len != dataSize)
    {
    	return false;
    }
    for ( i=0; i < dataSize; i ++)
    {
        *(dataRead + i) = *((uint8_t *)(blockAdress + i+BYTE_SAVE_LEN));
    }

    return true;
}

void flash_data_write(flash_block_address_t blockAdd, void *tx_DATA, uint32_t len)
{
	uint8_t *p;
	p = (uint8_t*)tx_DATA;

	flash_app_eraseBlock(blockAdd);
	flash_app_writeBlock(p, blockAdd, len);
}

void flash_data_read(flash_block_address_t blockAdd, void *rx_DATA, uint32_t len)
{
	uint8_t *p;
	p = (uint8_t*)rx_DATA;
	flash_app_readData(p, blockAdd, len);
}
