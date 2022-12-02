#ifndef __TICK_TIM_H
#define __TICK_TIM_H
#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "sys.h"
/*
	在此定义若干个软件定时器全局变量
	注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TIM_TASK_COUNT 4  //软件定时器的个数，(定时器ID范围0-3)

/*定时器结构体，成员变量必须是volatile,否则C编译器优化时可能有问题*/
typedef enum{
	TIM_ONCE_MODE = 0,	/*一次工作模式*/
	TIM_MULTI_MODE = 1	/*重复定时工作模式*/
}TIM_MODE_E;
/*定时器结构体，成员变量必须是volatile,否则C编译器优化时可能有问题*/
typedef struct{
	volatile uint8_t State;			//0-一次工作模式；1-重复调用
	volatile uint8_t Mode;			//计数器模式，一次性
	volatile uint8_t Flag;			//定时到达标志
	volatile uint32_t Count;		//计数器
	volatile uint32_t PreLoad;		//计数器重装载值
	void (*callfunc) (void);		//功能函数
}SOFT_TIM;

/*提供给其他C文件调用的函数*/
/*多定时器复用函数*/
void bsp_InitTimer(void);
void bsp_StartOnceTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void));
void bsp_StartAutoTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void));
void bsp_StopTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

/*周期调用函数，不允许放在while(1)中*/
void bsp_RunPer1ms(void);
void bsp_RunPer10ms(void);
void bsp_RunPer50ms(void);
/*延时函数，适合放在while(1)中*/
void bsp_DelayUs(uint32_t n);
void bsp_DelayMs(uint32_t n);
#endif

