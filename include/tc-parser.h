/****************************************************************************************
 *   FileName    : tc-parser.h
 *   Description : tc-parser.h
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
#ifndef TC_PARSER_H
#define TC_PARSER_H

#define TMM_PARSER_BUF_SIZE 	(IPC_MAXIMUM_DATA_SIZE*2)

typedef struct stParserHandle{
	uint8_t data[TMM_PARSER_BUF_SIZE];
	uint32_t offset;
	uint32_t dataSize;
	uint32_t maxSize;
	IPC_msg parserData;
	int32_t parserState;

	int32_t 	parserThreadRun;
	pthread_t 	parserThread;
	IPC_ReceivedData_cb      parser_ReceivedData;
}parserHandle;

void ipc_parser_resister_callback(parserHandle *handle, IPC_ReceivedData_cb cb);
parserHandle *ipc_parser_handle_init(void);
void ipc_parser_handle_deinit(parserHandle *handle);
uint32_t tc_parser_freeSpace(parserHandle *handle);
uint8_t * tc_parser_BufAddr(parserHandle *handle);
void tc_parser_push_size(parserHandle *handle, uint32_t size);
void tc_parser_decoder(parserHandle *handle);

#endif
