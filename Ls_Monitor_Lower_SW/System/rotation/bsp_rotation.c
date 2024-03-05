#include "bsp_rotation.h"

#define ROTATION_SENSOR_TIM			TIM2
#define ROTATION_SENSOR_PIN			GPIO_PIN_15
#define	ROTATION_SENSOR_PORT		GPIOA
#define ROTATION_SENSOR_TIM_PERIOD 	0xFFFFFFFF


static TIM_HandleTypeDef 	g_RotationTimeHandle;
static uint8_t g_updatecounter = 0;

/*******************************************************************************
  * @FunctionName: bsp_InitRotationSensor
  * @Author:       trx
  * @DateTime:     2022????5??22:13:00 
  * @Purpose:      ���ٴ�������ʼ����������Ҫ��ʼ����ʱ��ETR����
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InitRotationSensor(void)
{
	TIM_ClockConfigTypeDef sclocksourceconfig = {0};
	TIM_MasterConfigTypeDef smasterconfig = {0};

	g_RotationTimeHandle.Instance 				= ROTATION_SENSOR_TIM;
	g_RotationTimeHandle.Init.Prescaler 		= 0;
	g_RotationTimeHandle.Init.CounterMode 		= TIM_COUNTERMODE_UP;				/* ���ϼ���ģʽ */
	g_RotationTimeHandle.Init.Period 			= ROTATION_SENSOR_TIM_PERIOD;		/* ��ʱ????2λ��ʱ�� */
	g_RotationTimeHandle.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;			/* ��ʱ??��Ƶ */
	g_RotationTimeHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;	/* ��ʱ����װ��ֵʧ??*/

	__HAL_TIM_ENABLE_IT(&g_RotationTimeHandle, TIM_IT_UPDATE);						/* ʹ�ܶ�ʱ�����������??*/
	
	if(HAL_TIM_Base_Init(&g_RotationTimeHandle) != HAL_OK)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }
	sclocksourceconfig.ClockSource 				= TIM_CLOCKSOURCE_ETRMODE2;
	sclocksourceconfig.ClockPolarity 			= TIM_CLOCKPOLARITY_RISING;
	sclocksourceconfig.ClockPrescaler 			= TIM_CLOCKPRESCALER_DIV1;
	sclocksourceconfig.ClockFilter 				= 0;
	if(HAL_TIM_ConfigClockSource(&g_RotationTimeHandle,&sclocksourceconfig) != HAL_OK)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }
	smasterconfig.MasterOutputTrigger 			= TIM_TRGO_RESET;
	smasterconfig.MasterSlaveMode 				= TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&g_RotationTimeHandle,&smasterconfig) != HAL_OK)
    {
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
    }

	HAL_NVIC_SetPriority(TIM2_IRQn,1,0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);				/* �����ж�����?? */
	
	HAL_TIM_Base_Start(&g_RotationTimeHandle);
}

void TIM2_IRQHandler(void)
{
	  /* TIM Update event */
  if (__HAL_TIM_GET_FLAG(&g_RotationTimeHandle, TIM_FLAG_UPDATE) != RESET)
  {
    if (__HAL_TIM_GET_IT_SOURCE(&g_RotationTimeHandle, TIM_IT_UPDATE) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&g_RotationTimeHandle, TIM_IT_UPDATE);

      HAL_TIM_PeriodElapsedCallback(&g_RotationTimeHandle);
    }
  }
}
/*******************************************************************************
  * @FunctionName: HAL_TIM_PeriodElapsedCallback
  * @Author:       trx
  * @DateTime:     2022????5??22:11:00 
  * @Purpose:      ��ʱ������ص���??
  * @param:        _htim����ʱ�����
  * @return:       none
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *_htim)
{
	(void)_htim;
	g_updatecounter++;							/* ��ʱ��ÿ������Σ�������?? */
}

/*******************************************************************************
  * @FunctionName: HAL_TIM_Base_MspInit
  * @Author:       trx
  * @DateTime:     2022????5??22:14:07 
  * @Purpose:      HAL_TIM_Base_Init�����ص�������������ʼ������
  * @param:        htim����ʱ�����
  * @return:       none
*******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef * htim)
{
	GPIO_InitTypeDef gpio_initstruct = {0};
	if(htim->Instance == ROTATION_SENSOR_TIM)
		{
			__HAL_RCC_TIM2_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();

			gpio_initstruct.Pin 			= ROTATION_SENSOR_PIN;
			gpio_initstruct.Mode 			= GPIO_MODE_AF_PP;
			gpio_initstruct.Pull 			= GPIO_PULLUP;
			gpio_initstruct.Speed 			= GPIO_SPEED_FREQ_HIGH;
			gpio_initstruct.Alternate 		= GPIO_AF1_TIM2;
			HAL_GPIO_Init(ROTATION_SENSOR_PORT,&gpio_initstruct);
		}
}
/*******************************************************************************
  * @FunctionName: Rotation_Sensor_Get
  * @Author:       trx
  * @DateTime:     2022????5??22:05:34 
  * @Purpose:      ���ټ�??0.5s�ɼ�����һ??
  * @param:        _gear_num    �����ֳ�??
  * @param:        _wheel_radius�����ְ뾶����λm
  * @return:       ���٣���λm/s
*******************************************************************************/
float Rotation_Sensor_Get(uint8_t _gear_num,float _wheel_radius)
{
	uint32_t pulse_num = 0;
	float rotation = 0;
	pulse_num =(__HAL_TIM_GET_COUNTER(&g_RotationTimeHandle) + (g_updatecounter * ROTATION_SENSOR_TIM_PERIOD));/* �õ�������??*/
	/* ������õ�������??���������غ��½��أ������ڳ�??.5s��ʱ�������ȥ����һ����??*/
	__HAL_TIM_SET_COUNTER(&g_RotationTimeHandle,0);/* �������Ĵ���?? */
	g_updatecounter = 0;//��ʱ�������־��0
	rotation = (pulse_num/_gear_num * _wheel_radius * 3.14f / 0.5f);	/* ������̥���ܳ��ͳ��ֵĳ������������٣���λm/s */
	return rotation;
}

