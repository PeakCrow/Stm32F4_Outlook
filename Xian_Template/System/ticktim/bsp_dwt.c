/*
*********************************************************************************************************
*
*	模块名称 : 数据观察点与跟踪(DWT)模块
*	文件名称 : bsp_dwt.c
*	�?   �?: V1.0
*	�?   �?: 在CM3，CM4，CM7中可以有3种跟踪源：ETM, ITM 和DWT，本驱动主要实现
*              DWT中的时钟周期（CYCCNT）计数功能，此功能非常重要，可以很方便的
*              计算程序执行的时钟周期个数�?
*	修改记录 :
*		版本�?   日期        作�?    说明
*		V1.0    2019-02-24   Eric2013 正式发布
*
*	Copyright (C), 2019-2030, 安富莱电�?www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp_dwt.h"



/*
*********************************************************************************************************
*                                             寄存�?
*********************************************************************************************************
*/
#define  DEM_CR_TRCENA               (1 << 24)
#define  DWT_CR_CYCCNTENA            (1 <<  0)


/*
*********************************************************************************************************
*	�?�?�? bsp_InitDWT
*	功能说明: 初始化DWT. 该函数被 bsp_Init() 调用�?
*	�?   �? �?
*	�?�?�? �?
*********************************************************************************************************
*/
void bsp_InitDWT(void)
{
	DEM_CR         |= (unsigned int)DEM_CR_TRCENA;
	DWT_CYCCNT      = (unsigned int)0u;
	DWT_CR         |= (unsigned int)DWT_CR_CYCCNTENA;
}

#if 1  
/*
*********************************************************************************************************
*	�?�?�? bsp_DelayMS
*	功能说明: 为了让底层驱动在带RTOS和裸机情况下有更好的兼容�?
*             专门制作一个阻塞式的延迟函数，在底层驱动中ms毫秒延迟主要用于初始化，并不会影响实时性�?
*	�?   �? n 延迟长度，单�? ms
*	�?�?�? �?
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t _ulDelayTime)
{
	bsp_DelayUS(1000*_ulDelayTime);
}

/*
*********************************************************************************************************
*	�?�?�? bsp_DelayUS
*	功能说明: 这里的延时采用CPU的内部计数实现，32位计数器
*             	OSSchedLock(&err);
*				bsp_DelayUS(5);
*				OSSchedUnlock(&err); 根据实际情况看看是否需要加调度锁或选择关中�?
*	�?   �? _ulDelayTime  延迟长度，单�? us
*	�?�?�? �?
*   �?   �? 1. 主频168MHz的情况下�?2位计数器计满�?^32/168000000 = 25.565�?
*                建议使用本函数做延迟的话，延迟在1秒以下�? 
*             2. 实际通过示波器测试，微妙延迟函数比实际设置实际多运行0.25us左右的时间�?
*             下面数据测试条件�?
*             �?�? MDK5.15，优化等�?, 不同的MDK优化等级对其没有影响�?
*             �?�? STM32F407IGT6
*             �?�? 测试方法�?
*				 GPIOI->BSRRL = GPIO_Pin_8;
*				 bsp_DelayUS(10);
*				 GPIOI->BSRRH = GPIO_Pin_8;
*             -------------------------------------------
*                测试                 实际执行
*             bsp_DelayUS(1)          1.2360us
*             bsp_DelayUS(2)          2.256us
*             bsp_DelayUS(3)          3.256us
*             bsp_DelayUS(4)          4.256us
*             bsp_DelayUS(5)          5.276us
*             bsp_DelayUS(6)          6.276us
*             bsp_DelayUS(7)          7.276us
*             bsp_DelayUS(8)          8.276us
*             bsp_DelayUS(9)          9.276us
*             bsp_DelayUS(10)         10.28us
*            3. 两个32位无符号数相减，获取的结果再赋值给32位无符号数依然可以正确的获取差值�?
*              假如A,B,C都是32位无符号数�?
*              如果A > B  那么A - B = C，这个很好理解，完全没有问题
*              如果A < B  那么A - B = C�?C的数值就�?xFFFFFFFF - B + A + 1。这一点要特别注意，正好用于本函数�?
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		
	tStart = DWT_CYCCNT;                                     /* 刚进入时的计数器�?*/
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);	 /* 需要的节拍�?*/ 		      

	while(tCnt < tDelayCnt)
	{
		tCnt = DWT_CYCCNT - tStart; /* 求减过程中，如果发生第一�?2位计数器重新计数，依然可以正确计�?*/	
	}
}

void bsp_DelayDWT(uint32_t _ulDelayTime)
{
    uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		
	tCnt = 0;
	tDelayCnt = _ulDelayTime;	 /* 需要的节拍�?*/ 		      
	tStart = DWT_CYCCNT;         /* 刚进入时的计数器�?*/
	
	while(tCnt < tDelayCnt)
	{
		tCnt = DWT_CYCCNT - tStart; /* 求减过程中，如果发生第一�?2位计数器重新计数，依然可以正确计�?*/	
	}
}

/*
*********************************************************************************************************
*	�?�?�? HAL_Delay
*	功能说明: 重定向毫秒延迟函数。替换HAL中的函数。因为HAL中的缺省函数依赖于Systick中断，如果在USB、SD�?
*             中断中有延迟函数，则会锁死。也可以通过函数HAL_NVIC_SetPriority提升Systick中断
*	�?   �? �?
*	�?�?�? �?
*********************************************************************************************************
*/
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayMS(Delay);
}

HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority)
{
        return HAL_OK;
}

uint32_t HAL_GetTick (void)
{
        static uint32_t ticks = 0U;
        uint32_t i;

        if (_tx_thread_system_state == TX_INITIALIZE_IS_FINISHED)
        {
                return ((uint32_t)_tx_time_get());
        }

        /* 如果ThreadX还没有运行，采用下面方式 */
        for (i = (SystemCoreClock >> 14U); i > 0U; i--)
        {
                __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
                __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        }
       
        return ++ticks;
}

#endif

/***************************** 安富莱电�?www.armfly.com (END OF FILE) *********************************/
