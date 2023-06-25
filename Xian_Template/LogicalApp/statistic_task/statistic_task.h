#ifndef _STATISTIC_TASK_H_
#define _STATISTIC_TASK_H_

#include "sys.h"

#define  APP_CFG_TASK_IDLE_PRIO          31u
#define  APP_CFG_TASK_IDLE_STK_SIZE      1024u

#define  APP_CFG_TASK_STAT_PRIO          30u
#define  APP_CFG_TASK_STAT_STK_SIZE      1024u


extern TX_THREAD   AppTaskStatTCB;
extern TX_THREAD   AppTaskIdleTCB;
extern uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];
extern uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];

void  DispTaskInfo          (void);
void  AppTaskIDLE			(ULONG thread_input);
void  AppTaskStat			(ULONG thread_input);
void  OSStatInit 			(void);
#endif

