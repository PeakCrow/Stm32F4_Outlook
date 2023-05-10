#include "system_task_create.h"

/*
*********************************************************************************************************
*                                 �������ȼ�����ֵԽС���ȼ�Խ��
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          7u
#define  APP_CFG_TASK_MsgPro_PRIO                         3u
#define  APP_CFG_TASK_USER_IF_PRIO                        4u
#define  APP_CFG_TASK_COM_PRIO                            5u
#define  APP_CFG_TASK_STAT_PRIO                           30u
#define  APP_CFG_TASK_IDLE_PRIO                           31u
#define  APP_CFG_TASK_READC_PRIO						  6u
#define  APP_CFG_TASK_TFTLCD_PRIO						  2u


/*
*********************************************************************************************************
*                                    ����ջ��С����λ�ֽ�
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    1024u
#define  APP_CFG_TASK_COM_STK_SIZE                      1024u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  1024u
#define  APP_CFG_TASK_IDLE_STK_SIZE                  	1024u
#define  APP_CFG_TASK_STAT_STK_SIZE                  	1024u
#define  APP_CFG_TASK_READC_STK_SIZE                    1024u
#define  APP_CFG_TASK_TFTLCD_STK_SIZE                   2*2048u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   1024u
/*
*********************************************************************************************************
*                                       ��̬ȫ�ֱ���
*********************************************************************************************************
*/
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];

static  TX_THREAD   AppTaskCOMTCB;
static  uint64_t    AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE/8];

static  TX_THREAD   AppTaskUserIFTCB;
static  uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];

static  TX_THREAD   AppTaskIdleTCB;
static  uint64_t    AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE/8];

static  TX_THREAD   AppTaskStatTCB;
static  uint64_t    AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE/8];

static  TX_THREAD	AppTaskReadAdcTCB;
static  uint64_t	AppTaskReadAdcStk[APP_CFG_TASK_READC_STK_SIZE/8];

static  TX_THREAD	AppTaskTFTLCDTCB;
static  uint64_t	AppTaskTFTLCDStk[APP_CFG_TASK_TFTLCD_STK_SIZE/8];

static  TX_THREAD   AppTaskMsgProTCB;
static  uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];

/*
*********************************************************************************************************
*                                       �����ʱ������
*********************************************************************************************************
*/
TX_TIMER AppTimer;
TX_MUTEX AppPrintfSemp;				/* ����printf���� */
TX_MUTEX AppLCDSemp;				/* ����LVGL���� */
TX_MUTEX App_PowerDownSave;			/* ���ڵ��籣�� */  
TX_EVENT_FLAGS_GROUP  EventGroup; 	/* �¼���־�� */
/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskUserIF         (ULONG thread_input);
static  void  AppTaskCOM			(ULONG thread_input);
static  void  AppTaskIDLE			(ULONG thread_input);
static  void  AppTaskStat			(ULONG thread_input);
static  void  AppTaskREADADC		(ULONG thread_input);
static  void  AppTaskTFTLCD			(ULONG thread_input);
static  void  AppTaskMsgPro         (ULONG thread_input);  
static  void  AppTaskCreate         (void);
static  void  DispTaskInfo          (void);
static  void  AppObjCreate          (void);
static  void  OSStatInit 			(void);
        void  App_Printf 			(const char *fmt, ...);
        void  App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);
        void  TimerCallback         (ULONG thread_input);
/*
*******************************************************************************************************
*                               ����
*******************************************************************************************************
*/
__IO uint8_t   OSStatRdy;      		/* ͳ�����������־ */
__IO uint32_t  OSIdleCtr;     	    /* ����������� */
__IO float     OSCPUUsage;   	    /* CPU�ٷֱ� */
uint32_t       OSIdleCtrMax; 	    /* 1�������Ŀ��м��� */
uint32_t       OSIdleCtrRun; 	    /* 1���ڿ�������ǰ���� */


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


/*
*********************************************************************************************************
*	�� �� ��: tx_application_define
*	����˵��: ThreadXר�õ����񴴽���ͨ����������������˺�������tx_kernel_enter�����б�����(ò��)
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
    tx_thread_create(&AppTaskStartTCB,              /* ������ƿ��ַ */   
                       "App Task Start",              /* ������ */
                       AppTaskStart,                  /* ������������ַ */
                       0,                             /* ���ݸ�����Ĳ��� */
                       &AppTaskStartStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_START_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_START_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_START_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               /* ������ʱ��Ƭ */
                       TX_AUTO_START);                 /* �������������� */
   	   
	/**************����ͳ������*********************/
    tx_thread_create(&AppTaskStatTCB,               /* ������ƿ��ַ */    
                       "App Task STAT",              /* ������ */
                       AppTaskStat,                  /* ������������ַ */
                       0,                           /* ���ݸ�����Ĳ��� */
                       &AppTaskStatStk[0],           /* ��ջ����ַ */
                       APP_CFG_TASK_STAT_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_STAT_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_STAT_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             /* ������ʱ��Ƭ */
                       TX_AUTO_START);               /* �������������� */
					   
				   
	/**************������������*********************/
    tx_thread_create(&AppTaskIdleTCB,               /* ������ƿ��ַ */    
                       "App Task IDLE",              /* ������ */
                       AppTaskIDLE,                  /* ������������ַ */
                       0,                           /* ���ݸ�����Ĳ��� */
                       &AppTaskIdleStk[0],           /* ��ջ����ַ */
                       APP_CFG_TASK_IDLE_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_IDLE_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_IDLE_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             /* ������ʱ��Ƭ */
                       TX_AUTO_START);               /* �������������� */
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
    tx_thread_create(&AppTaskUserIFTCB,               	/* ������ƿ��ַ */      
                       "App Task UserIF",              	/* ������ */
                       AppTaskUserIF,                  	/* ������������ַ */
                       0,                              	/* ���ݸ�����Ĳ��� */
                       &AppTaskUserIFStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_USER_IF_STK_SIZE,  	/* ��ջ�ռ��С */  
                       APP_CFG_TASK_USER_IF_PRIO,      	/* �������ȼ�*/
                       APP_CFG_TASK_USER_IF_PRIO,      	/* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               	/* ������ʱ��Ƭ */
                       TX_AUTO_START);                 	/* �������������� */
	/**************����MsgPro����*********************/
    tx_thread_create(&AppTaskMsgProTCB,               /* ������ƿ��ַ */    
                       "App Msp Pro",                 /* ������ */
                       AppTaskMsgPro,                  /* ������������ַ */
                       0,                             /* ���ݸ�����Ĳ��� */
                       &AppTaskMsgProStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_MsgPro_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_MsgPro_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_MsgPro_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               /* ������ʱ��Ƭ */
                       TX_AUTO_START);                /* �������������� */	

	/**************����COM����*********************/
    tx_thread_create(&AppTaskCOMTCB,               		/* ������ƿ��ַ */    
                       "App Task COM",              	/* ������ */
                       AppTaskCOM,                  	/* ������������ַ */
                       0,                           	/* ���ݸ�����Ĳ��� */
                       &AppTaskCOMStk[0],            	/* ��ջ����ַ */
                       APP_CFG_TASK_COM_STK_SIZE,    	/* ��ջ�ռ��С */  
                       APP_CFG_TASK_COM_PRIO,        	/* �������ȼ�*/
                       APP_CFG_TASK_COM_PRIO,        	/* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             	/* ������ʱ��Ƭ */
                       TX_AUTO_START);               	/* �������������� */

	/**************����READ ADC����*********************/
    tx_thread_create(&AppTaskReadAdcTCB,               		/* ������ƿ��ַ */    
                       "App Task ReadAdc",              	/* ������ */
                       AppTaskREADADC,                  	/* ������������ַ */
                       0,                           	/* ���ݸ�����Ĳ��� */
                       &AppTaskReadAdcStk[0],            	/* ��ջ����ַ */
                       APP_CFG_TASK_READC_STK_SIZE,    	/* ��ջ�ռ��С */  
                       APP_CFG_TASK_READC_PRIO,        	/* �������ȼ�*/
                       APP_CFG_TASK_READC_PRIO,        	/* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             	/* ������ʱ��Ƭ */
                       TX_AUTO_START);               	/* �������������� */
	
	/**************����LCD DISP����*********************/
    tx_thread_create(&AppTaskTFTLCDTCB,               		/* ������ƿ��ַ */    
                       "App Task TFTLCD",              	/* ������ */
                       AppTaskTFTLCD,                  	/* ������������ַ */
                       0,                           	/* ���ݸ�����Ĳ��� */
                       &AppTaskTFTLCDStk[0],            	/* ��ջ����ַ */
                       APP_CFG_TASK_TFTLCD_STK_SIZE,    	/* ��ջ�ռ��С */  
                       APP_CFG_TASK_TFTLCD_PRIO,        	/* �������ȼ�*/
                       APP_CFG_TASK_TFTLCD_PRIO,        	/* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             	/* ������ʱ��Ƭ */
                       TX_AUTO_START);               	/* �������������� */

}

static int test_master(void)
{
	setNodeId(&TestSlave_Data_Data,0x00);
	setState(&TestSlave_Data_Data,Initialisation);
	setState(&TestSlave_Data_Data,Operational);
	return 0;
}
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
	bsp_InitADS1256();							/* ��ʼ������ADS1256.  PGA=1, DRATE=30KSPS, BUFEN=1, ��������5V */
	bsp_InitWs2812b();							/* ��ʼ��ws2812b�ɵ���Ч */
	bsp_SetTIMOutPWM(GPIOB,GPIO_PIN_6,TIM4,1,500,5000);/* ����һ��1k��50ռ�ձȵķ�����������֤������� */
	bsp_InitSPI1Bus();							/* SPI1���߳�ʼ�� */
	bsp_InitSFlash();							/* ��ʼ��SPI FLASHоƬ */
    bsp_InitSram();                             /* �ⲿsram��ʼ�� */
	lv_init(); 									/* lvgl ϵͳ��ʼ�� */
	lv_port_disp_init(); 						/* lvgl ��ʾ�ӿڳ�ʼ��,���� lv_init()�ĺ��� */
	lv_port_indev_init(); 						/* lvgl ����ӿڳ�ʼ��,���� lv_init()�ĺ��� */
	
	test_master();
	
	/* ���������ͨ�Ż���,��Ҫ�Ǹ��������ͨѶ���� */
	AppObjCreate();

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
*	�� �� ��: AppTaskStatistic
*	����˵��: ͳ����������ʵ��CPU�����ʵ�ͳ�ơ�Ϊ�˲��Ը���׼ȷ�����Կ���ע�͵��õ�ȫ���жϿ���
*	��    �� : thread_input ����������ʱ���ݵ��β� 
*	�� �� ֵ: ��
*   �� �� ��: 30
*********************************************************************************************************
*/
static void AppTaskStat(ULONG thread_input)
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
static void AppTaskIDLE(ULONG thread_input)
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


/******************************************����������������**********************************************/
/******************************************����������������**********************************************/
/******************************************����������������**********************************************/




static void AppTaskMsgPro(ULONG thread_input)
{
	(void)thread_input;
	ULONG actual_events;
	UINT status;
	//uint8_t stop_motor[] = {0xe0,0xf7};


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




/*******************************************************************************
  * @FunctionName: AppTaskTFTLCD
  * @Author:       trx
  * @DateTime:     2022��6��23��21:03:00 
  * @Purpose:      lcd��Ļ��������
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
static void AppTaskTFTLCD    (ULONG thread_input)
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

/*******************************************************************************
  * @FunctionName: AppTaskREADADC
  * @Author:       trx
  * @DateTime:     2022��6��23��21:03:14 
  * @Purpose:      adc���ݶ�ȡ����
  * @param:        thread_input
  * @return:       none
*******************************************************************************/
static  void  AppTaskREADADC	(ULONG thread_input)
{
	(VOID)thread_input;
//	uint8_t i;
//	int32_t iTemp;
//	float fTemp;
	

		/* ��ӡоƬID (ͨ����ID�����ж�Ӳ���ӿ��Ƿ�����) , ����ʱ״̬�Ĵ����ĸ�4bit = 3 */
#if 0
		{
			uint8_t id;
	
			id = ADS1256_ReadChipID();
	
			if (id != 3)
			{
				App_Printf("Error, ASD1256 Chip ID = 0x%X\r\n", id);
			}
			else
			{
				App_Printf("Ok, ASD1256 Chip ID = 0x%X\r\n", id);
			}
		}
#endif
		
		ADS1256_CfgADC(ADS1256_GAIN_1, ADS1256_30SPS);	/* ����ADC������ ����1:1, ����������� 30Hz */
	
		ADS1256_StartScan();	/* �����ж�ɨ��ģʽ, �����ɼ�8��ͨ����ADC����. ͨ�� ADS1256_GetAdc() ��������ȡ��Щ���� */
while (1)
	{
		#if 0
		/* ��ӡ�ɼ����� */
		for (i = 0; i < 8; i++)
			{

			    
			    //���㹫ʽ = 2 * VREF/(PGA * (2^23 - 1)) ������VREF��2.5V��PGA = 1
			    //����ʵ�ʵ�ѹֵ�����ƹ���ģ�������׼ȷ�������У׼
			    
				iTemp = ((int64_t)g_tADS1256.AdcNow[i] * 2500000) / 4194303; 
				
				fTemp = (float)iTemp / 1000000;   

				App_Printf("CH%d=%07d(%fV) ", i, g_tADS1256.AdcNow[i], fTemp);

				if(i == 3)
					{
						App_Printf("\r\n");
					}
			}
		App_Printf("\r\n\r\n");
		#else
		//iTemp = ((int64_t)g_tADS1256.AdcNow[5] * 2500000) / 4194303; 
		
		//fTemp = (float)iTemp / 1000000;   
		
		//App_Printf("CH%d=%07d(%fV) ", 6, g_tADS1256.AdcNow[5], fTemp);
		//App_Printf("\r\n");

		/* ��������ʲô��������� */
		//App_Printf("driver 1 pos is : %.2f driver current pos is %.2f\r\n",DriverX_Pos.driver1_pos,DriverX_Pos.current_pos);		
		#endif
		tx_thread_sleep(100);
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
static void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* �������� */
	(void)thread_input;
	uint16_t pos = 0;
	uint8_t buf[2] = {0};
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
					case KEY_UP_DOWN:			/* kup�������� */
						App_Printf("kup��������\r\n");				//��ɫ	
						break;
					case KEY_0_DOWN:			/* k0�������� */
					{
						App_Printf("k0��������\r\n");	
						DispTaskInfo();
						break;
					}
					case KEY_UP_UP:
						//I2C_EE_BufferRead(&pos1,0x05,1);
						//I2C_EE_BufferRead(&pos2,0x06,1);
						I2C_EE_BufferRead(buf,0x05,2);
						pos = buf[0] * 100 + buf[1];
						App_Printf("kup��������,pos %d  \r\n",pos);
						break;
				}
		}
        tx_thread_sleep(20);
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

void    App_I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
	/* ������� */
    tx_mutex_get(&App_PowerDownSave, TX_WAIT_FOREVER);

    I2C_EE_BufferWrite(pBuffer,WriteAddr,NumByteToWrite);

    tx_mutex_put(&App_PowerDownSave);	
}


/*
*********************************************************************************************************
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-III������Ϣͨ�����ڴ�ӡ����
*	��    �� ����
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static void DispTaskInfo(void)
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
static  void  AppObjCreate (void)
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
    




