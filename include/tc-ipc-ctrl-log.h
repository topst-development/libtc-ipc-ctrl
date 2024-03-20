/****************************************************************************************
 *
 *   TCC Version 1.0
 *   Copyright (c) Telechips Inc.
 *   All rights reserved 
 *
This source code contains confidential information of Telechips.
Any unauthorized use without a written permission of Telechips including not limited 
to re-distribution in source or binary form is strictly prohibited.
This source code is provided “AS IS” and nothing contained in this source code 
shall constitute any express or implied warranty of any kind, including without limitation, 
any warranty of merchantability, fitness for a particular purpose or non-infringement of any patent, 
copyright or other third party intellectual property right. 
No warranty is made, express or implied, regarding the information’s accuracy, 
completeness, or performance. 
In no event shall Telechips be liable for any claim, damages or other liability arising from, 
out of or in connection with this source code or the use in the source code. 
This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement 
between Telechips and Company.
*
****************************************************************************************/

#ifndef IPC_CTRL_LOG_H
#define IPC_CTRL_LOG_H

extern int32_t g_ipc_ctrl_debug;

#define LIBIPC_ERROR_LEVEL 	(0U)
#define LIBIPC_WARN_LEVEL 	(1U)
#define LIBIPC_INFO_LEVEL 	(2U)
#define LIBIPC_DEBUG_LEVEL 	(3U)

#define ERROR_LIBIPC_PRINTF(format, arg...) \
		fprintf(stderr, "[ERROR][LIB_IPC_CTRL][%s:%d] : "format"", __FUNCTION__, __LINE__, ##arg); \

#define WARN_LIBIPC_PRINTF(format, arg...) \
	if (g_ipc_ctrl_debug > LIBIPC_WARN_LEVEL) \
	{ \
		fprintf(stderr, "[WARN][LIB_IPC_CTRL][%s:%d] : "format"", __FUNCTION__, __LINE__, ##arg); \
	}

#define INFO_LIBIPC_PRINTF(format, arg...) \
	if (g_ipc_ctrl_debug > LIBIPC_INFO_LEVEL) \
	{ \
		fprintf(stderr, "[INFO][LIB_IPC_CTRL][%s:%d] : "format"", __FUNCTION__, __LINE__, ##arg); \
	}

#define DEBUG_LIBIPC_PRINTF(format, arg...) \
	if (g_ipc_ctrl_debug > LIBIPC_DEBUG_LEVEL) \
	{ \
		fprintf(stderr, "[DEBUG][LIB_IPC_CTRL][%s:%d] : "format"", __FUNCTION__, __LINE__, ##arg); \
	}


#endif
