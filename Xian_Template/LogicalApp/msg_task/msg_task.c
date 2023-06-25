#include "msg_task.h"

TX_THREAD   AppTaskUserIFTCB;
uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];
TX_THREAD   AppTaskMsgProTCB;
uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];
TX_MUTEX 	AppPrintfSemp;				/* 用于printf互斥 */
TX_MUTEX 	App_PowerDownSave;			/* 用于掉电保存 */ 


void AppTaskMsgPro(ULONG thread_input)
{
	(void)thread_input;
	ULONG actual_events;
	UINT status;
	while(1)
	{				
		status = tx_event_flags_get(&EventGroup,     /* 事件标志控制块 */
									DRIVER_POS_ALL,  /* 等待标志 */
									TX_OR_CLEAR ,    /* 等待任意bit满足即可 */
									&actual_events,  /* 获取实际值 */
									TX_WAIT_FOREVER);/* 永久等待 */		
		if(status == TX_SUCCESS)
		{
			switch (actual_events)
			{
				case DRIVER1_POS:			  
					/* */
				
					break;
				
				case DRIVER2_POS:			  
					/* */

					break;
				
				case DRIVER3_POS:			 
					/*  */

					break;
				
				default:       
					break;
			}		
		}
		tx_thread_sleep(200);
	}   
}
/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理
*	形    参 : thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 4
*********************************************************************************************************
*/
void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* 按键代码 */
	(void)thread_input;
    App_Printf("k0按键弹起\r\n");
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode != KEY_NONE)
		{
				switch(ucKeyCode)
				{
					case KEY_0_UP: 			  /* K1键按打印任务执行情况 */
						App_Printf("k0按键弹起\r\n");
					 	break;
					case KEY_0_DOWN:			/* k0按键按下 */
						App_Printf("k0按键按下\r\n");
						DispTaskInfo();
						break;
					case KEY_UP_UP:
						App_Printf("kup按键弹起\r\n");
						break;
					case KEY_UP_DOWN:			/* kup按键按下 */
						App_Printf("kup按键按下\r\n");
						break;                    
					case KEY_0_LONG:
						App_Printf("k0按键长按\r\n");
						break;
					case KEY_UP_LONG:
						App_Printf("kup按键长按\r\n");
						break;
					case KEY_MULTI_DOWM:
						App_Printf("kmulti按键按下\r\n");
						break;
					case KEY_MULTI_UP:
						App_Printf("kmulti按键弹起\r\n");
						break;
					case KEY_MULTI_LONG:
						App_Printf("kmulti按键长按\r\n");
						break;
				}
		}
        tx_thread_sleep(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参 : 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1];/* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list   v_args;
    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);
	/* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);
    printf("%s", buf_str);
    tx_mutex_put(&AppPrintfSemp);
}
/*
*********************************************************************************************************
*	函 数 名: App_I2C_EE_BufferWrite
*	功能说明: 线程安全的eeprom写人方式
*	形    参 : 同I2C_EE_BufferWrite的参数。
*	返 回 值: 无
*********************************************************************************************************
*/
void    App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
	/* 互斥操作 */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);
    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);
    tx_mutex_put(&App_PowerDownSave);	
}



