/****************************************************************************************
 *   FileName    : tc-rw-control.h
 *   Description : tc-rw-control.h
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

#ifndef TC_RW_CONTROL_H
#define TC_RW_CONTROL_H

#define READ_RING_BUFFER_SIZE		1024 * 16
#define WRITE_RING_BUFFER_SIZE	1024 * 16

struct ipc_ctrl_handle
{
	IPC_ReceivedData_cb receive_cb;
	int32_t fd;

	TC_RINGBUF readRingCtrl;
	TC_RINGBUF writeRingCtrl;
	uint8_t readRingBuffer[READ_RING_BUFFER_SIZE];
	uint8_t writeRingBuffer[WRITE_RING_BUFFER_SIZE];
	pthread_mutex_t readMutex;
	pthread_mutex_t writeMutex;

	int32_t		readThreadRun;
	pthread_t	readThread;

	int32_t 	writeThreadRun;
	pthread_t 	writeThread;

	parserHandle *pParserHandle;
};

int32_t tc_ipc_control_init(const char* device, struct ipc_ctrl_handle *ctrl_handle);
void tc_ipc_control_release(struct ipc_ctrl_handle *ctrl_handle);
int32_t tc_ipc_write_data(struct ipc_ctrl_handle *ctrl_handle,uint8_t * buffer, uint32_t size);
int32_t tc_ipc_read_data(struct ipc_ctrl_handle *ctrl_handle,uint8_t * buffer, uint32_t size);
int32_t tc_ipc_read_available_size(struct ipc_ctrl_handle *ctrl_handle);


#endif /* TC_RW_CONTROL_H */
