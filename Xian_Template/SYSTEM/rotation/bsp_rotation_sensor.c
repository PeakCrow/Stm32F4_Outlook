#include "sys.h"

#define ROTATION_SENSOR_TIM		TIM2
#define ROTATION_SENSOR_PIN		GPIO_PIN_15
#define	ROTATION_SENSOR_PORT	GPIOA



static TIM_HandleTypeDef 	g_RotationTimeHandle;

void bsp_InitRotationSensor(void)
{
	TIM_ClockConfigTypeDef sclocksourceconfig = {0};
	TIM_MasterConfigTypeDef smasterconfig = {0};

	g_RotationTimeHandle.Instance = ROTATION_SENSOR_TIM;
	g_RotationTimeHandle.Init.Prescaler = 0;
	g_RotationTimeHandle.Init.CounterMode = TIM_COUNTERMODE_UP;/* 向上计数模式 */
	g_RotationTimeHandle.Init.Period = 0xffffffff;/* 定时器2是32位定时器 */
	g_RotationTimeHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;/* 定时器1分频 */
	g_RotationTimeHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;/* 定时器重装载值失能 */

	if(HAL_TIM_Base_Init(&g_RotationTimeHandle) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
	sclocksourceconfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
	sclocksourceconfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
	sclocksourceconfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
	sclocksourceconfig.ClockFilter = 0;
	if(HAL_TIM_ConfigClockSource(&g_RotationTimeHandle,&sclocksourceconfig) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
	smasterconfig.MasterOutputTrigger = TIM_TRGO_RESET;
	smasterconfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&g_RotationTimeHandle,&smasterconfig) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}

	HAL_TIM_Base_Start(&g_RotationTimeHandle);
}



void HAL_TIM_Base_MspInit(TIM_HandleTypeDef * htim)
{
	GPIO_InitTypeDef gpio_initstruct = {0};
	if(htim->Instance == ROTATION_SENSOR_TIM)
		{
			__HAL_RCC_TIM2_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();

			gpio_initstruct.Pin = ROTATION_SENSOR_PIN;
			gpio_initstruct.Mode = GPIO_MODE_AF_PP;
			gpio_initstruct.Pull = GPIO_PULLUP;
			gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
			gpio_initstruct.Alternate = GPIO_AF1_TIM2;
			HAL_GPIO_Init(ROTATION_SENSOR_PORT,&gpio_initstruct);
		}
}

float Rotation_Sensor_Get(uint8_t _gear_num,float _wheel_radius,float _collectiontime)
{
	uint32_t pulse_num = 0;
	float rotation = 0;
	pulse_num = __HAL_TIM_GET_COUNTER(&g_RotationTimeHandle);/* 得到脉冲数值 */
	__HAL_TIM_SET_COUNTER(&g_RotationTimeHandle,0);/* 将计数寄存器清0 */
	rotation = (pulse_num/_gear_num * _wheel_radius * 3.14f / _collectiontime);	/* 根据轮胎的周长和齿轮的齿数来计算轮速，单位m/s */
	//rotation = pulse_num/24*0.464*3.14/0.5;	/* 根据轮胎的周长和齿轮的齿数来计算轮速，单位m/s */
	return rotation;
}

