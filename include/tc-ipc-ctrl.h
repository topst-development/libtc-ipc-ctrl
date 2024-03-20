/****************************************************************************************
 *   FileName    : tc-ipc-ctrl.h
 *   Description : tc-ipc-ctrl.h
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
#ifndef TC_IPC_SESSION_H
#define TC_IPC_SESSION_H

extern int32_t g_ipc_ctrl_debug;

typedef struct stIPC_msg
{
	uint16_t cmd1;
	uint16_t cmd2;
	uint16_t length;
	uint8_t* data;
}IPC_msg;

typedef void (*IPC_ReceivedData_cb)(IPC_msg* data);

void * tc_ipc_ctrl_open(const char *device, IPC_ReceivedData_cb eventCB);
int32_t tc_ipc_ctrl_msg_send(void * handle,uint16_t cmd1, uint16_t cmd2, uint8_t *buf, uint16_t length);
void tc_ipc_ctrl_close(void * handle);
void tc_ipc_ctrl_debug(int32_t on);

#endif	/* TC_IPC_SESSION_H */
