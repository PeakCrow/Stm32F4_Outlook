#ifndef _GUI_TASK_H_
#define _GUI_TASK_H_

#include "sys.h"                            

#define  APP_CFG_TASK_TFTLCD_STK_SIZE                   2*1024u
#define  APP_CFG_TASK_TFTLCD_PRIO						30u
extern   TX_THREAD	AppTaskTFTLCDTCB;
extern   uint64_t	AppTaskTFTLCDStk[APP_CFG_TASK_TFTLCD_STK_SIZE/8];
extern   TX_MUTEX AppLCDSemp;						/* ”√”⁄LVGLª•≥‚ */
void  AppTaskTFTLCD			(ULONG thread_input);

#endif


