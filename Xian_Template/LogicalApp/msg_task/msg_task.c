#include "msg_task.h"

TX_THREAD   AppTaskUserIFTCB;
uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];
TX_THREAD   AppTaskMsgProTCB;
uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];
TX_MUTEX 	AppPrintfSemp;				/* ����printf���� */
TX_MUTEX 	App_PowerDownSave;			/* ���ڵ��籣�� */ 


void AppTaskMsgPro(ULONG thread_input)
{
	(void)thread_input;
	ULONG actual_events;
	UINT status;
	while(1)
	{				
		status = tx_event_flags_get(&EventGroup,     /* �¼���־���ƿ� */
									DRIVER_POS_ALL,  /* �ȴ���־ */
									TX_OR_CLEAR ,    /* �ȴ�����bit���㼴�� */
									&actual_events,  /* ��ȡʵ��ֵ */
									TX_WAIT_FOREVER);/* ���õȴ� */		
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
*	�� �� ��: AppTaskUserIF
*	����˵��: ������Ϣ����
*	��    �� : thread_input ����������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 4
*********************************************************************************************************
*/
void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* �������� */
	(void)thread_input;
    App_Printf("k0��������\r\n");
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode != KEY_NONE)
		{
				switch(ucKeyCode)
				{
					case KEY_0_UP: 			  /* K1������ӡ����ִ����� */
						App_Printf("k0��������\r\n");
					 	break;
					case KEY_0_DOWN:			/* k0�������� */
						App_Printf("k0��������\r\n");
						DispTaskInfo();
						break;
					case KEY_UP_UP:
						App_Printf("kup��������\r\n");
						break;
					case KEY_UP_DOWN:			/* kup�������� */
						App_Printf("kup��������\r\n");
						break;                    
					case KEY_0_LONG:
						App_Printf("k0��������\r\n");
						break;
					case KEY_UP_LONG:
						App_Printf("kup��������\r\n");
						break;
					case KEY_MULTI_DOWM:
						App_Printf("kmulti��������\r\n");
						break;
					case KEY_MULTI_UP:
						App_Printf("kmulti��������\r\n");
						break;
					case KEY_MULTI_LONG:
						App_Printf("kmulti��������\r\n");
						break;
				}
		}
        tx_thread_sleep(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    �� : ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1];/* �ر�ע�⣬���printf�ı����϶࣬ע��˾ֲ������Ĵ�С�Ƿ��� */
    va_list   v_args;
    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);
	/* ������� */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);
    printf("%s", buf_str);
    tx_mutex_put(&AppPrintfSemp);
}
/*
*********************************************************************************************************
*	�� �� ��: App_I2C_EE_BufferWrite
*	����˵��: �̰߳�ȫ��eepromд�˷�ʽ
*	��    �� : ͬI2C_EE_BufferWrite�Ĳ�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void    App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
	/* ������� */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);
    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);
    tx_mutex_put(&App_PowerDownSave);	
}



