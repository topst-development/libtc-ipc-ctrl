/****************************************************************************************
 *   FileName    : tc-ipc-sesson.c
 *   Description : tc-ipc-sesson.c
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tc-ipc-ctrl-log.h"
#include "tc-buffer.h"
#include "tc-ipc-protocol.h"
#include "tc-ipc-ctrl.h"
#include "tc-parser.h"
#include "tc-ipc-dev.h"
#include "tc-rw-control.h"
#include "tc-package.h"
#include "tc-parser-thread.h"

int32_t g_ipc_ctrl_debug = LIBIPC_INFO_LEVEL;

void * tc_ipc_ctrl_open(const char *device, IPC_ReceivedData_cb eventCB)
{
	struct ipc_ctrl_handle *ctrl_handle = NULL;
	int32_t ret = -1;

	INFO_LIBIPC_PRINTF("device name : (%s)\n", device);
	if((device !=NULL)&&(eventCB != NULL))
	{
		ctrl_handle = malloc(sizeof(struct ipc_ctrl_handle));
		if(ctrl_handle != NULL)
		{
			memset(ctrl_handle, 0x00, sizeof(struct ipc_ctrl_handle));

			ret = tc_ipc_control_init(device, ctrl_handle);

			if(ret ==0)
			{
				ctrl_handle->receive_cb = eventCB;
				ret = tc_parser_init(ctrl_handle);
				if(ret != 0 )
				{
					ERROR_LIBIPC_PRINTF("tc_parser_init init.\n");
				}
			}
			else
			{
				ERROR_LIBIPC_PRINTF("ipc control init.\n");
			}

			if(ret != 0 )
			{
				free(ctrl_handle);
				ctrl_handle = NULL;
			}
		}
	}
	else
	{
		ERROR_LIBIPC_PRINTF("arguments eror.\n");
	}

	return (void *)ctrl_handle;
}

int32_t tc_ipc_ctrl_msg_send(void * handle, uint16_t cmd1, uint16_t cmd2, uint8_t *buf, uint16_t length)
{
	int32_t ret;
	if(handle != NULL)
	{
		struct ipc_ctrl_handle *ctrl_handle = (struct ipc_ctrl_handle *)handle;
		ret = tc_package_send(ctrl_handle, cmd1, cmd2, buf, length);
	}
	else
	{
		-1;
	}

	return ret;
}

void tc_ipc_ctrl_close(void * handle)
{
	struct ipc_ctrl_handle *ctrl_handle = (struct ipc_ctrl_handle *)handle;

	if(ctrl_handle != NULL)
	{
		tc_parser_release(ctrl_handle);
		tc_ipc_control_release(ctrl_handle);

		free(ctrl_handle);
	}
}

void tc_ipc_ctrl_debug(int32_t on)
{
	g_ipc_ctrl_debug = on;
}

