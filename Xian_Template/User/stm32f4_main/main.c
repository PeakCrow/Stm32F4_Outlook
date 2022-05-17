/*******************************************************************************
  * @file        main.c
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      TRX
  * @date        2022-4-25
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-4-25  TRX Created
*******************************************************************************/
#include "sys.h"


#if	DEBUG_SWITCH_EN == 1
#include "EventRecorder.h"
#endif


/*
*********************************************************************************************************
*                                 任务优先级，数值越小优先级越高
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          2u
#define  APP_CFG_TASK_MsgPro_PRIO                         3u
#define  APP_CFG_TASK_USER_IF_PRIO                        4u
#define  APP_CFG_TASK_COM_PRIO                            5u
#define  APP_CFG_TASK_STAT_PRIO                           30u
#define  APP_CFG_TASK_IDLE_PRIO                           31u


/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    4096u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   4096u
#define  APP_CFG_TASK_COM_STK_SIZE                      4096u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  4096u
#define  APP_CFG_TASK_IDLE_STK_SIZE                  	1024u
#define  APP_CFG_TASK_STAT_STK_SIZE                  	1024u

/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/                                                        
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];

static  TX_THREAD   AppTaskMsgProTCB;
static  uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];

static  TX_THREAD   AppTaskCOMTCB;
static  uint64_t    AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE/8];

static  TX_THREAD   AppTaskUserIFTCB;
static  uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];

static  TX_THREAD   AppTaskIdleTCB;
static  uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];

static  TX_THREAD   AppTaskStatTCB;
static  uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];


/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskMsgPro         (ULONG thread_input);
static  void  AppTaskUserIF         (ULONG thread_input);
static  void  AppTaskCOM			(ULONG thread_input);
static  void  AppTaskIDLE			(ULONG thread_input);
static  void  AppTaskStat			(ULONG thread_input);
static  void  App_Printf (const char *fmt, ...);
static  void  AppTaskCreate         (void);
static  void  DispTaskInfo          (void);
static  void  AppObjCreate          (void);
static  void  OSStatInit (void);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
static  TX_MUTEX   AppPrintfSemp;	/* 用于printf互斥 */


/* 统计任务使用 */
__IO uint8_t   OSStatRdy;        /* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;        /* 空闲任务计数 */
__IO float     OSCPUUsage;       /* CPU百分比 */
uint32_t       OSIdleCtrMax;     /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun;     /* 1秒内空闲任务当前计数 */

/*******************************************************************************
  * @FunctionName: main
  * @Author:       trx
  * @DateTime:     2022骞?鏈?5鏃0:45:22 
  * @Purpose:      
  * @param:        void               
  * @return:
*******************************************************************************/
int main(void)
{
//	uint8_t ucKeyCode;							/* 按键代码 */
	HAL_Init();                    	 			/* 初始化HAL库 */   
	Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */

#if	DEBUG_SWITCH_EN == 1
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
#endif

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
    /* 进入ThreadX内核 */
    tx_kernel_enter();
	while (1)
		{
//			ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
//			if (ucKeyCode != KEY_NONE)
//			{
//				switch (ucKeyCode)
//				{
//					case KEY_0_DOWN:			/* K1键按下(长按超过1s时，连发按下) */
//						printf("K0键按下\r\n");
//						//EPD_2in9b_V3_test();	/* 微雪墨水屏测试例程，进行了移植，并且显示“真缝合怪” 但是占用太高，高达30k的sram，目前看情况开启*/
//						break;
//					case KEY_0_UP:				/* K1键弹起 */
//						printf("K0键弹起\r\n");
//						break;
//					case KEY_0_LONG:			/* K1键长按(一次) */
//						printf("K0键长按\r\n");
//						break;
//					case KEY_UP_DOWN:			/* Kup键按下(长按超过1s时，连发按下) */
//						printf("KUP键按下\r\n");
//						break;
//					case KEY_UP_UP:				/* Kup键弹起 */
//						printf("KUP键弹起\r\n");
//						break;
//					case KEY_UP_LONG:			/* Kup键长按(一次) */
//						printf("kup键长按\r\n");
//						break;					/* 组合按键长按(一次) */
//					case KEY_MULTI_LONG:
//						printf("组合键长按\r\n");
//						break;
//					case KEY_MULTI_DOWM:		/* 组合按键按下(长按超过1s时，连发按下) */
//						printf("组合键按下\r\n");
//						break;
//					case KEY_MULTI_UP:			/* 组合按键弹起 */
//						printf("组合按键弹起\r\n");
//						break;
//					default:
//						/* 其它的键值不处理 */
//						break;
//				}
//			}
		}
}

void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();
}


/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无
*********************************************************************************************************
*/
void  tx_application_define(void *first_unused_memory)
{
	/*
	   如果实现任务CPU利用率统计的话，此函数仅用于实现启动任务，统计任务和空闲任务，其它任务在函数
	   AppTaskCreate里面创建。
	*/
	/**************创建启动任务*********************/
    tx_thread_create(&AppTaskStartTCB,              /* 任务控制块地址 */   
                       "App Task Start",              /* 任务名 */
                       AppTaskStart,                  /* 启动任务函数地址 */
                       0,                             /* 传递给任务的参数 */
                       &AppTaskStartStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_START_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_START_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_START_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                 /* 创建后立即启动 */
   	   
	/**************创建统计任务*********************/
    tx_thread_create(&AppTaskStatTCB,               /* 任务控制块地址 */    
                       "App Task STAT",              /* 任务名 */
                       AppTaskStat,                  /* 启动任务函数地址 */
                       0,                           /* 传递给任务的参数 */
                       &AppTaskStatStk[0],           /* 堆栈基地址 */
                       APP_CFG_TASK_STAT_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_STAT_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_STAT_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             /* 不开启时间片 */
                       TX_AUTO_START);               /* 创建后立即启动 */
					   
				   
	/**************创建空闲任务*********************/
    tx_thread_create(&AppTaskIdleTCB,               /* 任务控制块地址 */    
                       "App Task IDLE",              /* 任务名 */
                       AppTaskIDLE,                  /* 启动任务函数地址 */
                       0,                           /* 传递给任务的参数 */
                       &AppTaskIdleStk[0],           /* 堆栈基地址 */
                       APP_CFG_TASK_IDLE_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_IDLE_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_IDLE_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             /* 不开启时间片 */
                       TX_AUTO_START);               /* 创建后立即启动 */
			   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (ULONG thread_input)
{
	(void)thread_input;

	/* 优先执行任务统计 */
	OSStatInit();

	/* 内核开启后，恢复HAL里的时间基准 */
    HAL_ResumeTick();
	
    /* 外设初始化 */
	bsp_InitDWT();								/* 初始化DWT */
	bsp_InitTimer();							/* 初始化滴答定时器 */
	bsp_InitUart();								/* 初始化串口1外设 */
	bsp_InitKey();
	bsp_InitSPI1Bus();							/* SPI总线初始化 */
	bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
	bsp_InitSPI2Bus();
	bsp_I2C_EE_Init();
	bsp_InitLed();
	
	/* 创建任务 */
    AppTaskCreate(); 

	/* 创建任务间通信机制 */
	AppObjCreate();	

    while (1)
	{  
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
        bsp_ProPer1ms();
        tx_thread_sleep(1);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: 消息处理，这里用作浮点数串口打印
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskMsgPro(ULONG thread_input)
{
	double f_a = 1.1;
	double f_b = 2.2345;
	
	(void)thread_input;
		  
	while(1)
	{
		f_a += 0.00000000001;
		f_b -= 0.00000000002;
		App_Printf("AppTaskMsg: f_a = %.11f, f_b = %.11f\r\n", f_a, f_b);
        tx_thread_sleep(500);
	}   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* 按键代码 */
	
	(void)thread_input;
		  
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_0_DOWN:			  /* K1键按打印任务执行情况 */
					 DispTaskInfo();
					break;
				
				default:                     /* 其他的键值不处理 */
					break;
			}
		}

        tx_thread_sleep(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 浮点数串口打印
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 5
*********************************************************************************************************
*/
static void AppTaskCOM(ULONG thread_input)
{	
	double f_c = 1.1;
	double f_d = 2.2345;
	
	(void)thread_input;
	
	while(1)
	{
		f_c += 0.00000000001;
		f_d -= 0.00000000002;;
		App_Printf("AppTaskCom: f_a = %.11f, f_b = %.11f\r\n", f_c, f_d);
        bsp_LedToggle(2);
		bsp_LedToggle(1);
        tx_thread_sleep(500);
	} 			  	 	       											   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStatistic
*	功能说明: 统计任务，用于实现CPU利用率的统计。为了测试更加准确，可以开启注释调用的全局中断开关
*	形    参: thread_input 创建该任务时传递的形参 
*	返 回 值: 无
*   优 先 级: 30
*********************************************************************************************************
*/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    tx_thread_sleep(2u);        /* 时钟同步 */
	
    //__disable_irq();
    OSIdleCtr    = 0uL;         /* 清空闲计数 */
	//__enable_irq();
	
    tx_thread_sleep(100);       /* 统计100ms内，最大空闲计数 */
	
   	//__disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* 保存最大空闲计数 */
    OSStatRdy    = TRUE;
	//__enable_irq();
}

static void AppTaskStat(ULONG thread_input)
{
	(void)thread_input;

    while (OSStatRdy == FALSE) 
	{
        tx_thread_sleep(200);     /* 等待统计任务就绪 */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
	
    //__disable_irq();
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* 设置初始CPU利用率 0% */
	//__enable_irq();
	
    for (;;) 
	{
       // __disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* 获得100ms内空闲计数 */
        OSIdleCtr    = 0uL;          /* 复位空闲计数 */
       //	__enable_irq();            /* 计算100ms内的CPU利用率 */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        tx_thread_sleep(100);        /* 每100ms统计一次 */
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskIDLE
*	功能说明: 空闲任务
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 31
*********************************************************************************************************
*/
static void AppTaskIDLE(ULONG thread_input)
{	
  TX_INTERRUPT_SAVE_AREA

  (void)thread_input;
	
  while(1)
  {
	   TX_DISABLE
       OSIdleCtr++;
	   TX_RESTORE
  }			  	 	       											   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************创建MsgPro任务*********************/
    tx_thread_create(&AppTaskMsgProTCB,               /* 任务控制块地址 */    
                       "App Msp Pro",                 /* 任务名 */
                       AppTaskMsgPro,                  /* 启动任务函数地址 */
                       0,                             /* 传递给任务的参数 */
                       &AppTaskMsgProStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_MsgPro_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_MsgPro_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_MsgPro_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                /* 创建后立即启动 */
   

	/**************创建USER IF任务*********************/
    tx_thread_create(&AppTaskUserIFTCB,               /* 任务控制块地址 */      
                       "App Task UserIF",              /* 任务名 */
                       AppTaskUserIF,                  /* 启动任务函数地址 */
                       0,                              /* 传递给任务的参数 */
                       &AppTaskUserIFStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_USER_IF_STK_SIZE,  /* 堆栈空间大小 */  
                       APP_CFG_TASK_USER_IF_PRIO,      /* 任务优先级*/
                       APP_CFG_TASK_USER_IF_PRIO,      /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                 /* 创建后立即启动 */

	/**************创建COM任务*********************/
    tx_thread_create(&AppTaskCOMTCB,               /* 任务控制块地址 */    
                       "App Task COM",              /* 任务名 */
                       AppTaskCOM,                  /* 启动任务函数地址 */
                       0,                           /* 传递给任务的参数 */
                       &AppTaskCOMStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_COM_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_COM_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_COM_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             /* 不开启时间片 */
                       TX_AUTO_START);               /* 创建后立即启动 */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	 /* 创建互斥信号量 */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1]; /* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list   v_args;


    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

	/* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

    printf("%s", buf_str);

    tx_mutex_put(&AppPrintfSemp);
}

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* 定义一个任务控制块指针 */

    p_tcb = &AppTaskStartTCB;
	
	/* 打印标题 */
	App_Printf("===============================================================\r\n");
	App_Printf("OS CPU Usage = %5.2f%%\r\n", OSCPUUsage);
	App_Printf("===============================================================\r\n");
	App_Printf(" 任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名\r\n");
	App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* 遍历任务控制块列?TCB list)，打印所有的任务的优先级和名?*/
	while (p_tcb != (TX_THREAD *)0) 
	{
		
		App_Printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                    p_tcb->tx_thread_name);


        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
	}
}





