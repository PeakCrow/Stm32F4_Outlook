#include "system_task_create.h"

/*
*********************************************************************************************************
*                                 任务优先级，数值越小优先级越高
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          7u
#define  APP_CFG_TASK_MsgPro_PRIO                         3u
#define  APP_CFG_TASK_USER_IF_PRIO                        4u
#define  APP_CFG_TASK_COM_PRIO                            5u
#define  APP_CFG_TASK_STAT_PRIO                           30u
#define  APP_CFG_TASK_IDLE_PRIO                           31u
#define  APP_CFG_TASK_READC_PRIO						  6u
#define  APP_CFG_TASK_TFTLCD_PRIO						  2u


/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    1024u
#define  APP_CFG_TASK_COM_STK_SIZE                      1024u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  1024u
#define  APP_CFG_TASK_IDLE_STK_SIZE                  	1024u
#define  APP_CFG_TASK_STAT_STK_SIZE                  	1024u
#define  APP_CFG_TASK_READC_STK_SIZE                    1024u
#define  APP_CFG_TASK_TFTLCD_STK_SIZE                   2*2048u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   1024u
/*
*********************************************************************************************************
*                                       静态全局变量
*********************************************************************************************************
*/
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];

static  TX_THREAD   AppTaskCOMTCB;
static  uint64_t    AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE/8];

static  TX_THREAD   AppTaskUserIFTCB;
static  uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];

static  TX_THREAD   AppTaskIdleTCB;
static  uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];

static  TX_THREAD   AppTaskStatTCB;
static  uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];

static  TX_THREAD	AppTaskReadAdcTCB;
static  uint64_t	AppTaskReadAdcStk[APP_CFG_TASK_READC_STK_SIZE/8];

static  TX_THREAD	AppTaskTFTLCDTCB;
static  uint64_t	AppTaskTFTLCDStk[APP_CFG_TASK_TFTLCD_STK_SIZE/8];

static  TX_THREAD   AppTaskMsgProTCB;
static  uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];

/*
*********************************************************************************************************
*                                       软件定时器变量
*********************************************************************************************************
*/
TX_TIMER AppTimer;
TX_MUTEX AppPrintfSemp;				/* 用于printf互斥 */
TX_MUTEX AppLCDSemp;				/* 用于LVGL互斥 */
TX_MUTEX App_PowerDownSave;			/* 用于掉电保存 */  
TX_EVENT_FLAGS_GROUP  EventGroup; 	/* 事件标志组 */
/*
*********************************************************************************************************
*                                      函数声明
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskUserIF         (ULONG thread_input);
static  void  AppTaskCOM			(ULONG thread_input);
static  void  AppTaskIDLE			(ULONG thread_input);
static  void  AppTaskStat			(ULONG thread_input);
static  void  AppTaskREADADC		(ULONG thread_input);
static  void  AppTaskTFTLCD			(ULONG thread_input);
static  void  AppTaskMsgPro         (ULONG thread_input);  
static  void  AppTaskCreate         (void);
static  void  DispTaskInfo          (void);
static  void  AppObjCreate          (void);
static  void  OSStatInit 			(void);
        void  App_Printf 			(const char *fmt, ...);
        void  App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);
        void  TimerCallback         (ULONG thread_input);
/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
__IO uint8_t   OSStatRdy;      		/* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;     	    /* 空闲任务计数 */
__IO float     OSCPUUsage;   	    /* CPU百分比 */
uint32_t       OSIdleCtrMax; 	    /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun; 	    /* 1秒内空闲任务当前计数 */


/*******************************************************************************
  * @FunctionName: OSStatInit
  * @Author:       trx
  * @DateTime:     2022年5月24日23:52:39 
  * @Purpose:      具体的操作内容
  * @param:        void               
  * @return:       none
*******************************************************************************/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    tx_thread_sleep(2u);        /* 时钟同步 */
	
    __disable_irq();
    OSIdleCtr    = 0uL;         /* 清空闲计数 */
	__enable_irq();
	
    tx_thread_sleep(100);       /* 统计100ms内，最大空闲计数 */
	
   	__disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* 保存最大空闲计数 */
    OSStatRdy    = TRUE;
	__enable_irq();
}


/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数，此函数会在tx_kernel_enter函数中被调用(貌似)
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
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务，此函数在AppTaskStart函数中被调用
*	形    参 : 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************创建USER IF任务*********************/
    tx_thread_create(&AppTaskUserIFTCB,               	/* 任务控制块地址 */      
                       "App Task UserIF",              	/* 任务名 */
                       AppTaskUserIF,                  	/* 启动任务函数地址 */
                       0,                              	/* 传递给任务的参数 */
                       &AppTaskUserIFStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_USER_IF_STK_SIZE,  	/* 堆栈空间大小 */  
                       APP_CFG_TASK_USER_IF_PRIO,      	/* 任务优先级*/
                       APP_CFG_TASK_USER_IF_PRIO,      	/* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               	/* 不开启时间片 */
                       TX_AUTO_START);                 	/* 创建后立即启动 */
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

	/**************创建COM任务*********************/
    tx_thread_create(&AppTaskCOMTCB,               		/* 任务控制块地址 */    
                       "App Task COM",              	/* 任务名 */
                       AppTaskCOM,                  	/* 启动任务函数地址 */
                       0,                           	/* 传递给任务的参数 */
                       &AppTaskCOMStk[0],            	/* 堆栈基地址 */
                       APP_CFG_TASK_COM_STK_SIZE,    	/* 堆栈空间大小 */  
                       APP_CFG_TASK_COM_PRIO,        	/* 任务优先级*/
                       APP_CFG_TASK_COM_PRIO,        	/* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             	/* 不开启时间片 */
                       TX_AUTO_START);               	/* 创建后立即启动 */

	/**************创建READ ADC任务*********************/
    tx_thread_create(&AppTaskReadAdcTCB,               		/* 任务控制块地址 */    
                       "App Task ReadAdc",              	/* 任务名 */
                       AppTaskREADADC,                  	/* 启动任务函数地址 */
                       0,                           	/* 传递给任务的参数 */
                       &AppTaskReadAdcStk[0],            	/* 堆栈基地址 */
                       APP_CFG_TASK_READC_STK_SIZE,    	/* 堆栈空间大小 */  
                       APP_CFG_TASK_READC_PRIO,        	/* 任务优先级*/
                       APP_CFG_TASK_READC_PRIO,        	/* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             	/* 不开启时间片 */
                       TX_AUTO_START);               	/* 创建后立即启动 */
	
	/**************创建LCD DISP任务*********************/
    tx_thread_create(&AppTaskTFTLCDTCB,               		/* 任务控制块地址 */    
                       "App Task TFTLCD",              	/* 任务名 */
                       AppTaskTFTLCD,                  	/* 启动任务函数地址 */
                       0,                           	/* 传递给任务的参数 */
                       &AppTaskTFTLCDStk[0],            	/* 堆栈基地址 */
                       APP_CFG_TASK_TFTLCD_STK_SIZE,    	/* 堆栈空间大小 */  
                       APP_CFG_TASK_TFTLCD_PRIO,        	/* 任务优先级*/
                       APP_CFG_TASK_TFTLCD_PRIO,        	/* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             	/* 不开启时间片 */
                       TX_AUTO_START);               	/* 创建后立即启动 */

}

static int test_master(void)
{
	setNodeId(&TestSlave_Data_Data,0x00);
	setState(&TestSlave_Data_Data,Initialisation);
	setState(&TestSlave_Data_Data,Operational);
	return 0;
}
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
	bsp_InitADS1256();							/* 初始化配置ADS1256.  PGA=1, DRATE=30KSPS, BUFEN=1, 输入正负5V */
	bsp_InitWs2812b();							/* 初始化ws2812b可调灯效 */
	bsp_SetTIMOutPWM(GPIOB,GPIO_PIN_6,TIM4,1,500,5000);/* 生成一个1k，50占空比的方波，用来验证脉冲计数 */
	bsp_InitSPI1Bus();							/* SPI1总线初始化 */
	bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
    bsp_InitSram();                             /* 外部sram初始化 */
	lv_init(); 									/* lvgl 系统初始化 */
	lv_port_disp_init(); 						/* lvgl 显示接口初始化,放在 lv_init()的后面 */
	lv_port_indev_init(); 						/* lvgl 输入接口初始化,放在 lv_init()的后面 */
	
	test_master();
	
	/* 创建任务间通信机制,主要是各种任务间通讯函数 */
	AppObjCreate();

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
*	函 数 名: AppTaskStatistic
*	功能说明: 统计任务，用于实现CPU利用率的统计。为了测试更加准确，可以开启注释调用的全局中断开关
*	形    参 : thread_input 创建该任务时传递的形参 
*	返 回 值: 无
*   优 先 级: 30
*********************************************************************************************************
*/
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
	
    __disable_irq();
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* 设置初始CPU利用率 0% */
	__enable_irq();
	
    for (;;) 
	{
        __disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* 获得100ms内空闲计数 */
        OSIdleCtr    = 0uL;          /* 复位空闲计数 */
        __enable_irq();            /* 计算100ms内的CPU利用率 */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        tx_thread_sleep(100);        /* 每100ms统计一次 */
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskIDLE
*	功能说明: 空闲任务
*	形    参 : thread_input 创建该任务时传递的形参
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


/******************************************创建下面是子任务**********************************************/
/******************************************创建下面是子任务**********************************************/
/******************************************创建下面是子任务**********************************************/




static void AppTaskMsgPro(ULONG thread_input)
{
	(void)thread_input;
	ULONG actual_events;
	UINT status;
	//uint8_t stop_motor[] = {0xe0,0xf7};


	while(1)
	{				
		status = tx_event_flags_get(&EventGroup,     /* 事件标志控制块 */
									DRIVER_POS_ALL,  /* 等待标志 */
									TX_OR_CLEAR ,    /* 等待任意bit满足即可 */
									&actual_events,  /* 获取实际值 */
									TX_WAIT_FOREVER);/* 永久等待 */		
		
		if(status == TX_SUCCESS)
		{
			switch (actual_events)
			{
				case DRIVER1_POS:			  
					/* */
				
					break;
				
				case DRIVER2_POS:			  
					/* */

					break;
				
				case DRIVER3_POS:			 
					/*  */

					break;
				
				default:       
					break;
			}		
		}
		tx_thread_sleep(200);
	}   
}




/*******************************************************************************
  * @FunctionName: AppTaskTFTLCD
  * @Author:       trx
  * @DateTime:     2022年6月23日21:03:00 
  * @Purpose:      lcd屏幕测试任务
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
static void AppTaskTFTLCD    (ULONG thread_input)
{
	(VOID)thread_input;
	uint8_t lcd_id[12];				/* 存放LCD ID字符串 */
	
	App_Printf((char*)lcd_id,"LCD ID:%04X",lcddev.id);
 
	#if 1
	tx_mutex_get(&AppLCDSemp, TX_WAIT_FOREVER);
	Gui_Monitor_App();	/* 运行lvgl例程 */
	tx_mutex_put(&AppLCDSemp);
	while(1)
	{
		tx_mutex_get(&AppLCDSemp, TX_WAIT_FOREVER);
 		lv_timer_handler();
		//IWDG_Feed();
		tx_mutex_put(&AppLCDSemp);
		tx_thread_sleep(5);
	}
	
	#else
 	//Load_Drow_Dialog();
	if(tp_dev.touchtype&0X80)
		ctp_test();
	#endif
}

/*******************************************************************************
  * @FunctionName: AppTaskREADADC
  * @Author:       trx
  * @DateTime:     2022年6月23日21:03:14 
  * @Purpose:      adc数据读取任务
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
static  void  AppTaskREADADC	(ULONG thread_input)
{
	(VOID)thread_input;
//	uint8_t i;
//	int32_t iTemp;
//	float fTemp;
	

		/* 打印芯片ID (通过读ID可以判断硬件接口是否正常) , 正常时状态寄存器的高4bit = 3 */
#if 0
		{
			uint8_t id;
	
			id = ADS1256_ReadChipID();
	
			if (id != 3)
			{
				App_Printf("Error, ASD1256 Chip ID = 0x%X\r\n", id);
			}
			else
			{
				App_Printf("Ok, ASD1256 Chip ID = 0x%X\r\n", id);
			}
		}
#endif
		
		ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_30SPS);	/* 配置ADC参数： 增益1:1, 数据输出速率 30Hz */
	
		ADS1256_StartScan();	/* 启动中断扫描模式, 轮流采集8个通道的ADC数据. 通过 ADS1256_GetAdc() 函数来读取这些数据 */
while (1)
	{
		#if 0
		/* 打印采集数据 */
		for (i = 0; i < 8; i++)
			{

			    
			    //计算公式 = 2 * VREF/(PGA * (2^23 - 1)) ，这里VREF是2.5V，PGA = 1
			    //计算实际电压值（近似估算的），如需准确，请进行校准
			    
				iTemp = ((int64_t)g_tADS1256.AdcNow[i] * 2500000) / 4194303; 
				
				fTemp = (float)iTemp / 1000000;   

				App_Printf("CH%d=%07d(%fV) ", i, g_tADS1256.AdcNow[i], fTemp);

				if(i == 3)
					{
						App_Printf("\r\n");
					}
			}
		App_Printf("\r\n\r\n");
		#else
		//iTemp = ((int64_t)g_tADS1256.AdcNow[5] * 2500000) / 4194303; 
		
		//fTemp = (float)iTemp / 1000000;   
		
		//App_Printf("CH%d=%07d(%fV) ", 6, g_tADS1256.AdcNow[5], fTemp);
		//App_Printf("\r\n");

		/* 不运行是什么情况？？？ */
		//App_Printf("driver 1 pos is : %.2f driver current pos is %.2f\r\n",DriverX_Pos.driver1_pos,DriverX_Pos.current_pos);		
		#endif
		tx_thread_sleep(100);
	}
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理
*	形    参 : thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
static void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* 按键代码 */
	(void)thread_input;
	uint16_t pos = 0;
	uint8_t buf[2] = {0};
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode != KEY_NONE)
		{
				switch(ucKeyCode)
				{
					case KEY_0_UP: 			  /* K1键按打印任务执行情况 */
						App_Printf("k0按键弹起\r\n");						
					 	break;
					case KEY_UP_DOWN:			/* kup按键按下 */
						App_Printf("kup按键按下\r\n");				//红色	
						break;
					case KEY_0_DOWN:			/* k0按键按下 */
					{
						App_Printf("k0按键按下\r\n");	
						DispTaskInfo();
						break;
					}
					case KEY_UP_UP:
						//I2C_EE_BufferRead(&pos1,0x05,1);
						//I2C_EE_BufferRead(&pos2,0x06,1);
						I2C_EE_BufferRead(buf,0x05,2);
						pos = buf[0] * 100 + buf[1];
						App_Printf("kup按键弹起,pos %d  \r\n",pos);
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

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参 : 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1];/* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
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

void    App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
	/* 互斥操作 */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);

    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);

    tx_mutex_put(&App_PowerDownSave);	
}


/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-III任务信息通过串口打印出来
*	形    参 ：无
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
static  void  AppObjCreate (void)
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
    




