/****************************************************************************************
 *   FileName    : tc-ipc-dev.c
 *   Description : tc-ipc-dev.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
 #include <unistd.h>
#include <stdint.h>

#include "linux/tcc_ipc.h"
#include "tc-ipc-ctrl-log.h"
#include "tc-ipc-dev.h"

static int32_t ipc_ping_test(int32_t fd, tcc_ipc_ping_info *pingInfo);

int32_t ipc_open(const char * filePath, int32_t flag)
{
	int32_t fd = -1;

	if(filePath != NULL)
	{
		fd = open(filePath, flag);
		if(fd <0)
		{
			ERROR_LIBIPC_PRINTF("open fail : %s\n", filePath);
		}
	}
	return fd;
}

int32_t ipc_setparam(int32_t fd, uint32_t vMin, uint32_t vTime)
{
	int32_t ret =-1;

	if(fd > -1)
	{
		tcc_ipc_ctl_param ipc_param;
		ipc_param.vMin = vMin;
		ipc_param.vTime = vTime;
		ret = ioctl(fd, IOCTL_IPC_SET_PARAM , &ipc_param);
	}
	return ret;
}

int32_t ipc_write(int32_t fd, char * wbuffer, size_t wSize)
{
	int32_t ret =-1;
	if((fd > -1)&&(wbuffer != NULL)&&(wSize > (size_t)0))
	{
		ret = write(fd, wbuffer, wSize);
	}
	return ret;
}

int32_t ipc_read(int32_t fd, char * rbuffer, size_t rSize)
{
	int32_t ret =-1;
	if((fd > -1)&&(rbuffer != NULL)&&(rSize > (size_t)0))
	{
		ret = read(fd, (void *)rbuffer, rSize);
	}
	return ret;
}

int32_t ipc_flush(int32_t fd)
{
	int32_t ret =-1;
	if(fd > -1)
	{
		ret = ioctl(fd, IOCTL_IPC_FLUSH, NULL);
	}
	return ret;
}

static int32_t ipc_ping_test(int32_t fd, tcc_ipc_ping_info *pingInfo)
{
	int32_t ret =-1;
	if(fd > -1)
	{
		ret = ioctl(fd, IOCTL_IPC_PING_TEST, pingInfo);
	}
	return ret;
}

uint32_t ipc_status(int32_t fd)
{
	uint32_t status=0;

	if(fd > -1)
	{
		(void)ioctl(fd, IOCTL_IPC_ISREADY, &status);
	}
	return status;
}

int32_t ipc_close(int32_t fd)
{
	int32_t ret;
	ret = close(fd);
	return ret;
}

