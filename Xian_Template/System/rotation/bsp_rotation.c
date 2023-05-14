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
  * @DateTime:     2022�?�?5�?22:13:00 
  * @Purpose:      轮速传感器初始化函数，主要初始化定时器ETR功能
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InitRotationSensor(void)
{
	TIM_ClockConfigTypeDef sclocksourceconfig = {0};
	TIM_MasterConfigTypeDef smasterconfig = {0};

	g_RotationTimeHandle.Instance 				= ROTATION_SENSOR_TIM;
	g_RotationTimeHandle.Init.Prescaler 		= 0;
	g_RotationTimeHandle.Init.CounterMode 		= TIM_COUNTERMODE_UP;				/* 向上计数模式 */
	g_RotationTimeHandle.Init.Period 			= ROTATION_SENSOR_TIM_PERIOD;		/* 定时�?�?2位定时器 */
	g_RotationTimeHandle.Init.ClockDivision 	= TIM_CLOCKDIVISION_DIV1;			/* 定时�?分频 */
	g_RotationTimeHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;	/* 定时器重装载值失�?*/

	__HAL_TIM_ENABLE_IT(&g_RotationTimeHandle, TIM_IT_UPDATE);						/* 使能定时器溢出更新中�?*/
	
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
	HAL_NVIC_EnableIRQ(TIM2_IRQn);				/* 配置中断优先�? */
	
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
  * @DateTime:     2022�?�?5�?22:11:00 
  * @Purpose:      定时器溢出回调函�?
  * @param:        _htim：定时器句柄
  * @return:       none
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *_htim)
{
	(void)_htim;
	g_updatecounter++;							/* 定时器每溢出依次，计数器�? */
}

/*******************************************************************************
  * @FunctionName: HAL_TIM_Base_MspInit
  * @Author:       trx
  * @DateTime:     2022�?�?5�?22:14:07 
  * @Purpose:      HAL_TIM_Base_Init函数回调函数，用来初始化引脚
  * @param:        htim：定时器句柄
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
  * @DateTime:     2022�?�?5�?22:05:34 
  * @Purpose:      轮速计�?0.5s采集计算一�?
  * @param:        _gear_num    ：车轮齿�?
  * @param:        _wheel_radius：车轮半径，单位m
  * @return:       轮速，单位m/s
*******************************************************************************/
float Rotation_Sensor_Get(uint8_t _gear_num,float _wheel_radius)
{
	uint32_t pulse_num = 0;
	float rotation = 0;
	pulse_num =(__HAL_TIM_GET_COUNTER(&g_RotationTimeHandle) + (g_updatecounter * ROTATION_SENSOR_TIM_PERIOD));/* 得到脉冲数�?*/
	/* 在这里得到的脉冲�?含有上升沿和下降沿，但是在除�?.5s的时间后正好去掉了一半脉�?*/
	__HAL_TIM_SET_COUNTER(&g_RotationTimeHandle,0);/* 将计数寄存器�? */
	g_updatecounter = 0;//定时器溢出标志清0
	rotation = (pulse_num/_gear_num * _wheel_radius * 3.14f / 0.5f);	/* 根据轮胎的周长和齿轮的齿数来计算轮速，单位m/s */
	return rotation;
}

