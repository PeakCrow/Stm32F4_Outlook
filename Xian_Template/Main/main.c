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
  * @DateTime:     2023年7月3日 23:37:11
  * @Purpose:      程序入口文件
  * @param:        void               
  * @return:       void
*******************************************************************************/
int main(void)
{
	HAL_Init();                    	 			/* 初始化HAL??*/   
	Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */

	/* 内核开启前关闭HAL的时间基??*/
	HAL_SuspendTick();
	
    /* 进入ThreadX内核 */
    tx_kernel_enter();
	while(1)
		;
}




