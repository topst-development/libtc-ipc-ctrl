/****************************************************************************************
 *   FileName    : tc-buffer.c
 *   Description : tc-buffer.c
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
#include <stdint.h>
#include "tc-ipc-ctrl-log.h"
#include "tc-buffer.h"

void tc_buffer_init(TC_RINGBUF *pBufCtrl,uint8_t* buff,uint32_t size, pthread_mutex_t mutex)
{
	pBufCtrl->Head = 0;
	pBufCtrl->Tail = 0;
	pBufCtrl->MaxBufferSize = size;
	pBufCtrl->pBuffer = buff;
	pBufCtrl->mutex = mutex;
}

int32_t tc_push_one_byte(TC_RINGBUF *pBufCtrl,uint8_t data)
{
	int32_t ret = TC_BUFFER_OK;
	uint32_t temp;

	temp = pBufCtrl->Tail;
	temp++;
	temp %= pBufCtrl->MaxBufferSize;

	if (temp == pBufCtrl->Head)
	{
		ret = TC_BUFFER_FULL;
	}
	else
	{
		pBufCtrl->pBuffer[pBufCtrl->Tail] = data;
		pBufCtrl->Tail = (uint32_t)temp;
	}

	return ret;
}

int32_t tc_push_one_byte_overwrite(TC_RINGBUF *pBufCtrl,uint8_t data)
{
	int32_t ret;
	uint32_t temp;

	temp = pBufCtrl->Tail;
	temp++;
	temp %= pBufCtrl->MaxBufferSize;

	if (temp == pBufCtrl->Head)
	{
		pBufCtrl->Head++;
		pBufCtrl->pBuffer[pBufCtrl->Tail] = data;
		pBufCtrl->Tail = (uint32_t)temp;
		ret = TC_BUFFER_OK;
	}
	else
	{
		pBufCtrl->pBuffer[pBufCtrl->Tail] = data;
		pBufCtrl->Tail = (uint32_t)temp;
		ret = TC_BUFFER_OK;
	}

	return ret;
}

int32_t tc_pop_one_byte(TC_RINGBUF *pBufCtrl,uint8_t *data)
{
	uint32_t  temp;
	int32_t ret;

	if (pBufCtrl->Tail == pBufCtrl->Head)
	{
		ret = TC_BUFFER_EMPTY;
	}
	else
	{
		temp = pBufCtrl->Head;
		temp++;
		temp %= pBufCtrl->MaxBufferSize;
		*data = pBufCtrl->pBuffer[pBufCtrl->Head];
		pBufCtrl->Head = temp;
		ret = TC_BUFFER_OK;
	}
	return ret;
}

void	tc_buffer_flush(TC_RINGBUF *pBufCtrl)
{
	pBufCtrl->Head = 0;
	pBufCtrl->Tail = 0;
}

void	tc_buffer_flush_byte(TC_RINGBUF *pBufCtrl,uint32_t flushSize)
{
	uint32_t  temp;

	if (pBufCtrl->Tail < pBufCtrl->Head)
	{
		temp = pBufCtrl->Head + flushSize;
		if(temp< pBufCtrl->MaxBufferSize)
		{
			pBufCtrl->Head = temp;
		}
		else
		{
			temp %= pBufCtrl->MaxBufferSize;
			if(pBufCtrl->Tail <= temp)
			{
				pBufCtrl->Head = pBufCtrl->Tail;
			}
			else
			{
				pBufCtrl->Head = temp;
			}
		}
	}
	else
	{
		temp = pBufCtrl->Head + flushSize;
		if(pBufCtrl->Tail <= temp)
		{
			pBufCtrl->Head = pBufCtrl->Tail;
		}
		else
		{
			pBufCtrl->Head = temp;
		}
	}
}


int32_t	tc_buffer_data_available(const TC_RINGBUF *pBufCtrl)
{
	int32_t iRet, iRead, iWrite;
	iRead = (int32_t)pBufCtrl->Head;
	iWrite = (int32_t)pBufCtrl->Tail;

	if (iWrite >= iRead)
	{
		// The read pointer is before the write pointer in the buffer.
		iRet = iWrite -	iRead;
	}
	else
	{
		// The write pointer is before the read pointer in the buffer.
		iRet = (int32_t)pBufCtrl->MaxBufferSize - (iRead - iWrite);
	}

	return iRet;
}

int32_t	tc_buffer_free_space(const TC_RINGBUF *pBufCtrl)
{
	int32_t iRet, iRead, iWrite;
	iRead = (int32_t)pBufCtrl->Head;
	iWrite = (int32_t)pBufCtrl->Tail;

	if (iWrite < iRead)
	{
		// The write pointer is before the read pointer in the buffer.
		iRet = iRead - iWrite - 1;
	}
	else
	{
		// The read pointer is before the write pointer in the bufer.
		iRet = (int32_t)pBufCtrl->MaxBufferSize - (iWrite - iRead) - 1;
	}

	return iRet;
}

uint32_t tc_buffer_get_head(const TC_RINGBUF *pBufCtrl)
{
	return pBufCtrl->Head;
}

uint32_t tc_buffer_get_tail(const TC_RINGBUF *pBufCtrl)
{
	return pBufCtrl->Tail;
}

int32_t tc_push_buffer(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size)
{
	int32_t ret;
	uint32_t freeSpace;

	freeSpace = (uint32_t)tc_buffer_free_space(pBufCtrl);
	if(freeSpace >= size)
	{
		uint32_t continuousSize;

		continuousSize = pBufCtrl->MaxBufferSize - pBufCtrl->Tail;
		if(continuousSize > size)
		{
			(void)memcpy(&pBufCtrl->pBuffer[pBufCtrl->Tail], buffer, (size_t)size);
			pBufCtrl->Tail +=size;
			ret = (int32_t)size;
		}
		else
		{
			uint32_t remainSize;

			(void)memcpy(&pBufCtrl->pBuffer[pBufCtrl->Tail], buffer, (size_t)continuousSize);
			remainSize = size - continuousSize;
			(void)memcpy(&pBufCtrl->pBuffer[0], &buffer[continuousSize], (size_t)remainSize);
			pBufCtrl->Tail = remainSize;
			ret = (int32_t)size;
		}
	}
	else
	{
		ret =TC_BUFFER_FULL;
	}

	return ret;
}

int32_t tc_push_buffer_overwrite(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size)
{
	int32_t ret;
	uint32_t continuousSize;
	continuousSize = pBufCtrl->MaxBufferSize - pBufCtrl->Tail;
	if(continuousSize > size)
	{
		(void)memcpy(&pBufCtrl->pBuffer[pBufCtrl->Tail], buffer, (size_t)size);
		pBufCtrl->Tail +=size;
		pBufCtrl->Head = ((pBufCtrl->Tail + 1) % pBufCtrl->MaxBufferSize);
		ret = (int32_t)size;
	}
	else
	{
		uint32_t remainSize;

		(void)memcpy(&pBufCtrl->pBuffer[pBufCtrl->Tail], buffer, (size_t)continuousSize);
		remainSize = size - continuousSize;
		(void)memcpy(&pBufCtrl->pBuffer[0], &buffer[continuousSize], (size_t)remainSize);
		pBufCtrl->Tail = remainSize;
		pBufCtrl->Head = ((pBufCtrl->Tail + 1) % pBufCtrl->MaxBufferSize);
		ret = (int32_t)size;
	}

	return ret;
}

int32_t tc_pop_buffer(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size)
{
	int32_t ret = TC_BUFFER_ERROR;;

	if (pBufCtrl->Tail == pBufCtrl->Head)
	{
		ret = TC_BUFFER_EMPTY;
	}
	else
	{
		int32_t dataSize;
		dataSize = tc_buffer_data_available(pBufCtrl);
		if(dataSize >= (int32_t)size)
		{
			uint32_t continuousSize;

			continuousSize = pBufCtrl->MaxBufferSize - pBufCtrl->Head;
			if(continuousSize > size)
			{
				(void)memcpy(buffer, &pBufCtrl->pBuffer[pBufCtrl->Head],(size_t)size );
				pBufCtrl->Head += size;
				ret = TC_BUFFER_OK;
			}
			else
			{
				uint32_t remainSize;
				(void)memcpy(buffer, &pBufCtrl->pBuffer[pBufCtrl->Head],(size_t)continuousSize );
				remainSize = size - continuousSize;
				(void)memcpy(&buffer[continuousSize], &pBufCtrl->pBuffer[0],(size_t)remainSize );
				pBufCtrl->Head= remainSize;
				ret = TC_BUFFER_OK;
			}
		}
		else
		{
			/* error */
			;
		}
	}
	return ret;
}

#if 0
void tc_buffer_print(TC_RINGBUF *pBufCtrl)
{
	(void)fprintf(stderr, "%s\n", __FUNCTION__);

	if(pBufCtrl != NULL)
	{
		(void)fprintf(stderr, "%s : head (%d), tail(%d), max(%d) : ", __FUNCTION__, pBufCtrl->Head, pBufCtrl->Tail, pBufCtrl->MaxBufferSize);

		int32_t i;
		int32_t iRead, iWrite;
		iRead = (int32_t)pBufCtrl->Head;
		iWrite = (int32_t)pBufCtrl->Tail;

		if (iWrite >= iRead)
		{
			for(i=iRead; i<iWrite;i++)
			{
				(void)fprintf(stderr, "0x%02x " ,pBufCtrl->pBuffer[i]);
			}
		}
		else
		{
			for(i=iRead; i<(int32_t)pBufCtrl->MaxBufferSize; i++)
			{
				(void)fprintf(stderr, "0x%02x " ,pBufCtrl->pBuffer[i]);
			}
			for(i=0; i<iWrite;i++)
			{
				(void)fprintf(stderr, "0x%02x " ,pBufCtrl->pBuffer[i]);
			}
		}
		(void)fprintf(stderr, "\n" );
	}
}
#endif
