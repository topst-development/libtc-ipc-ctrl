/****************************************************************************************
 *   FileName    : tc-write-thread.c
 *   Description : tc-write-thread.c
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
#include "tc-ipc-dev.h"
#include "tc-parser.h"
#include "tc-buffer.h"
#include "tc-rw-control.h"
#include "tc-write-thread.h"

#define IPC_WRITE_SIZE			512

static void *WriteThread(void *arg)
{
	int32_t writeSize;
	int32_t ret;
	uint8_t writeBufs[IPC_WRITE_SIZE];

	if(arg != NULL)
	{
		struct ipc_ctrl_handle *ctrl_handle= (struct ipc_ctrl_handle*)arg;
		TC_RINGBUF *pBufCtrl = &ctrl_handle->writeRingCtrl;

		while(ctrl_handle->writeThreadRun == 1)
		{
			(void)usleep(5000);

			writeSize = tc_buffer_data_available(pBufCtrl);
			writeSize = MIN(writeSize, IPC_WRITE_SIZE);
			
			if(writeSize != 0)
			{
				if(ipc_status(ctrl_handle->fd) == (uint32_t)1)
				{
					(void)pthread_mutex_lock(&pBufCtrl->mutex);
					(void)tc_pop_buffer(pBufCtrl, writeBufs, (uint32_t)writeSize);
					(void)pthread_mutex_unlock(&pBufCtrl->mutex);
					ret = ipc_write(ctrl_handle->fd, (char *)writeBufs,(uint32_t)writeSize);
					if(ret != writeSize)
					{
						ERROR_LIBIPC_PRINTF("Can't write to ipc dev.\n");
					}
				}
				else
				{
					WARN_LIBIPC_PRINTF("ipc not ready\n");
				}
			}
		}
	}
	else
	{
		;
		/*
		debug msg : write buffer null
		*/
	}
	pthread_exit((void*)0);
}

int32_t tc_start_writethread(struct ipc_ctrl_handle *ctrl_handle)
{  
	int32_t ret = -1;
	if(ctrl_handle != NULL)
	{

		ctrl_handle->writeThreadRun = 1;
		ret = pthread_create(&ctrl_handle->writeThread, NULL, WriteThread, ctrl_handle);
		
		if(ret != 0)
		{
			ctrl_handle->writeThread = 0;
			ERROR_LIBIPC_PRINTF("create PlayTime thread failed: ");
		}
	}
	else
	{
		;
		/* 
		 debug msg : arg error
		 */
	}
	return ret;

}


void tc_stop_writethread(struct ipc_ctrl_handle *ctrl_handle)
{
	if(ctrl_handle->writeThreadRun == 1)
	{
		void *res;
		int32_t ret;
		ctrl_handle->writeThreadRun = 0;
		ret = pthread_join(ctrl_handle->writeThread, &res);
		if(ret != 0)
		{
			ERROR_LIBIPC_PRINTF("Write thread join failed: ");
		}
	}
}

