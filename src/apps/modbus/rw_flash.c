/*
 * rw_flash.c
 *
 *  Created on: May 30, 2023
 *      Author: Lover_boy_9x
 */
#include "rw_flash.h"

typedef enum
{
    MSG_CODE_READ_NUM_WORD = 0x10,
	MSG_CODE_RESPONE_READ_NUM_WORD = 0x11,
    MSG_CODE_READ_WORD = 0x12,
	MSG_CODE_RESPONE_READ_WORD = 0x13,
	MSG_CODE_WRITE_NUM_WORD = 0x20,
	MSG_CODE_RESPONE_WRITE_NUM_WORD = 0x21,
	MSG_CODE_WRITE_WORD = 0x22,
	MSG_CODE_RESPONE_WRITE_WORD = 0x23,
} msg_code_t;
uint8_t msgCodeList[8][2] = {
{     MSG_CODE_READ_NUM_WORD,2},
{     MSG_CODE_RESPONE_READ_NUM_WORD,2},
{     MSG_CODE_READ_WORD,6},
{	MSG_CODE_RESPONE_READ_WORD, 2},
{     MSG_CODE_WRITE_NUM_WORD,4},
{     MSG_CODE_RESPONE_WRITE_NUM_WORD,2},
{	MSG_CODE_WRITE_WORD, 6},
{	MSG_CODE_RESPONE_WRITE_WORD, 2}

};
extern uint16_t p[1000];
extern uint16_t learn_command_state;
uint8_t uartVoice_calculateChecksum(uint8_t *buf, uint8_t len)
{
    uint8_t ret = 0;
    uint8_t i = 0;
    for(i = 0; i < len; i++)
    {
        ret += buf[i];
    }
    return ret;
}
void uartVoice_sendFrame(uint8_t codemsg, uint8_t* data, uint16_t len)
{
	char bufSend[MAX_LEN_BUFFER_SEND];
	uint16_t lenFr = 0;
	bufSend[lenFr++] = MSG_HEADER;
	bufSend[lenFr++] = codemsg;
	bufSend[lenFr++] = len;
	if(data != NULL && len != 0 && (len <= MAX_LEN_BUFFER_SEND -2))
	{
	   memcpy(bufSend+lenFr, data, len);
	   lenFr +=len;
	}
	uint8_t cks = uartVoice_calculateChecksum((uint8_t*)bufSend, lenFr);
	bufSend[lenFr++] = cks;
	bufSend[lenFr++] = MSG_TEMINATE;
	PULL_DE_HIGH;
	DELAY_RS485;
	UART_sendFrame((uint8_t*)bufSend, lenFr);
	DELAY_RS485;
	PULL_DE_LOW;
}
void uartVoice_send2byte(uint8_t codemsg, uint16_t data)
{
	uartVoice_sendFrame(codemsg, (uint8_t*) &data, 2);
}
int uartVoice_getMsgIdx(uint8_t msgCode)
{
    uint8_t i = 0;
    for(i = 0; i < MSG_IDX_MAX; i++)
    {
        if(msgCodeList[i][0] == msgCode)
        {
            return i;
        }
    }
    return -1;
}
void uartVoice_processFlashCommand(uint8_t* command, uint16_t length)
{
	static uint8_t idx_command_read = 0;
	static bool enable_read = false;
	static uint16_t len_read;

	static uint8_t idx_command_write = 0;
	static bool enable_write = false;
	static uint16_t temp_len_write_num = 0;
	static uint16_t num_byte_write = 0;
	if (learn_command_state == LEARN_COMMAND_ENTER) return;
    if(command[0] == MSG_HEADER && command[length - 1] == MSG_TEMINATE)
    {
        //uint8_t lenData = command[2];
        uint8_t* pDataStart = &command[3];
        uint8_t cks = uartVoice_calculateChecksum(command, length - 2);  //k tinh byte terminate va cks
        if(cks == command[length - 2])
        {
            int msgIdx = uartVoice_getMsgIdx(command[1]);
            switch (command[1]) {
				case MSG_CODE_READ_NUM_WORD:
					if ((*pDataStart >= 1) && (*pDataStart <= 40))
					{
						uint32_t add = (*pDataStart + 10) * 0x10000;
						deviceFlash_readData(add, (uint8_t*)&len_read, 2);
						if (len_read > 1000)
						{
							enable_read = false;
							uartVoice_send2byte(MSG_CODE_RESPONE_READ_NUM_WORD, 0);
							return;
						}
						enable_read = true;
						idx_command_read = *pDataStart;
						deviceFlash_readData(add + 0x1000, (uint8_t*)p, 2 * len_read);
						uartVoice_sendFrame(MSG_CODE_RESPONE_READ_NUM_WORD, (uint8_t*) &len_read, 2);
					}
					else
					{
						enable_read = false;
					}
					break;
				case MSG_CODE_READ_WORD:
					if ((idx_command_read == *pDataStart) && ((*pDataStart >= 1) && (*pDataStart <= 40)))
					{
						if (enable_read == true)
						{
							uint8_t len_respone_read = *(pDataStart+4);
							uint16_t start_add_respone = *((uint16_t*)(pDataStart+2));
							if (((start_add_respone +  len_respone_read) <= len_read) && (start_add_respone < 1000) && (len_respone_read < 21) && (len_respone_read > 0))
							{
								uint8_t *ptn_to_start = (uint8_t*)p + start_add_respone * 2;
								uartVoice_sendFrame(MSG_CODE_RESPONE_READ_WORD, ptn_to_start, 2 * len_respone_read);
							}
							else
							{
								enable_read = false;
							}
						}
					}
					else
					{
						enable_read = false;
					}
					break;
				case MSG_CODE_WRITE_NUM_WORD:
					if ((*pDataStart >= 1) && (*pDataStart <= 40))
					{
						uint16_t len_write_num = *((uint16_t*)(pDataStart+2));
						if ((len_write_num > 0) && (len_write_num < 1000))
						{
							idx_command_write = *pDataStart;
							uint32_t add_write = (*pDataStart  + 10) * 0x10000;
							deviceFlash_eraseSector(add_write);
							deviceFlash_writeData(add_write, &len_write_num, 2);
							uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_NUM_WORD, 0x0001);
							enable_write = true;
							temp_len_write_num = len_write_num;
							num_byte_write = 0;

						}
						else
						{
							enable_write = false;
							uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_NUM_WORD, 0x0000);
						}
					}
					else
					{
						enable_write = false;
						uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_NUM_WORD, 0x0000);
					}
					break;
				case MSG_CODE_WRITE_WORD:
					if ((idx_command_write == *pDataStart) && ((*pDataStart >= 1) && (*pDataStart <= 40)))
					{
						if (enable_write)
						{
							if (num_byte_write == *(uint16_t*) (pDataStart + 2))
							{
								uint8_t len_respone_write = *(pDataStart+4);
								if (((num_byte_write + len_respone_write) < temp_len_write_num) && (len_respone_write < 21) && (len_respone_write > 0))
								{
									memcpy(p + num_byte_write, pDataStart + 6, len_respone_write * 2);
									uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0001);
									num_byte_write += len_respone_write;
								}
								else if  (((num_byte_write + len_respone_write) == temp_len_write_num) && (len_respone_write < 21) && (len_respone_write > 0))
								{
									memcpy(p + num_byte_write, pDataStart + 6, len_respone_write * 2);
									deviceFlash_eraseSector((idx_command_write + 10)* 0x10000 + 0x1000);
									deviceFlash_writeData((idx_command_write + 10)* 0x10000 + 0x1000, p, temp_len_write_num * 2);
									uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0001);
									enable_write = false;
								}
								else
								{
									enable_write = false;
									uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0000);
								}
							}
							else
							{
								enable_write = false;
								uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0000);
							}
						}
						else
						{
							enable_write = false;
							uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0000);
						}
					}
					else
					{
						enable_write = false;
						uartVoice_send2byte(MSG_CODE_RESPONE_WRITE_WORD, 0x0000);
					}
					break;
			}
        }
    }
}
void uartVoice_processFlashFormat(uint8_t *buffer_rx, uint16_t length)
{
	uint16_t idx = 0;
	if(length < 5) return;  //hien chua co lenh nao <4 byte
	while(idx < length - 2)
	{
		if(buffer_rx[idx] == MSG_HEADER)
		{
			int msgIdx = uartVoice_getMsgIdx(buffer_rx[idx+1]);
			if(msgIdx != -1)
			{
				uint16_t lengthExpect = msgCodeList[msgIdx][1];       //len expect
				if((lengthExpect == buffer_rx[idx+2]))
				{
					if(length < (idx+5 + lengthExpect)) return;  //+1header+1msgcode+1len(n)+1cks+1terminate

					uartVoice_processFlashCommand(&buffer_rx[idx], lengthExpect+5);
					idx = idx+5+lengthExpect;
				}
				else if ((buffer_rx[idx+1] == MSG_CODE_WRITE_WORD) && ((buffer_rx[idx+2] & 1) == 0) && (buffer_rx[idx+2] > lengthExpect) && (buffer_rx[idx+2] <= 46))
				{
					if(length < (idx+5 + buffer_rx[idx+2])) return;
					uartVoice_processFlashCommand(&buffer_rx[idx], buffer_rx[idx+2]+5);
					idx = idx+5+buffer_rx[idx+2];
				}
				else
				{
					idx++;
				}
			}
			else
			{
				idx++;
			}

		}
		else {
			idx++;
		}
	}
}

