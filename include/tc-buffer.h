/****************************************************************************************
 *   FileName    : tc-buffer.h
 *   Description : tc-buffer.h
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

#ifndef TC_BUFFER_H
#define TC_BUFFER_H

#include <stdint.h>
#include <pthread.h>

#define	TC_BUFFER_ERROR		(-1)
#define	TC_BUFFER_FULL		(-2)
#define	TC_BUFFER_EMPTY		(-3)
#define	TC_BUFFER_OK		0

typedef struct stTC_RINGBUF
{
	uint32_t 		Head;		//read position
	uint32_t		Tail;		//write position
	uint32_t		MaxBufferSize;
	uint8_t* 	pBuffer;
	pthread_mutex_t	mutex;
}TC_RINGBUF;

void	tc_buffer_init(TC_RINGBUF *pBufCtrl,uint8_t* buff,uint32_t size, pthread_mutex_t mutex);
int32_t tc_push_one_byte(TC_RINGBUF *pBufCtrl,uint8_t data);
int32_t tc_push_one_byte_overwrite(TC_RINGBUF *pBufCtrl,uint8_t data);
int32_t tc_pop_one_byte(TC_RINGBUF *pBufCtrl,uint8_t *data);
void	tc_buffer_flush(TC_RINGBUF *pBufCtrl);
void	tc_buffer_flush_byte(TC_RINGBUF *pBufCtrl,uint32_t flushSize);
int32_t	tc_buffer_data_available(const TC_RINGBUF *pBufCtrl);
int32_t	tc_buffer_free_space(const TC_RINGBUF *pBufCtrl);
uint32_t tc_buffer_get_head(const TC_RINGBUF *pBufCtrl);
uint32_t tc_buffer_get_tail(const TC_RINGBUF *pBufCtrl);
int32_t tc_push_buffer(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size);
int32_t tc_push_buffer_overwrite(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size);
int32_t tc_pop_buffer(TC_RINGBUF *pBufCtrl,uint8_t * buffer, uint32_t size);

#endif
