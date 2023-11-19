#include "system_task_create.h"
/*
*********************************************************************************************************
*                                    ����ջ��С����λ�ֽ�
*********************************************************************************************************
*/
#define  APP_CFG_TASK_COM_PRIO           7u
#define  APP_CFG_TASK_COM_STK_SIZE       1024u
#define  TASK_START_CREATE_PRIO         		 7u
#define  TASK_START_CREATE_SIZE     			 1024u
/*
*********************************************************************************************************
*                                       ��̬ȫ�ֱ���
*********************************************************************************************************
*/
static  TX_THREAD   AppTaskCOMTCB;
static  uint64_t    AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE/8];
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[TASK_START_CREATE_SIZE/8];
/*
*********************************************************************************************************
*                                       �����ʱ������
*********************************************************************************************************
*/
TX_TIMER AppTimer; 
TX_EVENT_FLAGS_GROUP  EventGroup; 	/* �¼���־�� */
/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskCOM			(ULONG thread_input);
static  void  AppTaskCreate         (void);
static  void  AppSysObjCreate       (void);
        void  TimerCallback         (ULONG thread_input);
/*
*********************************************************************************************************
*	�� �� ��: tx_application_define
*	����˵��: ThreadXר�õ����񴴽���ͨ����������������˺�������tx_kernel_enter�����б�����
*	��    �� : first_unused_memory  δʹ�õĵ�ַ�ռ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  tx_application_define(void *first_unused_memory)
{
	
	/*
	   ���ʵ������CPU������ͳ�ƵĻ����˺���������ʵ����������ͳ������Ϳ����������������ں���
	   AppTaskCreate���洴����
	*/
	/**************������������*********************/
    tx_thread_create(&AppTaskStartTCB,              	/* ������ƿ��ַ */   
                        "App Task Start",            	/* ������ */
                        AppTaskStart,              		/* ������������ַ */
                        0,                      		/* ���ݸ�����Ĳ��� */
                        &AppTaskStartStk[0],    		/* ��ջ����ַ */
    					TASK_START_CREATE_SIZE,        		/* ��ջ�ռ��С */
    					TASK_START_CREATE_PRIO,        		/* �������ȼ�*/
    					TASK_START_CREATE_PRIO,        		/* ������ռ��ֵ */
    					TX_NO_TIME_SLICE,       		/* ������ʱ��Ƭ */
    					TX_AUTO_START);          		/* �������������� */
	/**************����ͳ������*********************/
    tx_thread_create(&AppTaskStatTCB,"App Task STAT",AppTaskStat,0,&AppTaskStatStk[0],APP_CFG_TASK_STAT_STK_SIZE,
                       APP_CFG_TASK_STAT_PRIO,APP_CFG_TASK_STAT_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************������������*********************/
    tx_thread_create(&AppTaskIdleTCB,"App Task IDLE",AppTaskIDLE,0,&AppTaskIdleStk[0],APP_CFG_TASK_IDLE_STK_SIZE,
                       APP_CFG_TASK_IDLE_PRIO,APP_CFG_TASK_IDLE_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ�����񣬴˺�����AppTaskStart�����б�����
*	��    �� : ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************����USER IF����*********************/
    tx_thread_create(&AppTaskUserIFTCB,"App Task UserIF",AppTaskUserIF,0,&AppTaskUserIFStk[0],APP_CFG_TASK_USER_IF_STK_SIZE,
    				   APP_CFG_TASK_USER_IF_PRIO,APP_CFG_TASK_USER_IF_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************����MsgPro����*********************/
    tx_thread_create(&AppTaskMsgProTCB,"App Msp Pro",AppTaskMsgPro,0,&AppTaskMsgProStk[0],APP_CFG_TASK_MsgPro_STK_SIZE,
                       APP_CFG_TASK_MsgPro_PRIO,APP_CFG_TASK_MsgPro_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************����COM����*********************/
    tx_thread_create(&AppTaskCOMTCB,"App Task COM",AppTaskCOM,0,&AppTaskCOMStk[0],APP_CFG_TASK_COM_STK_SIZE,
                       APP_CFG_TASK_COM_PRIO,APP_CFG_TASK_COM_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
	/**************����LCD DISP����*********************/
    tx_thread_create(&AppTaskTFTLCDTCB,"App Task TFTLCD",AppTaskTFTLCD,0,&AppTaskTFTLCDStk[0],APP_CFG_TASK_TFTLCD_STK_SIZE,
                       APP_CFG_TASK_TFTLCD_PRIO,APP_CFG_TASK_TFTLCD_PRIO,TX_NO_TIME_SLICE,TX_AUTO_START);
}

//static int test_master(void)
//{
//	setNodeId(&TestSlave_Data_Data,0x00);
//	setState(&TestSlave_Data_Data,Initialisation);
//	setState(&TestSlave_Data_Data,Operational);
//	return 0;
//}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: �����������������а���������������
*	��    ��: thread_input ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (ULONG thread_input)
{
	(void)thread_input;
	/* ����ִ������ͳ�� */
	OSStatInit();
	/* �ں˿����󣬻ָ�HAL���ʱ���׼ */
    HAL_ResumeTick();
    /* �����ʼ�� */
	bsp_InitDWT();								/* ��ʼ��DWT */
	bsp_InitTimer();							/* ��ʼ���δ�ʱ�� */
	bsp_InitUart();								/* ��ʼ������1 2 3���� */
	bsp_InitKey();								/* ��ʼ���ᴥ���� */
	bsp_InitSPI2Bus();							/* ��ʼ��SPI2���ߣ���������īˮ�� */
	bsp_I2C_EE_Init();							/* ��ʼ��IIC���ߣ���������eepromоƬ */
	bsp_InitLed();								/* ��ʼ������LED�� */
	bsp_InitCan1Bus();							/* ��ʼ��CAN1 ���� */
	bsp_InitRotationSensor();					/* ��ʼ�����ٴ����� */
	bsp_InitWs2812b();							/* ��ʼ��ws2812b�ɵ���Ч */
	bsp_SetTIMOutPWM(GPIOB,GPIO_PIN_6,TIM4,1,500,5000);/* ����һ��1k��50ռ�ձȵķ�����������֤������� */
	bsp_InitSPI1Bus();							/* SPI1���߳�ʼ�� */
	bsp_InitSFlash();							/* ��ʼ��SPI FLASHоƬ */
    bsp_InitSram();                             /* �ⲿsram��ʼ�� */
	lv_init(); 									/* lvgl ϵͳ��ʼ�� */
	lv_port_disp_init(); 						/* lvgl ��ʾ�ӿڳ�ʼ��,���� lv_init()�ĺ��� */
	lv_port_indev_init(); 						/* lvgl ����ӿڳ�ʼ��,���� lv_init()�ĺ��� */
	//test_master();
	/* ���������ͨ�Ż���,��Ҫ�Ǹ��������ͨѶ���� */
	AppSysObjCreate();
	/* �������񣬴˺����а���������-�߳����� */
    AppTaskCreate();
	while (1)
		{
//			tx_trace_enable(&myBuf,TRC_BUF_SIZE,TRC_MAX_OBJ_COUNT);
			/* ��Ҫ�����Դ���ĳ��򣬶�Ӧ������̵��õ�SysTick_ISR */
			bsp_ProPer1ms();
			tx_thread_sleep(1);
		}
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCom
*	����˵��: ���������ڴ�ӡ
*	��    �� : thread_input ����������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 5
*********************************************************************************************************
*/
static void AppTaskCOM(ULONG thread_input)
{
	(void)thread_input;
	App_Printf("AppTaskCom����ʼ����\r\n");
	while(1)
	{
        bsp_LedToggle(2);
		bsp_LedToggle(1);
		tx_thread_sleep(100);
	}
}
/*******************************************************************************
  * @FunctionName: TimerCallback
  * @Author:       trx
  * @DateTime:     2022��6��23��21:04:39 
  * @Purpose:      �������ݶ�ȡ�������ʱ���ص�����
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
void TimerCallback(ULONG thread_input)
{
	/* ���ӳٲ����������ô���0������Ҫ�ڶ�ʱ��Ļص������������ */
	//App_Printf("%.1fm/s\r\n",Rotation_Sensor_Get(24,0.464));/* 1000HZ������Ӧ��121.41m/s */
}
/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨѶ
*	��    �� : ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppSysObjCreate (void)
{
	/* ���������ź��� */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
	tx_mutex_create(&AppLCDSemp,"AppLCDSemp",TX_NO_INHERIT);	
		
	/* ��ʱ���� */
	tx_timer_create(&AppTimer,
					"App Timer",
					TimerCallback, 
					0,                  /* ���ݵĲ��� */
					500,                /* ���ö�ʱ��ʱ������ĳ�ʼ�ӳ٣���λThreadXϵͳʱ������� */
					1, 				    /* ���ó�ʼ�ӳٺ�Ķ�ʱ���������ڣ��������Ϊ0����ʾ���ζ�ʱ��,��λms */
					TX_AUTO_ACTIVATE);	/* ���ʱ�� */
	
	/* �����¼���־�� */
	tx_event_flags_create(&EventGroup, "EventGroupName");
}
void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();	/* �ᴥ����ɨ�躯�� */
}
    




