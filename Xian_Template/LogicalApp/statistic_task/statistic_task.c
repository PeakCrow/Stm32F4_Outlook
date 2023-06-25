#include "statistic_task.h"

TX_THREAD   AppTaskStartTCB;
TX_THREAD   AppTaskIdleTCB;
uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];
TX_THREAD   AppTaskStatTCB;
uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];


__IO float     OSCPUUsage;   	    /* CPU�ٷֱ� */
__IO uint8_t   OSStatRdy;      		/* ͳ�����������־ */
__IO uint32_t  OSIdleCtr;     	    /* ����������� */
uint32_t       OSIdleCtrMax; 	    /* 1�������Ŀ��м��� */
uint32_t       OSIdleCtrRun; 	    /* 1���ڿ�������ǰ���� */
/*
*********************************************************************************************************
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-III������Ϣͨ�����ڴ�ӡ����
*	��    �� ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* ����һ��������ƿ�ָ�� */

    p_tcb = &AppTaskStartTCB;
	
	/* ��ӡ���� */
	App_Printf("===============================================================\r\n");
	App_Printf("OS CPU Usage = %5.2f%%\r\n", OSCPUUsage);
	App_Printf("===============================================================\r\n");
	App_Printf(" �������ȼ� ����ջ��С ��ǰʹ��ջ  ���ջʹ��   ������\r\n");
	App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* ����������ƿ���?TCB list)����ӡ���е���������ȼ�����?*/
	while (p_tcb != (TX_THREAD *)0) 
	{
		App_Printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                    p_tcb->tx_thread_name);


        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStatistic
*	����˵��: ͳ����������ʵ��CPU�����ʵ�ͳ�ơ�Ϊ�˲��Ը���׼ȷ�����Կ���ע�͵��õ�ȫ���жϿ���
*	��    �� : thread_input ����������ʱ���ݵ��β� 
*	�� �� ֵ: ��
*   �� �� ��: 30
*********************************************************************************************************
*/
void AppTaskStat(ULONG thread_input)
{
	(void)thread_input;

    while (OSStatRdy == FALSE) 
	{
        tx_thread_sleep(200);     /* �ȴ�ͳ��������� */
    }

    OSIdleCtrMax /= 100uL;
    if (OSIdleCtrMax == 0uL) 
	{
        OSCPUUsage = 0u;
    }
	
    __disable_irq();
    OSIdleCtr = OSIdleCtrMax * 100uL;  /* ���ó�ʼCPU������ 0% */
	__enable_irq();
	
    for (;;) 
	{
        __disable_irq();
        OSIdleCtrRun = OSIdleCtr;    /* ���100ms�ڿ��м��� */
        OSIdleCtr    = 0uL;          /* ��λ���м��� */
        __enable_irq();            /* ����100ms�ڵ�CPU������ */
        OSCPUUsage   = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
        tx_thread_sleep(100);        /* ÿ100msͳ��һ�� */
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskIDLE
*	����˵��: ��������
*	��    �� : thread_input ����������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 31
*********************************************************************************************************
*/
void AppTaskIDLE(ULONG thread_input)
{	
  TX_INTERRUPT_SAVE_AREA

  (void)thread_input;
	
	while(1)
	{
	   TX_DISABLE
	   OSIdleCtr++;
	   TX_RESTORE
	}
}
/*******************************************************************************
  * @FunctionName: OSStatInit
  * @Author:       trx
  * @DateTime:     2022��5��24��23:52:39 
  * @Purpose:      ����Ĳ�������
  * @param:        void               
  * @return:       none
*******************************************************************************/
void  OSStatInit (void)
{
	OSStatRdy = FALSE;
	
    tx_thread_sleep(2u);        /* ʱ��ͬ�� */
	
    __disable_irq();
    OSIdleCtr    = 0uL;         /* ����м��� */
	__enable_irq();
	
    tx_thread_sleep(100);       /* ͳ��100ms�ڣ������м��� */
	
   	__disable_irq();
    OSIdleCtrMax = OSIdleCtr;   /* ���������м��� */
    OSStatRdy    = TRUE;
	__enable_irq();
}


