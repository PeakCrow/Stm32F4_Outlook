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
  * @DateTime:     2023��7��3�� 23:37:11
  * @Purpose:      ��������ļ�
  * @param:        void               
  * @return:       void
*******************************************************************************/
int main(void)
{
	HAL_Init();                    	 			/* ��ʼ��HAL??*/   
	Stm32_Clock_Init(336,8,2,7);   				/* ����ʱ��,168Mhz */

	/* �ں˿���ǰ�ر�HAL��ʱ���??*/
	HAL_SuspendTick();
	
    /* ����ThreadX�ں� */
    tx_kernel_enter();
	while(1)
		;
}




