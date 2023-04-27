#ifndef _SYS_H_
#define _SYS_H_


/*
*********************************************************************************************************
*                                         ��׼��
*********************************************************************************************************
*/
#include <stdarg.h>					/* C���ļ�����������δ֪ʱ��ȡ�����еĲΔ�*/
#include <stdio.h>					/* C���ļ�����׼����������� */
#include <stdlib.h>					/* C���ļ��������ڸ��������ͣ�һЩ��͸���ͨ�ù��ߺ��� */
#include <math.h>					/* C���ļ���������ѧ���� */
#include <string.h>					/* C���ļ������ֲ����ַ�����ĺ���*/
/*
*********************************************************************************************************
*                                           ST HAL��
*********************************************************************************************************
*/
#include "stm32F407xx.h"
#include "stm32f4xx.h"
/*
*********************************************************************************************************
*                                           OS/FileX
*********************************************************************************************************
*/
#include "tx_api.h"
#include "tx_timer.h"
#include "tx_api.h"
#include "ff.h"

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
#include "ticktim.h"						/* �����ʱ��*/
#include "bsp_usart_fifo.h"					/* ����ͨѶ */
#include "bsp_key.h"						/* �ᴥ���� */
#include "bsp_spi_flash_demo.h"				/* spi flash ���� demo test*/
#include "bsp_spi_bus.h"					/* spi1 spi2 ���� */
#include "bsp_iic_bus.h"					/* IIC1����(����AT24C02оƬ) */
#include "EPD_Test.h"						/* īˮ��ģ��*/
#include "bsp_font.h"						/* īˮƿ�ֿ� */
#include "bsp_dwt.h"						/* оƬDWTģ�飬�������̳��ṩ */
#include "bsp_led.h"						/* ����led��*/
#include "bsp_spi_flash.h"					/* spi flashģ�� */
#include "bsp_can_bus.h"					/* can1 ���� */
#include "bsp_tim_pwm.h"					/* ��ʱ���жϳ�ʼ����PWM��ʼ��*/
#include "bsp_ws2812b.h"					/* ws2812b��ɫRGB��*/
#include "bsp_rotation_sensor.h"			/* ���ٴ����� */
#include "bsp_spi_ads1256.h"				/* 24λADCģ�������*/
#include "bsp_mlx90614.h"					/* mlx90614�����¶ȴ�������ʼ�� */
#include "bsp_font.h"						/* ��Ļ��ʾ������C���ķ�*/
#include "bsp_lcd.h"						/* ��Ļ��ʾģ�� */
#include "bsp_touch.h"						/* �������������ư����д�����������*/
#include "bsp_sdio_sd.h"					/* SD���ײ�ģ�� */	
#include "iwdg.h"                           /* ���Ź�ģ�� */
#include "system_task_create.h"             /* threadx����ϵͳ���񴴽� */
#include "bsp_sram.h"                       /* �ⲿ1Msramģ�� */
#include "DEV_Config.h"

#if	DEBUG_SWITCH_EN == 1
#include "EventRecorder.h"			/* �ڲ�dedbug(printf)������� */
#endif

/* ����һЩ���õ��������Ͷ̹ؼ��� */
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
*                                          �����ͺ���
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

/* ����RTOS����ʹ�� */
extern void SysTick_ISR(void);		/* �δ�ʱ���ж��ⲿ�ļ����p*/

#define bsp_ProPer1ms  SysTick_ISR

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ι�<CM3Ȩ��ָ��>>�����87ҳ~92��.M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
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

//IO�ڲ�ʹֻ�Ե�һ��IO��
//ȷ��n��ֵС��6!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  	//��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n) 	//���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n) 	//��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n) 	//���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n) 	//��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n) 	//���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n) 	//��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n) 	//���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n) 	//��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n) 	//����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n) 	//��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n) 	//����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)	//��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)	//����

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n) 	//��� 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n) 	//����

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n) 	//��� 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n) 	//����

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n) 	//��� 
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n) 	//����

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)	//��� 
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)	//���� 




void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq);//ʱ��ϵͳ����
/*
*********************************************************************************************************
*                                           ��ຯ����nouseful
*********************************************************************************************************
*/

void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������Д�
void INTX_ENABLE(void);	//���������Д�
void MSR_MSP(uint32_t addr);	//���ö�ջ��ַ 

#define  USE_THREADX    1						/* �����Ƿ�ʹ��threadx����ϵͳ */

/*
*********************************************************************************************************
*                                           �궨�v
*********************************************************************************************************
*/

/*   ����ٶ��Ż���Ҫ������ѡ�� : */
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
/*   ��С�����Ż���Ҫ������ѡ��:  */
/*
#define        TX_MAX_PRIORITIES                       32
#define        TX_DISABLE_PREEMPTION_THRESHOLD
#define        TX_DISABLE_REDUNDANT_CLEARING
#define        TX_DISABLE_NOTIFY_CALLBACKS
#define        TX_NOT_INTERRUPTABLE
#define        TX_TIMER_PROCESS_IN_ISR
*/


/* ����tx_port.h ����ĺ궨��  */
/*
#define TX_MAX_PRIORITIES                       32
#define TX_MINIMUM_STACK                        ????
#define TX_THREAD_USER_EXTENSION                ????
#define TX_TIMER_THREAD_STACK_SIZE              ????
#define TX_TIMER_THREAD_PRIORITY                ????
*/

/* 
   ȷ����ʱ���Ƿ��ڵĴ�������Ӧ�ö�ʱ�������ʱ��ͺ���tx_thread_sleep���õȣ�����ϵͳ��ʱ���������滹���ڶ�ʱ���ж��������?
   Ĭ�����ڶ�ʱ�������棬�����������溯���󣬽�ֱ���ڶ�ʱ���ж����洦������ȥ����ʱ��������������Դ?*/
//#define TX_TIMER_PROCESS_IN_ISR


/* �������ö�ʱ�������Ƿ����������ʽ��Ĭ�ϴ˹����ǹرյġ����ʹ�ܺ�������ʽ��ִ���ٶȿ죬�����Ӵ����� */
//#define TX_REACTIVATE_INLINE


/* ���������Ƿ�ر�ջ��䣬Ĭ���������ʹ�ܵģ����������ջ�ռ�ȫ�����Ϊ0xEF��
*  ����ThreadX���������������ʱջ�����õ�?
*/
//#define TX_DISABLE_STACK_FILLING


/* ����ʹ��ջ��⣬Ĭ���ǹرյġ���ѡ��ʹ�ܺ󣬶�TX_DISABLE_STACK_FILLINGûʹ��ʱ��ջ��佫����������ջ���*/
//#define TX_ENABLE_STACK_CHECKING


/* ���������Ƿ�ر���ռ��ֵ��Ĭ���ǿ����ġ����Ӧ�ó�����Ҫ�˹��ܣ��رպ���Խ��ʹ��������������� */
//#define TX_DISABLE_PREEMPTION_THRESHOLD


/* ���������Ƿ�����ThreadXȫ�ֱ������������������������ThreadX����ǰ�����.bss�Σ���ô���Թرղ���Ҫ������ */
//#define TX_DISABLE_REDUNDANT_CLEARING


/* ȷ���Ƿ���Ҫ��ʱ���飬��ֹ����Ҫ�û�ע�͵�tx_initialize_low_level�ļ�����tx_timer_interrupt�ĵ���?
   ���⣬��ֹ�󣬱���ʹ��TX_TIMER_PROCESS_IN_ISR */
/*
#define TX_NO_TIMER
#ifndef TX_TIMER_PROCESS_IN_ISR
#define TX_TIMER_PROCESS_IN_ISR
#endif
*/


/* ���������Ƿ�ر�֪ͨ�ص���Ĭ����ʹ�ܵġ����Ӧ�ó���û���õ���Ϣ�ص����رյ�����Լ�С���룬���ҿ�����������?*/
//#define TX_DISABLE_NOTIFY_CALLBACKS


/* ʹ��tx_thread_resume��tx_thread_suspendʹ���������룬����������������������ִ�����ܣ����������Ӵ����Y*/
//#define TX_INLINE_THREAD_RESUME_SUSPEND


/* ����TreadX�ں˲����жϣ��ô��ǽ��ʹ����������Ҳ����Ĵ���С����������ʱ�]*/
//#define TX_NOT_INTERRUPTABLE


/* ʹ���¼�Trace������΢���ӵ���m*/
//#define TX_ENABLE_EVENT_TRACE


/* ʹ��BLOCK_POOL��Ϣ��ȡ */
//#define TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO


/* ʹ��BYTE_POOL��Ϣ��ȡ */
//#define TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO


/* ʹ���¼���־��Ϣ��ȡ */
//#define TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO


/* ʹ�ܻ����ź�����Ϣ�� */
//#define TX_MUTEX_ENABLE_PERFORMANCE_INFO


/* ʹ����Ϣ������Ϣ��ȡ */
//#define TX_QUEUE_ENABLE_PERFORMANCE_INFO

/* ʹ���ź�����Ϣ�� */
//#define TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO


/* ʹ��������Ϣ��ȡ */
//#define TX_THREAD_ENABLE_PERFORMANCE_INFO


/* ʹ�ܶ�ʱ����Ϣ��*/
//#define TX_TIMER_ENABLE_PERFORMANCE_INFO



#endif

