/****************************************************************************************
 *   FileName    : tc-package.c
 *   Description : tc-package.c
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
#include <stdint.h>

#include "tc-ipc-ctrl-log.h"
#include "tc-ipc-ctrl.h"
#include "tc-ipc-protocol.h"
#include "tc-buffer.h"
#include "tc-crc.h"
#include "tc-parser.h"
#include "tc-rw-control.h"
#include "tc-package.h"

#if 0
static void tc_input_data(uint8_t* bufs, uint32_t data, uint16_t length)
{
	if(length != 0)
	{
		for(length; length > 0; length--)
		{
			*bufs = data >> ((length-1)*8);
			bufs++;
		}
	}
	else
	{
		*bufs = 0x00;
	}
}
#endif

int32_t tc_package_send(struct ipc_ctrl_handle *ctrl_handle,uint16_t cmd1, uint16_t cmd2, uint8_t *buf, uint16_t length)
{
	uint8_t sendBuf[IPC_MAXIMUM_PACKET_SIZE];
	uint32_t totalsize;
	uint16_t crc;
	int32_t ret;
	int32_t i;
	DEBUG_LIBIPC_PRINTF("cmd1(0x%x), cmd2(0x%x), bufsize(%d)\n", cmd1, cmd2, length);

	sendBuf[0] = 0xff;
	sendBuf[1] = 0x55;
	sendBuf[2] = 0xaa;

	for(i=0;i< (int32_t)length;i++)
	{
		DEBUG_LIBIPC_PRINTF("%d : buf [0x%02x]\n", i,buf[i] );
	}

	sendBuf[IPC_PROTOCOL_COMMAND1_HIGH_OFFSET] =  (uint8_t)((cmd1 & (uint16_t)0xff00) >> 8);
	sendBuf[IPC_PROTOCOL_COMMAND1_LOW_OFFSET] = (uint8_t)(cmd1 & (uint16_t)0x00ff);
	sendBuf[IPC_PROTOCOL_COMMAND2_HIGH_OFFSET] = (uint8_t)((cmd2 & (uint16_t)0xff00) >> 8);
	sendBuf[IPC_PROTOCOL_COMMAND2_LOW_OFFSET] = (uint8_t)(cmd2 & (uint16_t)0x00ff);
	sendBuf[IPC_PROTOCOL_LENGTH_HIGH_OFFSET] = (uint8_t)((length & (uint16_t)0xff00) >> 8);
	sendBuf[IPC_PROTOCOL_LENGTH_LOW_OFFSET] = (uint8_t)(length & (uint16_t)0x00ff);

	if((length > (uint16_t)0)&&(buf != NULL))
	{
		(void)memcpy(&sendBuf[IPC_PROTOCOL_DATA_OFFSET], buf, length);
		totalsize = (uint32_t)IPC_PROTOCOL_DATA_OFFSET + (uint32_t)length;
	}
	else
	{
		sendBuf[IPC_PROTOCOL_DATA_OFFSET]=0x00;
		totalsize = (uint32_t)IPC_PROTOCOL_DATA_OFFSET + (uint32_t)1;
	}

	crc = tc_CRC16((const char *)sendBuf, totalsize, 0);

	sendBuf[totalsize] = (uint8_t)((crc & (uint16_t)0xff00) >> 8);
	sendBuf[totalsize + (uint32_t)1] = (uint8_t)(crc & (uint16_t)0xff);

	totalsize += (uint32_t)2;

	ret = tc_ipc_write_data(ctrl_handle,sendBuf, totalsize);
	if(ret != (int32_t)totalsize)
	{
		ERROR_LIBIPC_PRINTF("Send data error (Can't push data to write buffer) \n");
	}

	return ret;
}

