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




/*******************************************************************************
  * @FunctionName: main
  * @Author:       trx
  * @DateTime:     2022�?�?5�?9�?8�?
  * @Purpose:      
  * @param:        void               
  * @return:
*******************************************************************************/
int main(void)
{
	HAL_Init();                    	 			/* 初始化HAL�?*/   
	Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */

#if	DEBUG_SWITCH_EN == 1
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始�?EventRecorder 并开�?*/
#endif

	/* 内核开启前关闭HAL的时间基�?*/
	HAL_SuspendTick();
	
    /* 进入ThreadX内核 */
    tx_kernel_enter();
	while(1)
		;
}




