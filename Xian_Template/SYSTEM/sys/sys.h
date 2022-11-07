#ifndef _SYS_H_
#define _SYS_H_


/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/
#include <stdarg.h>					/* C库文件，参数个数未知时获取函数中的参数 */
#include <stdio.h>					/* C库文件，标准输入输出函数 */
#include <stdlib.h>					/* C库文件，定义了4个变量类型，一些宏和各种通用工具函数 */
#include <math.h>					/* C库文件，各种数学函数 */
#include <string.h>					/* C库文件，各种操作字符数组的函数 */


/*
*********************************************************************************************************
*                                           OS/FileX
*********************************************************************************************************
*/
#include "tx_api.h"
#include "tx_timer.h"
#include "tx_api.h"

/*
*********************************************************************************************************
*                                        	UI
*********************************************************************************************************
*/
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "Monitor.h"
/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/
#include "stm32f4xx.h"
#include "ticktim.h"						/* 软件定时器 */
#include "bsp_usart_fifo.h"			/* 串口通讯 */
#include "bsp_key.h"						/* 轻触按键 */
#include "bsp_spi_flash_demo.h"	/* spi flash 驱动 demo test*/
#include "bsp_spi_bus.h"				/* spi1 spi2 总线 */
#include "bsp_iic_bus.h"				/* IIC1总线(包含AT24C02芯片) */
#include "EPD_Test.h"						/* 墨水屏驱动 */
#include "bsp_font.h"
#include "bsp_dwt.h"						/* 芯片DWT模块，安富莱教程提供 */
#include "bsp_led.h"						/* 板载led灯 */
#include "bsp_spi_flash.h"			/* spi flash驱动 */
#include "bsp_can_bus.h"				/* can1 总线 */
#include "bsp_tim_pwm.h"				/* 定时器中断初始化和PWM初始化 */
#include "bsp_ws2812b.h"				/* ws2812b三色RGB灯 */
#include "bsp_rotation_sensor.h"/* 轮速传感器 */
#include "bsp_spi_ads1256.h"		/* 24位ADC模拟量采集 */
#include "bsp_mlx90614.h"				/* mlx90614红外温度传感器初始化 */
#include "bsp_font.h"						/* 屏幕显示的字体C库文件 */
#include "bsp_lcd.h"						/* 屏幕显示驱动 */
#include "bsp_touch.h"					/* 电容屏触摸驱动,包含有触摸画板例程 */
#include "bsp_sdio_sd.h"				/* SD卡底层驱动 */	 

/***********************************ThreadX操作系统组件属性外部声名*************************************************************/
extern void  App_Printf 			(const char *fmt, ...);
extern TX_EVENT_FLAGS_GROUP  EventGroup; /* 事件标志组 */

#if	DEBUG_SWITCH_EN == 1
#include "EventRecorder.h"			/* 内部dedbug调试组件 */
#endif

/* 定义一些常用的数据类型短关键字 */
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;

/*
*********************************************************************************************************
*                                          变量和函数
*********************************************************************************************************
*/

#define DISABLE   0
#define ENABLE    1
#define DISABLED  0
#define ENABLED   1
#define OFF       0
#define ON        1
#define FALSE     0
#define TRUE      1

/* 方便RTOS里面使用 */
extern void SysTick_ISR(void);		/* 滴答定时器中断外部文件声明 */

#define bsp_ProPer1ms  SysTick_ISR

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) 		//0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) 		//0x40020414
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) 		//0x40020814
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) 		//0x40020C14
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) 		//0x40021014
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) 		//0x40021414
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) 		//0x40021814
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) 		//0x40021C14
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) 		//0x40022014
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) 		//0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) 		//0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) 		//0x40020010
#define GPIOB_IDR_Addr    (GPIOB_BASE+16)		//0x40020410
#define GPIOC_IDR_Addr    (GPIOC_BASE+16)		//0x40020810
#define GPIOD_IDR_Addr    (GPIOD_BASE+16)		//0x40020C10
#define GPIOE_IDR_Addr    (GPIOE_BASE+16)		//0x40021010
#define GPIOF_IDR_Addr    (GPIOF_BASE+16)		//0x40021410
#define GPIOG_IDR_Addr    (GPIOG_BASE+16)		//0x40021810
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) 		//0x40021C10
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) 		//0x40022010
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) 		//0x40022410
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) 		//0x40022810

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  	//输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n) 	//输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n) 	//输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n) 	//输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n) 	//输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n) 	//输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n) 	//输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n) 	//输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n) 	//输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n) 	//输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n) 	//输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n) 	//输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)	//输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)	//输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n) 	//输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n) 	//输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n) 	//输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n) 	//输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n) 	//输出 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n) 	//输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)	//输出 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)	//输入 




void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq);//时钟系统配置
/*
*********************************************************************************************************
*                                           汇编函数，nouseful
*********************************************************************************************************
*/

void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);	//设置堆栈地址 

#define  USE_THREADX    1						/* 配置是否使用threadx操作系统 */

/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/

/*   最快速度优化需要开启的选项 : */
		/*
        TX_MAX_PRIORITIES                       32
        TX_DISABLE_PREEMPTION_THRESHOLD
        TX_DISABLE_REDUNDANT_CLEARING
        TX_DISABLE_NOTIFY_CALLBACKS
        TX_NOT_INTERRUPTABLE
        TX_TIMER_PROCESS_IN_ISR
        TX_REACTIVATE_INLINE
        TX_DISABLE_STACK_FILLING
        TX_INLINE_THREAD_RESUME_SUSPEND
*/
/*   最小代码优化需要开启的选项:  */
/*
#define        TX_MAX_PRIORITIES                       32
#define        TX_DISABLE_PREEMPTION_THRESHOLD
#define        TX_DISABLE_REDUNDANT_CLEARING
#define        TX_DISABLE_NOTIFY_CALLBACKS
#define        TX_NOT_INTERRUPTABLE
#define        TX_TIMER_PROCESS_IN_ISR
*/


/* 覆盖tx_port.h 里面的宏定义  */
/*
#define TX_MAX_PRIORITIES                       32
#define TX_MINIMUM_STACK                        ????
#define TX_THREAD_USER_EXTENSION                ????
#define TX_TIMER_THREAD_STACK_SIZE              ????
#define TX_TIMER_THREAD_PRIORITY                ????
*/

/* 
   确定定时器是否到期的处理，比如应用定时器，溢出时间和函数tx_thread_sleep调用等，是在系统定时器任务里面还是在定时器中断里面调用。
   默认是在定时任务里面，当定义了下面函数后，将直接在定时器中断里面处理，可以去掉定时器任务所消耗资源。 */
//#define TX_TIMER_PROCESS_IN_ISR


/* 用于设置定时器激活是否采用内联方式，默认此功能是关闭的。如果使能后，内联方式的执行速度快，但增加代码量 */
//#define TX_REACTIVATE_INLINE


/* 用于设置是否关闭栈填充，默认情况下是使能的，所有任务的栈空间全部填充为0xEF，
*  带有ThreadX调试组件或者运行时栈检测会用到。
*/
//#define TX_DISABLE_STACK_FILLING


/* 用于使能栈检测，默认是关闭的。此选项使能后，而TX_DISABLE_STACK_FILLING没使能时，栈填充将开启，方便栈检测 */
//#define TX_ENABLE_STACK_CHECKING


/* 用于设置是否关闭抢占阀值，默认是开启的。如果应用程序不需要此功能，关闭后可以降低代码需求，提升性能 */
//#define TX_DISABLE_PREEMPTION_THRESHOLD


/* 用于设置是否清零ThreadX全局变量，如果编译器启动代码在ThreadX运行前清除了.bss段，那么可以关闭不必要的清零 */
//#define TX_DISABLE_REDUNDANT_CLEARING


/* 确定是否不需要定时器组，禁止后需要用户注释掉tx_initialize_low_level文件里面tx_timer_interrupt的调用。
   另外，禁止后，必须使能TX_TIMER_PROCESS_IN_ISR */
/*
#define TX_NO_TIMER
#ifndef TX_TIMER_PROCESS_IN_ISR
#define TX_TIMER_PROCESS_IN_ISR
#endif
*/


/* 用于设置是否关闭通知回调，默认是使能的。如果应用程序没有用到消息回调，关闭掉后可以减小代码，并且可以提升性能。 */
//#define TX_DISABLE_NOTIFY_CALLBACKS


/* 使能tx_thread_resume和tx_thread_suspend使用内联代码，优势是提升这两个函数的执行性能，劣势是增加代码量 */
//#define TX_INLINE_THREAD_RESUME_SUSPEND


/* 设置TreadX内核不可中断，好处是降低处理负担，并且产生的代码小。但增加锁时间 */
//#define TX_NOT_INTERRUPTABLE


/* 使能事件Trace，会稍微增加点代码 */
//#define TX_ENABLE_EVENT_TRACE


/* 使能BLOCK_POOL信息获取 */
//#define TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO


/* 使能BYTE_POOL信息获取 */
//#define TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO


/* 使能事件标志信息获取 */
//#define TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO


/* 使能互斥信号量信息获取  */
//#define TX_MUTEX_ENABLE_PERFORMANCE_INFO


/* 使能消息对象信息获取 */
//#define TX_QUEUE_ENABLE_PERFORMANCE_INFO

/* 使能信号量信息获取  */
//#define TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO


/* 使能任务信息获取 */
//#define TX_THREAD_ENABLE_PERFORMANCE_INFO


/* 使能定时器信息获取 */
//#define TX_TIMER_ENABLE_PERFORMANCE_INFO



#endif

