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
#define  APP_CFG_TASK_READC_PRIO						  6u
#define  APP_CFG_TASK_TFTLCD_PRIO						  7u


/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    1024u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   1024u
#define  APP_CFG_TASK_COM_STK_SIZE                      1024u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  1024u
#define  APP_CFG_TASK_IDLE_STK_SIZE                  	1024u
#define  APP_CFG_TASK_STAT_STK_SIZE                  	1024u
#define  APP_CFG_TASK_READC_STK_SIZE                    1024u
#define  APP_CFG_TASK_TFTLCD_STK_SIZE                   1024u


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


/*
*********************************************************************************************************
*                                       软件定时器变量
*********************************************************************************************************
*/
TX_TIMER AppTimer;

/*
*********************************************************************************************************
*                                       软件定时器回调函数
*********************************************************************************************************
*/
void TimerCallback(ULONG thread_input);


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
static  void  App_Printf 			(const char *fmt, ...);
static  void  AppTaskCreate         (void);
static  void  DispTaskInfo          (void);
static  void  AppObjCreate          (void);
static  void  OSStatInit 			(void);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
static  TX_MUTEX   AppPrintfSemp;	/* 用于printf互斥 */


/* 统计任务使用 */
__IO uint8_t   OSStatRdy;      		/* 统计任务就绪标志 */
__IO uint32_t  OSIdleCtr;     	    /* 空闲任务计数 */
__IO float     OSCPUUsage;   	    /* CPU百分比 */
uint32_t       OSIdleCtrMax; 	    /* 1秒内最大的空闲计数 */
uint32_t       OSIdleCtrRun; 	    /* 1秒内空闲任务当前计数 */

/*******************************************************************************
  * @FunctionName: main
  * @Author:       trx
  * @DateTime:     2022年4月25日09点28分
  * @Purpose:      
  * @param:        void               
  * @return:
*******************************************************************************/
int main(void)
{
	HAL_Init();                    	 			/* 初始化HAL库 */   
	Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */

#if	DEBUG_SWITCH_EN == 1
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
#endif

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
    /* 进入ThreadX内核 */
    tx_kernel_enter();
	while(1)
		;
}

void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();	/* 轻触按键扫描函数 */
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
	 	/* 定时器组 */
	tx_timer_create(&AppTimer,
					"App Timer",
					TimerCallback, 
					0,                  /* 传递的参数 */
					500,                /* 设置定时器时间溢出的初始延迟，单位ThreadX系统时间节拍数 */
					500, 				/* 设置初始延迟后的定时器运行周期，如果设置为0，表示单次定时器,单位ms */
					TX_AUTO_ACTIVATE);	/* 激活定时器 */
}
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
	bsp_InitSPI1Bus();							/* SPI总线初始化 */
	bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
	bsp_InitSPI2Bus();							/* 初始化SPI2总线，用来驱动墨水屏 */
	bsp_I2C_EE_Init();							/* 初始化IIC总线，并且驱动eeprom芯片 */
	bsp_InitLed();								/* 初始化板载LED灯 */
	bsp_InitCan1Bus();							/* 初始化CAN1 总线 */
	bsp_InitWs2812b();							/* 初始化ws2812b可调灯效 */
	bsp_InitRotationSensor();					/* 初始化轮速传感器 */
	bsp_SetTIMOutPWM(GPIOB,GPIO_PIN_6,TIM4,1,500,5000);/* 生成一个1k，50占空比的方波，用来验证脉冲计数 */	
	//bsp_InitADS1256();							/* 初始化配置ADS1256.  PGA=1, DRATE=30KSPS, BUFEN=1, 输入正负5V */
	//bsp_Initlcd();								/* 初始化LCD屏幕 */
	//tp_dev.init();
	/* 创建任务，此函数中包含有3个子任务 */
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
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color)
{
	if(len==0)
		return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径，这里的半径同样相当于线条的粗细程度
//color:颜色
void gui_fill_circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
{											  
	uint32_t i;
	uint32_t imax = ((uint32_t)r*707)/1000+1;
	uint32_t sqmax = (uint32_t)r*(uint32_t)r+(uint32_t)r/2;
	uint32_t x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)//如果坐标值小于线条的大小，则退出函数，不进行画线
		return; 
	delta_x=x2-x1; //计算坐标增量 ，每次触摸的时候会连续读两次的坐标数值
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	
	if(delta_x>0)
		incx=1; //设置单步方向 正方向
	else if(delta_x==0)//x的坐标差值为0，代表是垂直线
		incx=0;//垂直线 
	else 
		{
			incx=-1;//负方向
			delta_x=-delta_x;//插值变为负值
		} 
		
	if(delta_y>0)//进行与x坐标值同样的操作
		incy=1; //正方向
	else if(delta_y==0)
		incy=0;//水平线 
	else
	{
		incy=-1;//负方向
		delta_y=-delta_y;
	} 
	
	
	if( delta_x>delta_y)
		distance=delta_x; //选取基本增量坐标轴 
	else 
		distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(TFT_WHITE);//清屏   
 	POINT_COLOR=TFT_BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=TFT_RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜色(电容触摸屏用)												 
const uint16_t POINT_COLOR_TBL[5]={TFT_RED,TFT_GREEN,TFT_BLUE,TFT_BROWN,TFT_GRED};  
//电容触摸屏测试函数
void ctp_test(void)
{
	uint8_t t=0;
	uint8_t i=0;	  	    
 	uint16_t lastpos[5][2];		//存放临时的坐标数据 
	while(1)//在while循环中不停的读取tp_dev.x 与 y的坐标，并且进行相应的操作
	{
		tp_dev.scan(0);
		for(t=0;t<5;t++)//这里t循环几次就代表可以同时画几条线,最多只能画5条
		{
			if((tp_dev.sta)&(1<<t))
			{
                //printf("X坐标:%d,Y坐标:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
                    
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else 
				lastpos[t][0]=0XFFFF;
		}
		
		tx_thread_sleep(5);i++;
		//if(i%20==0)LED0=!LED0;
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
	uint8_t lcd_id[12];				//存放LCD ID字符串
	
	App_Printf((char*)lcd_id,"LCD ID:%04X",lcddev.id);

	POINT_COLOR=TFT_RED;
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"TOUCH TEST");	
	LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,"2017/4/14");
	
 	Load_Drow_Dialog();	
	//if(tp_dev.touchtype&0X80)
		//ctp_test();
	
	while (1)
		{
			LCD_Clear(TFT_GREEN);
			tx_thread_sleep(1000);
			LCD_Clear(TFT_BLUE);
			tx_thread_sleep(1000);
		}
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
	int32_t iTemp;
	float fTemp;
		/* 打印芯片ID (通过读ID可以判断硬件接口是否正常) , 正常时状态寄存器的高4bit = 3 */
#if 1
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
		iTemp = ((int64_t)g_tADS1256.AdcNow[5] * 2500000) / 4194303; 
		
		fTemp = (float)iTemp / 1000000;   
		
		//App_Printf("CH%d=%07d(%fV) ", i, g_tADS1256.AdcNow[5], fTemp);
		//App_Printf("\r\n");
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
	
	float temp;
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode != KEY_NONE)
		{
				switch(ucKeyCode)
				{
					case KEY_0_UP: 			  /* K1键按打印任务执行情况 */
						App_Printf("k0按键弹起\r\n");
					 	//DispTaskInfo();
					 	//HAL_I2C_Master_Transmit(&iic_handle,0x00,buf1,2,100);
					//I2C_EE_BufferRead(buf2,tmp+1,3);
					 	break;
					case KEY_UP_DOWN:			/* kup按键按下 */
						App_Printf("kup按键按下\r\n");				//红色	
						temp = bsp_MLX90614_ReadTemp();
						App_Printf("%.1f\r\n",temp);
						break;
					case KEY_0_DOWN:			/* k0按键按下 */
					{
						App_Printf("k0按键按下\r\n");
						//Ws2812b_Run_Water_Lamp(0xff,0x00,0x00,1000,gradua_on);/* 当按下key0后，会进行12s的延时，才能触发下次的按键输入bug */
					break;
					}
					case KEY_UP_UP:
						App_Printf("kup按键弹起");
						//Ws2812b_Set_Alloff();
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
	App_Printf("AppTaskCom任务开始执行\r\n");
	while(1)
	{
        bsp_LedToggle(2);
		bsp_LedToggle(1);
        tx_thread_sleep(1000);
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


