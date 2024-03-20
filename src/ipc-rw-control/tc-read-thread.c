/****************************************************************************************
 *   FileName    : tc-read-thread.c
 *   Description : tc-read-thread.c
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
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "tc-ipc-protocol.h"
#include "tc-ipc-ctrl-log.h"
#include "tc-ipc-ctrl.h"
#include "tc-parser.h"
#include "tc-buffer.h"
#include "tc-rw-control.h"
#include "tc-read-thread.h"

#define TMM_READ_SIZE 		520


static void *ReadThread(void *arg)
{
	int32_t readSize;
	uint8_t readBufs[TMM_READ_SIZE];

	if(arg != NULL)
	{
		struct ipc_ctrl_handle *ctrl_handle = (struct ipc_ctrl_handle *)arg;
		TC_RINGBUF *pBufCtrl = &ctrl_handle->readRingCtrl;

		while (ctrl_handle->readThreadRun == 1)
		{
			(void)usleep(5000);

			/* read */
			readSize = read(ctrl_handle->fd, (char *)readBufs, TMM_READ_SIZE);
			/* push to buffer */
			if(readSize > 0)
			{
				DEBUG_LIBIPC_PRINTF("Read from IPC : read size (%d) \n", readSize);
				{
					int i;
					for(i=0; i< readSize; i++)
					{
						DEBUG_LIBIPC_PRINTF("0x%02x\n", readBufs[i]);
					}
				}
				if(readSize > tc_buffer_free_space(pBufCtrl))
				{
					(void)pthread_mutex_lock(&pBufCtrl->mutex);
					(void)tc_push_buffer_overwrite(pBufCtrl, readBufs, (uint32_t)readSize);
					(void)pthread_mutex_unlock(&pBufCtrl->mutex);
				}
				else
				{
					(void)tc_push_buffer(pBufCtrl, readBufs, (uint32_t)readSize);
				}
			}
		}
	}
	else
	{
		ERROR_LIBIPC_PRINTF("Read buffer is NULL ");
	}
	pthread_exit((void *)0);
}

int32_t tc_start_readthread(struct ipc_ctrl_handle *ctrl_handle)
{
	int32_t ret = -1;

	if(ctrl_handle != NULL)
	{
		ctrl_handle->readThreadRun = 1;
		ret= pthread_create(&ctrl_handle->readThread, NULL, ReadThread, ctrl_handle);
		if (ret != 0)
		{
			ctrl_handle->readThreadRun = 0;
			ERROR_LIBIPC_PRINTF("create PlayTime thread failed: ");
		}
	}
	else
	{
		ERROR_LIBIPC_PRINTF("Can't start read thread : argument error");
	}
	return ret;
}

int32_t tc_stop_readthread(struct ipc_ctrl_handle *ctrl_handle)
{
	int32_t err = -1;

	if (ctrl_handle->readThreadRun == 1)
	{
		void *res;

		ctrl_handle->readThreadRun = 0;
		err = pthread_join(ctrl_handle->readThread, &res);
		if (err != 0)
		{
			ERROR_LIBIPC_PRINTF("read thread join faild: ");
		}
	}
	return err;
}

