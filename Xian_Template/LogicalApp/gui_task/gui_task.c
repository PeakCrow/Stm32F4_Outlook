#include "gui_task.h"


TX_THREAD	AppTaskTFTLCDTCB;
uint64_t	AppTaskTFTLCDStk[APP_CFG_TASK_TFTLCD_STK_SIZE/8];
TX_MUTEX AppLCDSemp;				/* ����LVGL���� */

/*******************************************************************************
  * @FunctionName: AppTaskTFTLCD
  * @Author:       trx
  * @DateTime:     2022��6��23��21:03:00 
  * @Purpose:      lcd��Ļ��������
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
void AppTaskTFTLCD    (ULONG thread_input)
{
	(VOID)thread_input;
	uint8_t lcd_id[12];				/* ���LCD ID�ַ��� */
	App_Printf((char*)lcd_id,"LCD ID:%04X",lcddev.id);
	#if 1
	tx_mutex_get(&AppLCDSemp, TX_WAIT_FOREVER);
	Gui_Monitor_App();	/* ����lvgl���� */
	tx_mutex_put(&AppLCDSemp);
	while(1)
	{
		tx_mutex_get(&AppLCDSemp, TX_WAIT_FOREVER);
 		lv_timer_handler();
		//IWDG_Feed();
		tx_mutex_put(&AppLCDSemp);
		tx_thread_sleep(5);
	}
	
	#else
 	//Load_Drow_Dialog();
	if(tp_dev.touchtype&0X80)
		ctp_test();
	#endif
}



