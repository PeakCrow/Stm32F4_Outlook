#include "system_task_create.h"
/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define  APP_CFG_TASK_COM_PRIO           7u
#define  APP_CFG_TASK_COM_STK_SIZE       1024u
#define  TASK_START_CREATE_PRIO         		 7u
#define  TASK_START_CREATE_SIZE     			 1024u
/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/
static  TX_THREAD   AppTaskCOMTCB;
static  uint64_t    AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE/8];
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[TASK_START_CREATE_SIZE/8];
/*
*********************************************************************************************************
*                                       软件定时器变量
*********************************************************************************************************
*/
TX_TIMER AppTimer; 
TX_EVENT_FLAGS_GROUP  EventGroup; 	/* 事件标志组 */
/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskCOM			(ULONG thread_input);
static  void  AppTaskCreate         (void);
static  void  AppSysObjCreate       (void);
        void  TimerCallback         (ULONG thread_input);
/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数，此函数会在tx_kernel_enter函数中被调用
*	形    参 : first_unused_memory  未使用的地址空间
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
    tx_thread_create(&AppTaskStartTCB,              	/* 任务控制块地址 */   
                        "App Task Start",            	/* 任务名 */
                        AppTaskStart,              		/* 启动任务函数地址 */
                        0,                      		/* 传递给任务的参数 */
                        &AppTaskStartStk[0],    		/* 堆栈基地址 */
    					TASK_START_CREATE_SIZE,        		/* 堆栈空间大小 */
    					TASK_START_CREATE_PRIO,        		/* 任务优先级*/
    					TASK_START_CREATE_PRIO,        		/* 任务抢占阀值 */
    					TX_NO_TIME_SLICE,       		/* 不开启时间片 */
    					TX_AUTO_START);          		/* 创建后立即启动 */
	/**************创建统计任务*********************/
    tx_thread_create(&AppTaskStatTCB,"App Task STAT",AppTaskStat,0,&AppTaskStatStk[0],APP_CFG_TASK_STAT_STK_SIZE,
                       APP_CFG_TASK_STAT_PRIO,APP_CFG_TASK_STAT_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************创建空闲任务*********************/
    tx_thread_create(&AppTaskIdleTCB,"App Task IDLE",AppTaskIDLE,0,&AppTaskIdleStk[0],APP_CFG_TASK_IDLE_STK_SIZE,
                       APP_CFG_TASK_IDLE_PRIO,APP_CFG_TASK_IDLE_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务，此函数在AppTaskStart函数中被调用
*	形    参 : 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************创建USER IF任务*********************/
    tx_thread_create(&AppTaskUserIFTCB,"App Task UserIF",AppTaskUserIF,0,&AppTaskUserIFStk[0],APP_CFG_TASK_USER_IF_STK_SIZE,
    				   APP_CFG_TASK_USER_IF_PRIO,APP_CFG_TASK_USER_IF_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************创建MsgPro任务*********************/
    tx_thread_create(&AppTaskMsgProTCB,"App Msp Pro",AppTaskMsgPro,0,&AppTaskMsgProStk[0],APP_CFG_TASK_MsgPro_STK_SIZE,
                       APP_CFG_TASK_MsgPro_PRIO,APP_CFG_TASK_MsgPro_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************创建COM任务*********************/
    tx_thread_create(&AppTaskCOMTCB,"App Task COM",AppTaskCOM,0,&AppTaskCOMStk[0],APP_CFG_TASK_COM_STK_SIZE,
                       APP_CFG_TASK_COM_PRIO,APP_CFG_TASK_COM_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************创建LCD DISP任务*********************/
    tx_thread_create(&AppTaskTFTLCDTCB,"App Task TFTLCD",AppTaskTFTLCD,0,&AppTaskTFTLCDStk[0],APP_CFG_TASK_TFTLCD_STK_SIZE,
                       APP_CFG_TASK_TFTLCD_PRIO,APP_CFG_TASK_TFTLCD_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
}

//static int test_master(void)
//{
//	setNodeId(&TestSlave_Data_Data,0x00);
//	setState(&TestSlave_Data_Data,Initialisation);
//	setState(&TestSlave_Data_Data,Operational);
//	return 0;
//}
/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。启动任务中包含有三个子任务
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
	bsp_InitUart();								/* 初始化串口1 2 3外设 */
	bsp_InitKey();								/* 初始化轻触按键 */
	bsp_InitSPI2Bus();							/* 初始化SPI2总线，用来驱动墨水屏 */
	bsp_I2C_EE_Init();							/* 初始化IIC总线，并且驱动eeprom芯片 */
	bsp_InitLed();								/* 初始化板载LED灯 */
	bsp_InitCan1Bus();							/* 初始化CAN1 总线 */
	bsp_InitRotationSensor();					/* 初始化轮速传感器 */
	bsp_InitWs2812b();							/* 初始化ws2812b可调灯效 */
	bsp_SetTIMOutPWM(GPIOB,GPIO_PIN_6,TIM4,1,500,5000);/* 生成一个1k，50占空比的方波，用来验证脉冲计数 */
	bsp_InitSPI1Bus();							/* SPI1总线初始化 */
	bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
    bsp_InitSram();                             /* 外部sram初始化 */
	lv_init(); 									/* lvgl 系统初始化 */
	lv_port_disp_init(); 						/* lvgl 显示接口初始化,放在 lv_init()的后面 */
	lv_port_indev_init(); 						/* lvgl 输入接口初始化,放在 lv_init()的后面 */
	//test_master();
	/* 创建任务间通信机制,主要是各种任务间通讯函数 */
	AppSysObjCreate();
	/* 创建任务，此函数中包含子任务-线程任务 */
    AppTaskCreate();
	while (1)
		{
//			tx_trace_enable(&myBuf,TRC_BUF_SIZE,TRC_MAX_OBJ_COUNT);
			/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
			bsp_ProPer1ms();
			tx_thread_sleep(1);
		}
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 浮点数串口打印
*	形    参 : thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 5
*********************************************************************************************************
*/
static void AppTaskCOM(ULONG thread_input)
{
	(void)thread_input;
	App_Printf("AppTaskCom任务开始运行\r\n");
	while(1)
	{
        bsp_LedToggle(2);
		bsp_LedToggle(1);
		tx_thread_sleep(100);
	}
}
/*******************************************************************************
  * @FunctionName: TimerCallback
  * @Author:       trx
  * @DateTime:     2022年6月23日21:04:39 
  * @Purpose:      轮速数据读取，软件定时器回调函数
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
void TimerCallback(ULONG thread_input)
{
	/* 带延迟参数，且设置大于0，都不要在定时组的回调函数里面调用 */
	//App_Printf("%.1fm/s\r\n",Rotation_Sensor_Get(24,0.464));/* 1000HZ方波下应该121.41m/s */
}
/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参 : 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppSysObjCreate (void)
{
	/* 创建互斥信号量 */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
	tx_mutex_create(&AppLCDSemp,"AppLCDSemp",TX_NO_INHERIT);	
		
	/* 定时器组 */
	tx_timer_create(&AppTimer,
					"App Timer",
					TimerCallback, 
					0,                  /* 传递的参数 */
					500,                /* 设置定时器时间溢出的初始延迟，单位ThreadX系统时间节拍数 */
					1, 				    /* 设置初始延迟后的定时器运行周期，如果设置为0，表示单次定时器,单位ms */
					TX_AUTO_ACTIVATE);	/* 激活定时器 */
	
	/* 创建事件标志组 */
	tx_event_flags_create(&EventGroup, "EventGroupName");
}
void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();	/* 轻触按键扫描函数 */
}
    




