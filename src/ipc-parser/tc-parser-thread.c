/****************************************************************************************
 *   FileName    : tc-parser-thread.c
 *   Description : tc-parser-thread.c
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

#include "tc-ipc-ctrl-log.h"
#include "tc-buffer.h"
#include "tc-ipc-ctrl.h"
#include "tc-ipc-protocol.h"
#include "tc-parser.h"
#include "tc-rw-control.h"
#include "tc-parser-thread.h"

static void *ParserThread(void *arg)
{
	int32_t ret;


	if(arg != NULL)
	{
		struct ipc_ctrl_handle *ctrl_handle = (struct ipc_ctrl_handle *)arg;
		parserHandle * pParserHandle = ctrl_handle->pParserHandle;

		while((pParserHandle->parserThreadRun == 1)&&(pParserHandle != NULL))
		{
			int32_t availableSize;
			availableSize = tc_ipc_read_available_size(ctrl_handle);
			if(availableSize > 0)
			{
				uint32_t parserSize, readSize;
				parserSize =  tc_parser_freeSpace(pParserHandle);
				if((uint32_t)availableSize > parserSize)
				{
					readSize = parserSize;
				}
				else
				{
					readSize = (uint32_t)availableSize;
				}

				ret = tc_ipc_read_data( ctrl_handle,tc_parser_BufAddr(pParserHandle), readSize);
				if(ret == 0)
				{
					tc_parser_push_size(pParserHandle,readSize);
					tc_parser_decoder(pParserHandle);
				}
			}
			(void)usleep(5000);
		}
	}

	pthread_exit((void *)0);

}

int32_t tc_start_parserthread(struct ipc_ctrl_handle *ctrl_handle)
{
	INFO_LIBIPC_PRINTF("%s\n", __FUNCTION__);
	int32_t ret = -1;

	if((ctrl_handle != NULL)&&(ctrl_handle->pParserHandle != NULL))
	{
		parserHandle * pParserHandle = ctrl_handle->pParserHandle;

		ipc_parser_resister_callback(pParserHandle,ctrl_handle->receive_cb);

		pParserHandle->parserThreadRun = 1;
		ret = pthread_create(&pParserHandle->parserThread, NULL, ParserThread, ctrl_handle);
		if(ret != 0)
		{
			pParserHandle->parserThreadRun = 0;
			//error msg
		}
	}
	return ret;
}

void tc_stop_parserthread(struct ipc_ctrl_handle *ctrl_handle)
{
	int32_t ret;

	if((ctrl_handle != NULL)&&(ctrl_handle->pParserHandle != NULL))
	{
		parserHandle * pParserHandle = ctrl_handle->pParserHandle;

		if(pParserHandle->parserThreadRun == 1)
		{
			void *res;
			pParserHandle->parserThreadRun = 0;
			ret = pthread_join(pParserHandle->parserThread, &res);
			if(ret != 0)
			{
				//error msg
				ERROR_LIBIPC_PRINTF("Parser thread join fail\n");
			}
		}
	}
}

int tc_parser_init(struct ipc_ctrl_handle *ctrl_handle)
{
	int32_t ret = -1;

	INFO_LIBIPC_PRINTF("\n");

	ctrl_handle->pParserHandle = ipc_parser_handle_init();
	if(ctrl_handle->pParserHandle == NULL)
	{
		ERROR_LIBIPC_PRINTF("IPC parser init fail.\n");
	}
	else
	{
		ret =  tc_start_parserthread(ctrl_handle);
		if(ret < 0 )
		{
			tc_parser_release(ctrl_handle);
		}
		else
		{
			ret = 0;
		}
	}
	return ret;
}

void tc_parser_release(struct ipc_ctrl_handle *ctrl_handle)
{
	if(ctrl_handle->pParserHandle != NULL)
	{
		tc_stop_parserthread(ctrl_handle);
		ipc_parser_handle_deinit(ctrl_handle->pParserHandle);
		ctrl_handle->pParserHandle = NULL;
	}
}

