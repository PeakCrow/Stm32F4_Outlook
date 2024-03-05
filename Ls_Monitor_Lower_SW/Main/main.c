/*********************************************
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
*********************************************/
#include "sys.h"
/*
 * @FunctionName : main
 * @DateTime     : 2024年2月25日 02:57:35
 * @Purpose      : 主函数程序入口
 * @param        : void
 * @return       : void
*/
int main(void)
{
    HAL_Init();                         /* HAL库初始化 */
    Stm32_Clock_Init(336,8,2,7);        /* system tick,168Mhz */

    /* 关闭HAL库 systick */
    HAL_SuspendTick();
    
    /* 进入Thread X OS */
    tx_kernel_enter();
    while(1);
}




