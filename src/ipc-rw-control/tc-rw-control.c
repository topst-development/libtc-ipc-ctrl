 /****************************************************************************************
 *   FileName    : tc-rw-control.c
 *   Description : tc-rw-control.c
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
***************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "tc-ipc-protocol.h"
#include "tc-ipc-dev.h"
#include "tc-ipc-ctrl-log.h"
#include "tc-buffer.h"
#include "tc-ipc-ctrl.h"
#include "tc-parser.h"
#include "tc-rw-control.h"
#include "tc-read-thread.h"
#include "tc-write-thread.h"

#define COM_VMIN	1
#define COM_VTIME	1	/* block timeout 0.01sec */

int32_t tc_ipc_control_init(const char* device, struct ipc_ctrl_handle *ctrl_handle)
{
	int32_t ret;

	INFO_LIBIPC_PRINTF("In\n");

	pthread_mutex_init(&ctrl_handle->readMutex, NULL);
	pthread_mutex_init(&ctrl_handle->writeMutex, NULL);

	/* 1.buffer init */
	tc_buffer_init(&ctrl_handle->readRingCtrl,ctrl_handle->readRingBuffer,READ_RING_BUFFER_SIZE, ctrl_handle->readMutex);
	tc_buffer_init(&ctrl_handle->writeRingCtrl,ctrl_handle->writeRingBuffer,WRITE_RING_BUFFER_SIZE, ctrl_handle->writeMutex);


	/* 3. Open comport : IPC*/
	ctrl_handle->fd =  ipc_open(device, O_RDWR);
	if(ctrl_handle->fd > -1)
	{
		ret = ipc_setparam(ctrl_handle->fd, COM_VMIN, COM_VTIME);
		if(ret > -1)
		{
			if(ipc_status(ctrl_handle->fd) != (uint32_t)1)
			{
				INFO_LIBIPC_PRINTF("My IPC open. But other ipc is not ready. skip\n");
			}
		}
	}
	else
	{
		ret = -1;
	}

	/* start thread */
	/* 4 . read-thread */
	if(ret == 0 )
	{
		INFO_LIBIPC_PRINTF("start read thread\n");
		ret = tc_start_readthread(ctrl_handle);
	}

	/* 5.  write-thread */
	if(ret ==0 )
	{
		INFO_LIBIPC_PRINTF("start write thread\n");
		ret = tc_start_writethread(ctrl_handle);
	}
	return ret;
}


void tc_ipc_control_release(struct ipc_ctrl_handle *ctrl_handle)
{
	INFO_LIBIPC_PRINTF("\n");
	(void)tc_stop_readthread(ctrl_handle);
	tc_stop_writethread(ctrl_handle);
	ipc_close(ctrl_handle->fd);
}

int32_t tc_ipc_write_data(struct ipc_ctrl_handle *ctrl_handle,uint8_t * buffer, uint32_t size)
{
	int32_t ret =-1;
	if(buffer !=NULL)
	{
		(void)pthread_mutex_lock(&ctrl_handle->writeRingCtrl.mutex);
		if(size > (uint32_t)tc_buffer_free_space(&ctrl_handle->writeRingCtrl))
		{
			ret = tc_push_buffer_overwrite(&ctrl_handle->writeRingCtrl, buffer, size);
		}
		else
		{
			ret = tc_push_buffer(&ctrl_handle->writeRingCtrl, buffer, size);
		}
		(void)pthread_mutex_unlock(&ctrl_handle->writeRingCtrl.mutex);
	}
	return ret;
}

int32_t tc_ipc_read_data(struct ipc_ctrl_handle *ctrl_handle,uint8_t * buffer, uint32_t size)
{
	int32_t ret=-1;

	if(buffer != NULL)
	{
		(void)pthread_mutex_lock(&ctrl_handle->readRingCtrl.mutex);
		ret = tc_pop_buffer(&ctrl_handle->readRingCtrl, buffer, size);
		{
			int i;
			for(i=0; i < size; i++)
			{
				DEBUG_LIBIPC_PRINTF("0x%02x\n", buffer[i]);
			}
		}
		(void)pthread_mutex_unlock(&ctrl_handle->readRingCtrl.mutex);
	}
	return ret;
}

int32_t tc_ipc_read_available_size(struct ipc_ctrl_handle *ctrl_handle)
{
	return tc_buffer_data_available(&ctrl_handle->readRingCtrl);
}


