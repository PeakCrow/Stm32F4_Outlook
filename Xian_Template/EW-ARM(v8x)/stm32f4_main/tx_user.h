/*
*********************************************************************************************************
*
*	ģ������ : ThreadX�ں������ļ�
*	�ļ����� : tx_user.h
*	��    �� : V1.0
*	˵    �� : �ں������ļ�
*
*	�޸ļ�¼ :
*		�汾��    ����        ����     ˵��
*		V1.0    2020-06-04  Eric2013   �״η���
*
*	Copyright (C), 2020-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __TX_USER_H__
#define  __TX_USER_H__

/*
*********************************************************************************************************
*                                           �궨��
*********************************************************************************************************
*/
/*   
   ����ٶ��Ż���Ҫ������ѡ�� :

        TX_MAX_PRIORITIES                       32
        TX_DISABLE_PREEMPTION_THRESHOLD
        TX_DISABLE_REDUNDANT_CLEARING
        TX_DISABLE_NOTIFY_CALLBACKS
        TX_NOT_INTERRUPTABLE
        TX_TIMER_PROCESS_IN_ISR
        TX_REACTIVATE_INLINE
        TX_DISABLE_STACK_FILLING
        TX_INLINE_THREAD_RESUME_SUSPEND
   
   ��С�����Ż���Ҫ������ѡ��:
   
        TX_MAX_PRIORITIES                       32
        TX_DISABLE_PREEMPTION_THRESHOLD
        TX_DISABLE_REDUNDANT_CLEARING
        TX_DISABLE_NOTIFY_CALLBACKS
        TX_NOT_INTERRUPTABLE
        TX_TIMER_PROCESS_IN_ISR
 */


/* ����tx_port.h ����ĺ궨��  */
/*
#define TX_MAX_PRIORITIES                       32
#define TX_MINIMUM_STACK                        ????
#define TX_THREAD_USER_EXTENSION                ????
#define TX_TIMER_THREAD_STACK_SIZE              ????
#define TX_TIMER_THREAD_PRIORITY                ????
*/

/* 
   ȷ����ʱ���Ƿ��ڵĴ�������Ӧ�ö�ʱ�������ʱ��ͺ���tx_thread_sleep���õȣ�����ϵͳ��ʱ���������滹���ڶ�ʱ���ж�������á�
   Ĭ�����ڶ�ʱ�������棬�����������溯���󣬽�ֱ���ڶ�ʱ���ж����洦������ȥ����ʱ��������������Դ�� */
//#define TX_TIMER_PROCESS_IN_ISR


/* �������ö�ʱ�������Ƿ����������ʽ��Ĭ�ϴ˹����ǹرյġ����ʹ�ܺ�������ʽ��ִ���ٶȿ죬�����Ӵ����� */
//#define TX_REACTIVATE_INLINE


/* ���������Ƿ�ر�ջ��䣬Ĭ���������ʹ�ܵģ����������ջ�ռ�ȫ�����Ϊ0xEF��
*  ����ThreadX���������������ʱջ�����õ���
*/
//#define TX_DISABLE_STACK_FILLING


/* ����ʹ��ջ��⣬Ĭ���ǹرյġ���ѡ��ʹ�ܺ󣬶�TX_DISABLE_STACK_FILLINGûʹ��ʱ��ջ��佫����������ջ��� */
//#define TX_ENABLE_STACK_CHECKING


/* ���������Ƿ�ر���ռ��ֵ��Ĭ���ǿ����ġ����Ӧ�ó�����Ҫ�˹��ܣ��رպ���Խ��ʹ��������������� */
//#define TX_DISABLE_PREEMPTION_THRESHOLD


/* ���������Ƿ�����ThreadXȫ�ֱ������������������������ThreadX����ǰ�����.bss�Σ���ô���Թرղ���Ҫ������ */
//#define TX_DISABLE_REDUNDANT_CLEARING


/* ȷ���Ƿ���Ҫ��ʱ���飬��ֹ����Ҫ�û�ע�͵�tx_initialize_low_level�ļ�����tx_timer_interrupt�ĵ��á�
   ���⣬��ֹ�󣬱���ʹ��TX_TIMER_PROCESS_IN_ISR */
/*
#define TX_NO_TIMER
#ifndef TX_TIMER_PROCESS_IN_ISR
#define TX_TIMER_PROCESS_IN_ISR
#endif
*/


/* ���������Ƿ�ر�֪ͨ�ص���Ĭ����ʹ�ܵġ����Ӧ�ó���û���õ���Ϣ�ص����رյ�����Լ�С���룬���ҿ����������ܡ� */
//#define TX_DISABLE_NOTIFY_CALLBACKS


/* ʹ��tx_thread_resume��tx_thread_suspendʹ���������룬����������������������ִ�����ܣ����������Ӵ����� */
//#define TX_INLINE_THREAD_RESUME_SUSPEND


/* ����TreadX�ں˲����жϣ��ô��ǽ��ʹ����������Ҳ����Ĵ���С����������ʱ�� */
//#define TX_NOT_INTERRUPTABLE


/* ʹ���¼�Trace������΢���ӵ���� */
//#define TX_ENABLE_EVENT_TRACE


/* ʹ��BLOCK_POOL��Ϣ��ȡ */
//#define TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO


/* ʹ��BYTE_POOL��Ϣ��ȡ */
//#define TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO


/* ʹ���¼���־��Ϣ��ȡ */
//#define TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO


/* ʹ�ܻ����ź�����Ϣ��ȡ  */
//#define TX_MUTEX_ENABLE_PERFORMANCE_INFO


/* ʹ����Ϣ������Ϣ��ȡ */
//#define TX_QUEUE_ENABLE_PERFORMANCE_INFO

/* ʹ���ź�����Ϣ��ȡ  */
//#define TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO


/* ʹ��������Ϣ��ȡ */
//#define TX_THREAD_ENABLE_PERFORMANCE_INFO


/* ʹ�ܶ�ʱ����Ϣ��ȡ */
//#define TX_TIMER_ENABLE_PERFORMANCE_INFO

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
