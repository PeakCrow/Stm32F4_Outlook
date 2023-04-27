#ifndef	_SYSTEM_TASK_CREATE_H_
#define	_SYSTEM_TASK_CREATE_H_

#include "sys.h"

/***********************************ThreadX操作系统组件属性外部声名*************************************************************/
extern void  App_Printf 			(const char *fmt, ...);
extern TX_EVENT_FLAGS_GROUP  EventGroup; /* 事件标志组 */
extern void    App_I2C_EE_ByteWrite(uint8_t * pBuffer, uint8_t WriteAddr);
extern TX_EVENT_FLAGS_GROUP  EventGroup; 	/* 事件标志组 */
#define DRIVER1_POS (1<<0)
#define	DRIVER2_POS (1<<1)
#define DRIVER3_POS (1<<2)
#define DRIVER_POS_ALL (DRIVER1_POS | DRIVER2_POS | DRIVER3_POS)
#define HARDWARE_MAJOR_VERSION           1
#define HARDWARE_MINOR_VERSION           0
#define HARDWARE_PATCH_VERSION           0
/***********************************ThreadX操作系统组件属性外部声名*************************************************************/


#endif


