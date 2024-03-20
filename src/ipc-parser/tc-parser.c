/****************************************************************************************
 *   FileName    : tc-parser.c
 *   Description : tc-parser.c
 ****************************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips Inc.
 *   All rights reserved 
 
This source code contains confidential information of Telechips.
Any unauthorized use without a written permission of Telechips including not limited 
to re-distribution in source or binary form is strictly prohibited.
This source code is provided ¡°AS IS¡± and nothing contained in this source code 
shall constitute any express or implied warranty of any kind, including without limitation, 
any warranty of merchantability, fitness for a particular purpose or non-infringement of any patent, 
copyright or other third party intellectual property right. 
No warranty is made, express or implied, regarding the information¡¯s accuracy, 
completeness, or performance. 
In no event shall Telechips be liable for any claim, damages or other liability arising from, 
out of or in connection with this source code or the use in the source code. 
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement 
between Telechips and Company.
*
****************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "tc-ipc-ctrl-log.h"
#include "tc-ipc-ctrl.h"
#include "tc-crc.h"
#include "tc-ipc-protocol.h"
#include "tc-parser.h"

enum DecodeState
{
	StateIdle,
	StateSync,
	StateStart,
	StateHader,
	StateData,
	StateCRC
};

static void tc_parser_clear(parserHandle *handle)
{
	handle->parserState = (int32_t)StateIdle;
	handle->parserData.cmd1= 0;
	handle->parserData.cmd2= 0;
	handle->parserData.length = 0;
	handle->parserData.data = NULL;
}

static void tc_parser_put_data(uint8_t*buffer, const uint8_t *handle, uint32_t sizeData)
{
	if(sizeData > (uint32_t)0)
	{
		(void)memcpy(buffer, handle, sizeData);
	}
}

void ipc_parser_resister_callback(parserHandle *handle, IPC_ReceivedData_cb cb)
{
	if(NULL != cb)
	{
	   handle->parser_ReceivedData = cb;
	}
}


parserHandle *ipc_parser_handle_init(void)
{
	parserHandle *handle;

	handle = malloc(sizeof(parserHandle));
	if(handle != NULL)
	{
		handle->offset = (uint32_t)0;
		handle->dataSize = 0;
		handle->maxSize = (uint16_t)TMM_PARSER_BUF_SIZE;
		handle->parserState = (int32_t)StateIdle;
		memset(handle->data, 0x00,TMM_PARSER_BUF_SIZE);
	}
	return handle;
}

void ipc_parser_handle_deinit(parserHandle *handle)
{
	if(handle != NULL)
	{
		free(handle);
	}
}

uint32_t tc_parser_freeSpace(parserHandle *handle)
{
	uint32_t freeSpace=0;
	if(handle != NULL)
	{
		freeSpace = handle->maxSize - (handle->offset + handle->dataSize);
	}
	return freeSpace;
}

uint8_t * tc_parser_BufAddr(parserHandle *handle)
{
	uint8_t *buf=NULL;
	if(handle != NULL)
	{
		buf = &handle->data[handle->offset];
	}
	return buf;
}

void tc_parser_push_size(parserHandle *handle, uint32_t size)
{
	if(handle != NULL)
	{
		handle->dataSize += size;
	}
}

void tc_parser_decoder(parserHandle *handle)
{
	if(handle != NULL)
	{
		uint32_t stop =0;
		uint32_t i;
		uint32_t remainSize;
		uint8_t * inBuffer ;
		uint16_t inBufferLen;

		while((stop == (uint32_t)0)
			&&(handle->offset < handle->dataSize)
			&&((uint32_t)IPC_MINIMUM_PACKET_SIZE <= (handle->dataSize-handle->offset)))
		{

			{
				int j;
				DEBUG_LIBIPC_PRINTF("handle->parserState = %d\n", handle->parserState);
				DEBUG_LIBIPC_PRINTF("cmd1 (0x%04x)\n",handle->parserData.cmd1);
				DEBUG_LIBIPC_PRINTF("cmd2 (0x%04x)\n",handle->parserData.cmd2);
			}


			inBuffer = &handle->data[handle->offset];
			inBufferLen = (uint16_t)(handle->dataSize - handle->offset);

			if(handle->parserState == (int32_t)StateIdle)
			{
				if(inBuffer[IPC_PROTOCOL_SYNC_OFFSET] == (uint8_t)IPC_SYNC_CODE)
				{
					handle->parserState = (int32_t)StateSync;
				}
				else
				{
					handle->offset++;
				}
			}
			else if(handle->parserState == (int32_t)StateSync)
			{
				if(inBuffer[IPC_PROTOCOL_FIRST_START_OFFSET] == (uint8_t)IPC_FIRST_START_CODE)
				{
					handle->parserState = (int32_t)StateStart;
				}
				else
				{
					handle->parserState = (int32_t)StateIdle;
					handle->offset++;
				}
			}
			else if(handle->parserState == (int32_t)StateStart)
			{

				if(inBuffer[IPC_PROTOCOL_SECOND_START_OFFSET] == (uint8_t)IPC_SECOND_START_CODE)
				{
					handle->parserState =(int32_t) StateHader;
				}
				else
				{
					handle->parserState = (int32_t)StateIdle;
					handle->offset++;
				}
			}
			else if(handle->parserState == (int32_t)StateHader)
			{
				handle->parserData.cmd1 = ((uint16_t)inBuffer[IPC_PROTOCOL_COMMAND1_HIGH_OFFSET] << 8) | inBuffer[IPC_PROTOCOL_COMMAND1_LOW_OFFSET];
				handle->parserData.cmd2 = ((uint16_t)inBuffer[IPC_PROTOCOL_COMMAND2_HIGH_OFFSET] << 8) | inBuffer[IPC_PROTOCOL_COMMAND2_LOW_OFFSET];
				handle->parserData.length = ((uint16_t)inBuffer[IPC_PROTOCOL_LENGTH_HIGH_OFFSET] << 8) | inBuffer[IPC_PROTOCOL_LENGTH_LOW_OFFSET];
				handle->parserState = (int32_t)StateData;
			}
			else if(handle->parserState == (int32_t)StateData)
			{
				uint16_t ipcDataSize;
				if(handle->parserData.length > (uint16_t)0)
				{
					ipcDataSize = handle->parserData.length - (uint16_t)1;	//for dummy data
				}
				else
				{
					ipcDataSize = 0;
				}

				if( inBufferLen >= (ipcDataSize + (uint16_t)IPC_MINIMUM_PACKET_SIZE))
				{
					if(handle->parserData.length == (uint16_t)0)
					{
						handle->parserData.data = NULL;
					}
					else
					{
						handle->parserData.data = &inBuffer[IPC_PROTOCOL_DATA_OFFSET];
					}
					handle->parserState = (int32_t)StateCRC;
				}
				else
				{
					stop = 1;
				}
			}
			else if(handle->parserState == (int32_t)StateCRC)
			{
				uint16_t crcOffset;

				if(handle->parserData.length > (uint16_t)0)
				{
					crcOffset = (uint16_t)(handle->parserData.length + (uint16_t)IPC_PROTOCOL_DATA_OFFSET);
				}
				else
				{
					/* for dummy */
					crcOffset = (uint16_t)IPC_PROTOCOL_DATA_OFFSET+(uint16_t)1;
				}

				if( inBufferLen >= (crcOffset + (uint16_t)2))
				{
					uint16_t crcSrc = ((uint16_t)inBuffer[crcOffset] << 8) | inBuffer[crcOffset+(uint16_t)1];
					uint16_t crcDst = tc_CRC16((const char *)inBuffer, crcOffset, 0);
					if(crcSrc == crcDst)
					{
						if(handle->parser_ReceivedData != NULL)
						{
							{
								int j;
								DEBUG_LIBIPC_PRINTF("call parser_ReceivedData\n");
								DEBUG_LIBIPC_PRINTF("cmd1 (0x%04x)\n",handle->parserData.cmd1);
								DEBUG_LIBIPC_PRINTF("cmd2 (0x%04x)\n",handle->parserData.cmd2);
								DEBUG_LIBIPC_PRINTF("lenght %d\n",handle->parserData.length);
								for(j=0; j< handle->parserData.length; j++)
								{
									DEBUG_LIBIPC_PRINTF("data[%d] = (0x%02x)\n",handle->parserData.data[j]);
								}
							}
							handle->parser_ReceivedData(&handle->parserData);
						}
					}
					tc_parser_clear(handle);
					handle->offset += (crcOffset + (uint32_t)2);
				}
				else
				{
					stop =1;
				}
			}
			else
			{
				/* Intentionally Empty */
			}
		}

		remainSize = handle->dataSize - handle->offset;
		for(i= (uint32_t)0; i<remainSize; i++)
		{
			handle->data[i] = handle->data[i+handle->offset];
		}
		handle->offset = 0;
		handle->dataSize = remainSize;
	}
}
