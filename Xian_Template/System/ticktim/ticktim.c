#include "ticktim.h"
/*全局运行时间，单位1ms,最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题*/
__IO int32_t g_iRunTime = 0;
/*定义于软件定时器结构体变量*/
static SOFT_TIM s_tTmr[TIM_TASK_COUNT] = {0};
/*等待变量初始化*/
static __IO uint8_t g_ucEnableSystickISR = 0;
/*滴答定时器定时减小定时器任务的周期数值*/
static void bsp_SoftTimerDec(SOFT_TIM *_tim);
/*滴答定时器中断服务程序*/
void SysTick_ISR(void);

/************************************************
*Function name	:bsp_InitTimer
*Description	:配置systick中断，并初始化软件定时器变量;一定要初始化，否则会无法正常调用延时函数
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日17点14分
*************************************************/
void bsp_InitTimer(void)
{
	uint8_t i;
	//清零所有的软件定时器
	for(i = 0;i < TIM_TASK_COUNT;i++){	
		s_tTmr[i].State = 0;		
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TIM_ONCE_MODE; //默认是一次性工作模式
		s_tTmr[i].callfunc = 0;
	}
	/*
		配置systic中断周期为1ms，并启动systick中断。

    	SystemCoreClock 是固件中定义的系统内核时钟，对于STM32H7,一般为 400MHz

    	SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
	    	-- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
	    	-- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
	    	-- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us

    	对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms
    */
#if USE_THREADX	 == 0
	SysTick_Config(SystemCoreClock / 1000);
#endif
	g_ucEnableSystickISR = 1;	//1表示执行systick中断
	
}
/************************************************
*Function name	:bsp_StartOnceTimer
*Description	:创建一个一次性的定时器
*Input			:_id:定时器任务id	_period:定时器任务周期,单位:ms
				:callfunc:功能函数
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日19点39分
*************************************************/
void bsp_StartOnceTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
	if(_id >= TIM_TASK_COUNT){
		/*打印出错的源代码文件名，函数名称*/
		printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
		while(1);	/*参数异常，死机等待看门狗复位*/
	}
	__set_PRIMASK(1);	/*禁止全局中断*/
	s_tTmr[_id].State = 0;
	s_tTmr[_id].Count = _period;		/*实时计数器初值*/
	s_tTmr[_id].PreLoad = _period;		/*计数器自动重装载值，单次模式不起作用*/
	s_tTmr[_id].Flag = 0;				/*定时间到标志*/
	s_tTmr[_id].Mode = TIM_ONCE_MODE;	/*一次性工作模式*/
	s_tTmr[_id].callfunc = callfunc;	/*调用回调函数*/
	__set_PRIMASK(0);	/*开启全局中断*/
}
/************************************************
*Function name	:bsp_StartAutoTimer
*Description	:创建一个不停定时的定时器
*Input			:_id:定时器任务id	_period:定时器任务周期,单位:ms
				:callfunc:功能函数
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日19点52分
*************************************************/
void bsp_StartAutoTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
	if(_id >= TIM_TASK_COUNT){
		/*打印出错的源代码文件名，函数名称*/
		printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
		while(1);/*参数异常，死机等待看门狗复位*/
	}
	__set_PRIMASK(1);	/*禁止全局中断*/
	s_tTmr[_id].State = 1;
	s_tTmr[_id].Count = _period;		/*实时计数器初值*/
	s_tTmr[_id].PreLoad = _period;		/*计数器自动重装载值，仅自动模式起作用*/
	s_tTmr[_id].Flag = 0;				/*定时时间到标志*/
	s_tTmr[_id].Mode = TIM_MULTI_MODE;	/*自动重复调用模式*/
	s_tTmr[_id].callfunc = callfunc;	/*调用回调函数*/
	__set_PRIMASK(0);	/*开中断*/
		
}
/************************************************
*Function name	:bsp_StopTimer
*Description	:停止一个定时器，如果需要重新调用，必须再次创建
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点18分
*************************************************/
void bsp_StopTimer(uint8_t _id)
{
	s_tTmr[_id].callfunc = 0;
}
/************************************************
*Function name	:bsp_SoftTimerDec
*Description	:每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*Input			:_tim:定时器任务的id数组
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点07分
*************************************************/
static void bsp_SoftTimerDec(SOFT_TIM *_tim)
{
	if(_tim->Count > 0){
		/*如果定时器变量减到1则设置定时器到达标志*/
		if(--_tim->Count == 0){
			_tim->Flag = 1;
			/*如果是自动模式，则自动重装载值*/
			if(_tim->Mode == TIM_MULTI_MODE){
				_tim->Count = _tim->PreLoad;
			}
		}
	}
}
/************************************************
*Function name	:SysTick_ISR
*Description	:SysTick中断服务程序，每隔1ms进入1次
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点08分
*************************************************/
void SysTick_ISR(void)
{
	static uint8_t s_count10ms = 0,s_count50ms = 0;
	uint8_t i;
	
	/*每隔1ms，对所有的软件定时器进行减小操作*/
	for(i = 0;i < TIM_TASK_COUNT;i++){
		bsp_SoftTimerDec(&s_tTmr[i]);	/*定时器任务减小1*/
		if((s_tTmr[i].Flag == 1) && (s_tTmr[i].callfunc)){
			s_tTmr[i].callfunc();		//调用功能函数
			s_tTmr[i].Flag = 0;
			if(s_tTmr[i].State == 0){	//如果是一次性函数
				s_tTmr[i].callfunc = 0;	//清空回调函数参数
			}
		}
	}	
	/*全局运行时间每1ms加1*/
	g_iRunTime++;
	if(g_iRunTime == 0x7FFFFFFF){	/*uint32_t类型的最大值*/
		g_iRunTime = 0;
	}
	bsp_RunPer1ms();		/*每隔1ms调用一次此函数*/
	if(++s_count10ms >= 10){
		s_count10ms = 0;
		bsp_RunPer10ms();	/*每隔10ms调用一次此函数*/
	}
	if(++s_count50ms >= 50){
		s_count50ms = 0;
		bsp_RunPer50ms();	/*每隔50ms调用一次此函数*/
	}
}

/************************************************
*Function name	:bsp_RunPer1ms
*Description	:1ms周期调用函数，user可以重写
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点09分
*************************************************/
__weak void bsp_RunPer1ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}
/************************************************
*Function name	:bsp_RunPer1ms
*Description	:10ms周期调用函数，user可以重写
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点09分
*************************************************/
__weak void bsp_RunPer10ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}
/************************************************
*Function name	:bsp_RunPer1ms
*Description	:50ms周期调用函数，user可以重写
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点22分
*************************************************/
__weak void bsp_RunPer50ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}

#if USE_THREADX == 0
/************************************************
*Function name	:bsp_DelayUs
*Description	:us级延迟。 必须在systick定时器启动后才能调用此函数。
*Input			:n:延时多少微秒
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点26分
*************************************************/
void bsp_DelayUs(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK是一个递减的计数器 */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }	
}
/************************************************
*Function name	:bsp_DelayUs
*Description	:ms级延迟。 必须在systick定时器启动后才能调用此函数。
*Input			:n:延时多少毫秒
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022年4月5日22点26分
*************************************************/
void bsp_DelayMs(uint32_t n)
{
	uint32_t i;
	for(i = 0;i < n;i++){
		bsp_DelayUs(1000);
	}
}

/**
  * @brief  This function handles SysTick Handler.底层中断函数
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	if(g_ucEnableSystickISR == 0){	/*代表没有进行多定时器任务初始化*/
		return;
	}
	SysTick_ISR();					/*需要在中断中不停的调用的中断服务程序*/	
}

#endif

int32_t bsp_GetRunTime(void)
{
	int32_t runtime;

	__set_PRIMASK(1);	/* 鍏抽棴鍏ㄥ眬涓柇 */
	runtime = g_iRunTime;
	__set_PRIMASK(0);	/* 鎵撳紑鍏ㄥ眬涓柇 */
	return runtime;
}


