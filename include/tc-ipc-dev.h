/****************************************************************************************
 *   FileName    : tc-ipc-dev.h
 *   Description : tc-ipc-dev.h
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
#ifndef TC_IPC_DEV_H
#define TC_IPC_DEV_H

#ifndef NULL
#define NULL	(0)
#endif

int32_t ipc_open(const char * filePath, int32_t flag);
int32_t ipc_setparam(int32_t fd, uint32_t vMin, uint32_t vTime);
int32_t ipc_write(int32_t fd, char * wbuffer, size_t wSize);
int32_t ipc_read(int32_t fd, char * rbuffer, size_t rSize);
int32_t ipc_flush(int32_t fd);
uint32_t ipc_status(int32_t fd);
int32_t ipc_close(int32_t fd);

#endif
