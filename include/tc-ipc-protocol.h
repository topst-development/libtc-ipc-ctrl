/****************************************************************************************
 *   FileName    : tc-ipc-protocol.h
 *   Description : tc-ipc-protocol.h
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

#ifndef TC_IPC_PROTOCOL_H
#define TC_IPC_PROTOCOL_H

#define EXTKEY_POWER_BIT 1<<15		// SVN MODE KEY for 803x

#define IPC_MINIMUM_PACKET_SIZE			12
#define IPC_MAXIMUM_PACKET_SIZE			520
#define IPC_MAXIMUM_DATA_SIZE			512
#define IPC_SYNC_CODE			       		0xff
#define IPC_FIRST_START_CODE				0x55
#define IPC_SECOND_START_CODE			0xaa

typedef enum
{
	IPC_PROTOCOL_SYNC_OFFSET = 0x00,
	IPC_PROTOCOL_FIRST_START_OFFSET,
	IPC_PROTOCOL_SECOND_START_OFFSET,
	IPC_PROTOCOL_COMMAND1_HIGH_OFFSET,
	IPC_PROTOCOL_COMMAND1_LOW_OFFSET,
	IPC_PROTOCOL_COMMAND2_HIGH_OFFSET,
	IPC_PROTOCOL_COMMAND2_LOW_OFFSET,
	IPC_PROTOCOL_LENGTH_HIGH_OFFSET,
	IPC_PROTOCOL_LENGTH_LOW_OFFSET,
	IPC_PROTOCOL_DATA_OFFSET,
	IPC_PROTOCOL_MAX
}IPC_PROTOCOL_OFFSET;

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#endif
