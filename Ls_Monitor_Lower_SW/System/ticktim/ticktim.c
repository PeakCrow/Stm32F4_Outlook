#include "ticktim.h"
/*ȫ������ʱ�䣬��λ1ms,����Ա�ʾ 24.85�죬�����Ĳ�Ʒ��������ʱ�䳬�������������뿼���������*/
__IO int32_t g_iRunTime = 0;
/*�����������ʱ���ṹ�����*/
static SOFT_TIM s_tTmr[TIM_TASK_COUNT] = {0};
/*�ȴ�������ʼ��*/
static __IO uint8_t g_ucEnableSystickISR = 0;
/*�δ�ʱ����ʱ��С��ʱ�������������ֵ*/
static void bsp_SoftTimerDec(SOFT_TIM *_tim);
/*�δ�ʱ���жϷ������*/
void SysTick_ISR(void);

/************************************************
*Function name	:bsp_InitTimer
*Description	:����systick�жϣ�����ʼ�������ʱ������;һ��Ҫ��ʼ����������޷�����������ʱ����
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��17��14��
*************************************************/
void bsp_InitTimer(void)
{
	uint8_t i;
	//�������е������ʱ��
	for(i = 0;i < TIM_TASK_COUNT;i++){	
		s_tTmr[i].State = 0;		
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TIM_ONCE_MODE; //Ĭ����һ���Թ���ģʽ
		s_tTmr[i].callfunc = 0;
	}
	/*
		����systic�ж�����Ϊ1ms��������systick�жϡ�

    	SystemCoreClock �ǹ̼��ж����ϵͳ�ں�ʱ�ӣ�����STM32H7,һ��Ϊ 400MHz

    	SysTick_Config() �������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�.
	    	-- SystemCoreClock / 1000  ��ʾ��ʱƵ��Ϊ 1000Hz�� Ҳ���Ƕ�ʱ����Ϊ  1ms
	    	-- SystemCoreClock / 500   ��ʾ��ʱƵ��Ϊ 500Hz��  Ҳ���Ƕ�ʱ����Ϊ  2ms
	    	-- SystemCoreClock / 2000  ��ʾ��ʱƵ��Ϊ 2000Hz�� Ҳ���Ƕ�ʱ����Ϊ  500us

    	���ڳ����Ӧ�ã�����һ��ȡ��ʱ����1ms�����ڵ���CPU���ߵ͹���Ӧ�ã��������ö�ʱ����Ϊ 10ms
    */
#if USE_THREADX	 == 0
	SysTick_Config(SystemCoreClock / 1000);
#endif
	g_ucEnableSystickISR = 1;	//1��ʾִ��systick�ж�
	
}
/************************************************
*Function name	:bsp_StartOnceTimer
*Description	:����һ��һ���ԵĶ�ʱ��
*Input			:_id:��ʱ������id	_period:��ʱ����������,��λ:ms
				:callfunc:���ܺ���
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��19��39��
*************************************************/
void bsp_StartOnceTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
	if(_id >= TIM_TASK_COUNT){
		/*��ӡ�����Դ�����ļ�������������*/
		printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
		while(1);	/*�����쳣�������ȴ����Ź���λ*/
	}
	__set_PRIMASK(1);	/*��ֹȫ���ж�*/
	s_tTmr[_id].State = 0;
	s_tTmr[_id].Count = _period;		/*ʵʱ��������ֵ*/
	s_tTmr[_id].PreLoad = _period;		/*�������Զ���װ��ֵ������ģʽ��������*/
	s_tTmr[_id].Flag = 0;				/*��ʱ�䵽��־*/
	s_tTmr[_id].Mode = TIM_ONCE_MODE;	/*һ���Թ���ģʽ*/
	s_tTmr[_id].callfunc = callfunc;	/*���ûص�����*/
	__set_PRIMASK(0);	/*����ȫ���ж�*/
}
/************************************************
*Function name	:bsp_StartAutoTimer
*Description	:����һ����ͣ��ʱ�Ķ�ʱ��
*Input			:_id:��ʱ������id	_period:��ʱ����������,��λ:ms
				:callfunc:���ܺ���
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��19��52��
*************************************************/
void bsp_StartAutoTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void))
{
	if(_id >= TIM_TASK_COUNT){
		/*��ӡ�����Դ�����ļ�������������*/
		printf("Error: file %s,function %s()\r\n", __FILE__,__FUNCTION__);
		while(1);/*�����쳣�������ȴ����Ź���λ*/
	}
	__set_PRIMASK(1);	/*��ֹȫ���ж�*/
	s_tTmr[_id].State = 1;
	s_tTmr[_id].Count = _period;		/*ʵʱ��������ֵ*/
	s_tTmr[_id].PreLoad = _period;		/*�������Զ���װ��ֵ�����Զ�ģʽ������*/
	s_tTmr[_id].Flag = 0;				/*��ʱʱ�䵽��־*/
	s_tTmr[_id].Mode = TIM_MULTI_MODE;	/*�Զ��ظ�����ģʽ*/
	s_tTmr[_id].callfunc = callfunc;	/*���ûص�����*/
	__set_PRIMASK(0);	/*���ж�*/
		
}
/************************************************
*Function name	:bsp_StopTimer
*Description	:ֹͣһ����ʱ���������Ҫ���µ��ã������ٴδ���
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��18��
*************************************************/
void bsp_StopTimer(uint8_t _id)
{
	s_tTmr[_id].callfunc = 0;
}
/************************************************
*Function name	:bsp_SoftTimerDec
*Description	:ÿ��1ms�����ж�ʱ��������1�����뱻SysTick_ISR�����Ե��á�
*Input			:_tim:��ʱ�������id����
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��07��
*************************************************/
static void bsp_SoftTimerDec(SOFT_TIM *_tim)
{
	if(_tim->Count > 0){
		/*�����ʱ����������1�����ö�ʱ�������־*/
		if(--_tim->Count == 0){
			_tim->Flag = 1;
			/*������Զ�ģʽ�����Զ���װ��ֵ*/
			if(_tim->Mode == TIM_MULTI_MODE){
				_tim->Count = _tim->PreLoad;
			}
		}
	}
}
/************************************************
*Function name	:SysTick_ISR
*Description	:SysTick�жϷ������ÿ��1ms����1��
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��08��
*************************************************/
void SysTick_ISR(void)
{
	static uint8_t s_count10ms = 0,s_count50ms = 0;
	uint8_t i;
	
	/*ÿ��1ms�������е������ʱ�����м�С����*/
	for(i = 0;i < TIM_TASK_COUNT;i++){
		bsp_SoftTimerDec(&s_tTmr[i]);	/*��ʱ�������С1*/
		if((s_tTmr[i].Flag == 1) && (s_tTmr[i].callfunc)){
			s_tTmr[i].callfunc();		//���ù��ܺ���
			s_tTmr[i].Flag = 0;
			if(s_tTmr[i].State == 0){	//�����һ���Ժ���
				s_tTmr[i].callfunc = 0;	//��ջص���������
			}
		}
	}	
	/*ȫ������ʱ��ÿ1ms��1*/
	g_iRunTime++;
	if(g_iRunTime == 0x7FFFFFFF){	/*uint32_t���͵����ֵ*/
		g_iRunTime = 0;
	}
	bsp_RunPer1ms();		/*ÿ��1ms����һ�δ˺���*/
	if(++s_count10ms >= 10){
		s_count10ms = 0;
		bsp_RunPer10ms();	/*ÿ��10ms����һ�δ˺���*/
	}
	if(++s_count50ms >= 50){
		s_count50ms = 0;
		bsp_RunPer50ms();	/*ÿ��50ms����һ�δ˺���*/
	}
}

/************************************************
*Function name	:bsp_RunPer1ms
*Description	:1ms���ڵ��ú�����user������д
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��09��
*************************************************/
__weak void bsp_RunPer1ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}
/************************************************
*Function name	:bsp_RunPer1ms
*Description	:10ms���ڵ��ú�����user������д
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��09��
*************************************************/
__weak void bsp_RunPer10ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}
/************************************************
*Function name	:bsp_RunPer1ms
*Description	:50ms���ڵ��ú�����user������д
*Input			:none
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��22��
*************************************************/
__weak void bsp_RunPer50ms(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_FLASH_OperationErrorCallback could be implemented in the user file
   */ 	
}

#if USE_THREADX == 0
/************************************************
*Function name	:bsp_DelayUs
*Description	:us���ӳ١� ������systick��ʱ����������ܵ��ô˺�����
*Input			:n:��ʱ����΢��
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��26��
*************************************************/
void bsp_DelayUs(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
	reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* �ս���ʱ�ļ�����ֵ */

    while (1)
    {
        tnow = SysTick->VAL;    
        if (tnow != told)
        {    
            /* SYSTICK��һ���ݼ��ļ����� */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* ����װ�صݼ� */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }	
}
/************************************************
*Function name	:bsp_DelayUs
*Description	:ms���ӳ١� ������systick��ʱ����������ܵ��ô˺�����
*Input			:n:��ʱ���ٺ���
				:
*Output			:none
*Retrurn		:none
*Author			:trx
*Date			:2022��4��5��22��26��
*************************************************/
void bsp_DelayMs(uint32_t n)
{
	uint32_t i;
	for(i = 0;i < n;i++){
		bsp_DelayUs(1000);
	}
}

/**
  * @brief  This function handles SysTick Handler.�ײ��жϺ���
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	if(g_ucEnableSystickISR == 0){	/*����û�н��жඨʱ�������ʼ��*/
		return;
	}
	SysTick_ISR();					/*��Ҫ���ж��в�ͣ�ĵ��õ��жϷ������*/	
}

#endif

int32_t bsp_GetRunTime(void)
{
	int32_t runtime;

	__set_PRIMASK(1);	/* 关闭全局中断 */
	runtime = g_iRunTime;
	__set_PRIMASK(0);	/* 打开全局中断 */
	return runtime;
}


