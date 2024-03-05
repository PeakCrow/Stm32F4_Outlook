#ifndef _MSG_TASK_H_
#define _MSG_TASK_H_

#include "sys.h"


#define    APP_CFG_TASK_USER_IF_PRIO       4u
#define    APP_CFG_TASK_USER_IF_STK_SIZE   1024u

#define    APP_CFG_TASK_MsgPro_PRIO        3u
#define    APP_CFG_TASK_MsgPro_STK_SIZE    1024u


extern TX_THREAD   AppTaskUserIFTCB; 
extern uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];
extern TX_THREAD   AppTaskMsgProTCB;
extern uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];
extern TX_MUTEX    AppPrintfSemp                /* 用于printf互斥 */
extern TX_EVENT_FLAGS_GROUP  EventGroup;        /* 事件标志组 */
extern TX_EVENT_FLAGS_GROUP  EventGroup;        /* 事件标志组 */


typedef struct _bsp_all_msg{
    float * driver_pos_data[3];
    float temp_data;
    float rottation_data;
    float * adc_data[3];
    CAN_RxHeaderTypeDef * can_rx_msg;
    uint8_t * g_canrxbuf;
    uint8_t key_value;
} bsp_all_msg;
void  AppTaskMsgPro             (ULONG thread_input); 
void  AppTaskUserIF             (ULONG thread_input);
void  App_Printf                (const char *fmt, ...);
void  App_I2C_EE_ByteWrite  (uint8_t * pBuffer, uint8_t WriteAddr);

#endif

