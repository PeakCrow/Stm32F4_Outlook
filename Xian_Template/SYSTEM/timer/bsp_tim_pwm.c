/*
*********************************************************************************************************
*
*	模块名称 : TIM基本定时中断和PWM驱动模块
*	文件名称 : bsp_tim_pwm.c
*	版    本 : V1.6
*	说    明 : 利用STM32F429内部TIM输出PWM信号， 并实现基本的定时中断
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-08-16 armfly  正式发布
*		V1.1	2014-06-15 armfly  完善 bsp_SetTIMOutPWM，当占空比=0和100%时，关闭定时器，GPIO配置为输出
*		V1.2	2015-05-08 armfly  解决TIM8不能输出PWM的问题。
*		V1.3	2015-07-23 armfly  初始化定时器，必须设置 TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x0000;		
*								   TIM1 和 TIM8 必须设置。否则蜂鸣器的控制不正常。
*		V1.4	2015-07-30 armfly  增加反相引脚输出PWM函数 bsp_SetTIMOutPWM_N();
*		V1.5	2016-02-01 armfly  去掉 TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
*		V1.6	2016-02-27 armfly  解决TIM14无法中断的BUG, TIM8_TRG_COM_TIM14_IRQn
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "sys.h"


/*
 	可以输出到GPIO的TIM通道:

	TIM1_CH1, PA8,	PE9,
	TIM1_CH2, PA9,	PE11
	TIM1_CH3, PA10,	PE13
	TIM1_CH4, PA11,	PE14

	TIM2_CH1, PA15 (仅限429，439) 407没有此脚
	TIM2_CH2, PA1,	PB3
	TIM2_CH3, PA2,	PB10
	TIM2_CH4, PA3,	PB11

	TIM3_CH1, PA6,  PB4, PC6
	TIM3_CH2, PA7,	PB5, PC7
	TIM3_CH3, PB0,	PC8
	TIM3_CH4, PB1,	PC9

	TIM4_CH1, PB6,  PD12
	TIM4_CH2, PB7,	PD13
	TIM4_CH3, PB8,	PD14
	TIM4_CH4, PB9,	PD15

	TIM5_CH1, PA0,  PH10
	TIM5_CH2, PA1,	PH11
	TIM5_CH3, PA2,	PH12
	TIM5_CH4, PA3,	PI10

	TIM8_CH1, PC6,  PI5
	TIM8_CH2, PC7,	PI6
	TIM8_CH3, PC8,	PI7
	TIM8_CH4, PC9,	PI2

	TIM9_CH1, PA2,  PE5
	TIM9_CH2, PA3,	PE6

	TIM10_CH1, PB8,  PF6

	TIM11_CH1, PB9,  PF7

	TIM12_CH1, PB14,  PH6
	TIM12_CH2, PB15,  PH9

	TIM13_CH1, PA6,  PF8
	TIM14_CH1, PA7,  PF9

	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	

	APB1 定时器的输入时钟 TIMxCLK = SystemCoreClock / 2; 84M
	APB2 定时器的输入时钟 TIMxCLK = SystemCoreClock; 168M
*/


static DMA_HandleTypeDef hdma_ch1 = {0};
TIM_HandleTypeDef  g_TimHandle = {0};

/*
*********************************************************************************************************
*	函 数 名: bsp_RCC_GPIO_Enable
*	功能说明: 使能GPIO时钟
*	形    参: GPIOx GPIOA - GPIOI
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RCC_GPIO_Enable(GPIO_TypeDef* GPIOx)
{
	if (GPIOx == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
	else if (GPIOx == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
	else if (GPIOx == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
	else if (GPIOx == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
	else if (GPIOx == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
	else if (GPIOx == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
	else if (GPIOx == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
	else if (GPIOx == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
	else if (GPIOx == GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RCC_TIM_Enable
*	功能说明: 使能TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_RCC_TIM_Enable(TIM_TypeDef* TIMx)
{
	if (TIMx == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
	else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
	else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
	else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
	else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
	else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
	else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
	else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
	else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
	else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_ENABLE();
	else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_ENABLE();
	else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
	else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
	else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();
	else
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_RCC_TIM_Disable
*	功能说明: 关闭TIM RCC 时钟
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: TIM外设时钟名
*********************************************************************************************************
*/
void bsp_RCC_TIM_Disable(TIM_TypeDef* TIMx)
{
	/*
		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	*/
	if (TIMx == TIM1) __HAL_RCC_TIM3_CLK_DISABLE();
	else if (TIMx == TIM2) __HAL_RCC_TIM2_CLK_DISABLE();
	else if (TIMx == TIM3) __HAL_RCC_TIM3_CLK_DISABLE();
	else if (TIMx == TIM4) __HAL_RCC_TIM4_CLK_DISABLE();
	else if (TIMx == TIM5) __HAL_RCC_TIM5_CLK_DISABLE();
	else if (TIMx == TIM6) __HAL_RCC_TIM6_CLK_DISABLE();
	else if (TIMx == TIM7) __HAL_RCC_TIM7_CLK_DISABLE();
	else if (TIMx == TIM8) __HAL_RCC_TIM8_CLK_DISABLE();
	else if (TIMx == TIM9) __HAL_RCC_TIM9_CLK_DISABLE();
	else if (TIMx == TIM10) __HAL_RCC_TIM10_CLK_DISABLE();
	else if (TIMx == TIM11) __HAL_RCC_TIM11_CLK_DISABLE();
	else if (TIMx == TIM12) __HAL_RCC_TIM12_CLK_DISABLE();
	else if (TIMx == TIM13) __HAL_RCC_TIM13_CLK_DISABLE();
	else if (TIMx == TIM14) __HAL_RCC_TIM14_CLK_DISABLE();
	else
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetAFofTIM
*	功能说明: 根据TIM 得到AF寄存器配置
*	形    参: TIMx TIM1 - TIM14
*	返 回 值: AF寄存器配置
*********************************************************************************************************
*/
uint8_t bsp_GetAFofTIM(TIM_TypeDef* TIMx)
{
	uint8_t ret = 0;

	if (TIMx == TIM1) ret = GPIO_AF1_TIM1;
	else if (TIMx == TIM2) ret = GPIO_AF1_TIM2;
	
	else if (TIMx == TIM3) ret = GPIO_AF2_TIM3;
	else if (TIMx == TIM4) ret = GPIO_AF2_TIM4;
	else if (TIMx == TIM5) ret = GPIO_AF2_TIM5;
	
	else if (TIMx == TIM8) ret = GPIO_AF3_TIM8;
	else if (TIMx == TIM9) ret = GPIO_AF3_TIM9;
	else if (TIMx == TIM10) ret = GPIO_AF3_TIM10;
	else if (TIMx == TIM11) ret = GPIO_AF3_TIM11;
	
	else if (TIMx == TIM12) ret = GPIO_AF9_TIM12;
	else if (TIMx == TIM13) ret = GPIO_AF9_TIM13;
	else if (TIMx == TIM14) ret = GPIO_AF9_TIM14;
	else
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	
	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ConfigTimGpio
*	功能说明: 配置GPIO和TIM时钟， GPIO连接到TIM输出通道
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM14
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigTimGpio(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX, TIM_TypeDef* TIMx)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* 使能GPIO时钟 */
	bsp_RCC_GPIO_Enable(GPIOx);

  	/* 使能TIM时钟 */
	bsp_RCC_TIM_Enable(TIMx);

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = bsp_GetAFofTIM(TIMx);
	GPIO_InitStruct.Pin = GPIO_PinX;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_ConfigGpioOut
*	功能说明: 配置GPIO为推挽输出。主要用于PWM输出，占空比为0和100的情况。
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_PinX : GPIO_PIN_0 - GPIO__PIN_15
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_ConfigGpioOut(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinX)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	bsp_RCC_GPIO_Enable(GPIOx);		/* 使能GPIO时钟 */

	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Pin = GPIO_PinX;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	HAL_TIM_PWM_Stop_DMA(&g_TimHandle, TIM_CHANNEL_1);
}

void MX_DMA_Init(void)
{
	
		
		__HAL_RCC_DMA1_CLK_ENABLE();//开启DMA1时钟
		__DMA1_CLK_ENABLE();
		
		hdma_ch1.Instance 					= DMA1_Stream4;
		hdma_ch1.Init.Channel 				= DMA_CHANNEL_5;			//通道选择2--
		hdma_ch1.Init.Direction				= DMA_MEMORY_TO_PERIPH;		//存储器到外设
		hdma_ch1.Init.PeriphInc 			= DMA_PINC_DISABLE;			//外设非增量模式
		hdma_ch1.Init.MemInc				= DMA_MINC_ENABLE;			//存储器增量模式
		hdma_ch1.Init.PeriphDataAlignment 	= DMA_PDATAALIGN_HALFWORD;		//外设数据长度16位
		hdma_ch1.Init.MemDataAlignment 		= DMA_MDATAALIGN_HALFWORD;		//存储器数据长度16位
		hdma_ch1.Init.Mode 					= DMA_CIRCULAR;				//外设周期模式可以发送数组内的数据，正常模式只会发送第一个数据
		hdma_ch1.Init.Priority 				= DMA_PRIORITY_LOW;			//中等优先级
		hdma_ch1.Init.FIFOMode 				= DMA_FIFOMODE_DISABLE;
		hdma_ch1.Init.FIFOThreshold 		= DMA_FIFO_THRESHOLD_FULL;
		hdma_ch1.Init.MemBurst 				= DMA_MBURST_SINGLE;		//存储器单次传输
		hdma_ch1.Init.PeriphBurst 			= DMA_MBURST_SINGLE;		//外设突发单次传输
		
		__HAL_LINKDMA(&g_TimHandle,hdma[TIM_DMA_ID_CC1],hdma_ch1);//将DMA与TIM联系起来	
		
		if(HAL_DMA_DeInit(&hdma_ch1) != HAL_OK)
			{
				printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
			}
		if(HAL_DMA_Init((&g_TimHandle)->hdma[TIM_DMA_ID_CC1]) != HAL_OK)//4--
			{
				printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
			}
		
		HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);//6--
		HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	
}
void DMA1_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_ch1);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

void TIM3_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&g_TimHandle);
}


/*
*********************************************************************************************************
*	函 数 名: bsp_SetTIMOutPWM
*	功能说明: 设置引脚输出的PWM信号的频率和占空比.  当频率为0，并且占空为0时，关闭定时器，GPIO输出0；
*			  当频率为0，占空比为100%时，GPIO输出1.
*	形    参: GPIOx : GPIOA - GPIOK
*			  GPIO_Pin : GPIO_PIN_0 - GPIO__PIN_15
*			  TIMx : TIM1 - TIM14
*             _ucChannel：使用的定时器通道，范围1 - 4
*			  _ulFreq : PWM信号频率，单位Hz (实际测试，可以输出100MHz），0 表示禁止输出
*			  _ulDutyCycle : PWM信号占空比，单位: 万分之一。如5000，表示50.00%的占空比
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetTIMOutPWM(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t _ucChannel,
	 uint32_t _ulFreq, uint32_t _ulDutyCycle)
{									//GRB
	static uint16_t pwm_led4[240 +12 * 24]  = {	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30,
												30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30, 30,30,30,30,30,30,30,30
										};

	TIM_HandleTypeDef TimHandle = {0};
	TIM_OC_InitTypeDef sConfig = {0};	
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t pulse;
	uint32_t uiTIMxCLK;
	const uint16_t TimChannel[6+1] = {0, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4};

	if (_ucChannel > 6)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	
	if (_ulDutyCycle == 0)
	{		
		//bsp_RCC_TIM_Disable(TIMx);		/* 关闭TIM时钟, 可能影响其他通道 */		
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */				
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);	/* PWM = 0 */		
		return;
	}
	else if (_ulDutyCycle == 10000)
	{
		//bsp_RCC_TIM_Disable(TIMx);		/* 关闭TIM时钟, 可能影响其他通道 */
		bsp_ConfigGpioOut(GPIOx, GPIO_Pin);	/* 配置GPIO为推挽输出 */		
		HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);	/* PWM = 1 */			
		return;
	}
	
	/* 下面是PWM输出 */
	
	bsp_ConfigTimGpio(GPIOx, GPIO_Pin, TIMx);	/* 使能GPIO和TIM时钟，并连接TIM通道到GPIO */
	
	if(TIMx == TIM3)
		MX_DMA_Init();
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器时钟 = 168M */
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		/* APB1 定时器 = 84M */
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}
	pulse = (_ulDutyCycle * usPeriod) / 10000;

	if(TIMx == TIM3)
		HAL_TIM_PWM_DeInit(&g_TimHandle);
	else
		HAL_TIM_PWM_DeInit(&TimHandle);
    if(TIMx == TIM3)
		{
			/*  PWM频率 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）*/
			g_TimHandle.Instance = TIMx;
			g_TimHandle.Init.Prescaler         = usPrescaler;
			g_TimHandle.Init.Period            = usPeriod;
			g_TimHandle.Init.ClockDivision     = 0;
			g_TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
			g_TimHandle.Init.RepetitionCounter = 0;
			g_TimHandle.Init.AutoReloadPreload = 0;
		}
	else
		{	/*  PWM频率 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）*/
			TimHandle.Instance = TIMx;
			TimHandle.Init.Prescaler         = usPrescaler;
			TimHandle.Init.Period            = usPeriod;
			TimHandle.Init.ClockDivision     = 0;
			TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
			TimHandle.Init.RepetitionCounter = 0;
			TimHandle.Init.AutoReloadPreload = 0;
		}
	if(TIMx == TIM3)
		{	
			
			if (HAL_TIM_PWM_Init(&g_TimHandle) != HAL_OK)
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
			
		}
	else
		{
			if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
		}


	/* 配置定时器PWM输出通道 */
	sConfig.OCMode       = TIM_OCMODE_PWM1;
	sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
	sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
	sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

	/* 占空比 */
	sConfig.Pulse = pulse;
	if(TIMx == TIM3)
		{
			if (HAL_TIM_PWM_ConfigChannel(&g_TimHandle, &sConfig, TimChannel[_ucChannel]) != HAL_OK)
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
		}
	else
		{
			if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TimChannel[_ucChannel]) != HAL_OK)
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
		}

	
	/* 启动PWM输出 */
	if(TIMx == TIM3)
		{
			if (HAL_TIM_PWM_Start_DMA(&g_TimHandle,TimChannel[_ucChannel],(uint32_t*)pwm_led4,sizeof(pwm_led4)/sizeof(pwm_led4[0])) != HAL_OK)//以DMA模式开启PWM生成
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
		}
	else
		{
			if (HAL_TIM_PWM_Start(&TimHandle, TimChannel[_ucChannel]) != HAL_OK)
				{
					printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
				}
		}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_SetTIMforInt
*	功能说明: 配置TIM和NVIC，用于简单的定时中断，开启定时中断。另外注意中断服务程序需要由用户应用程序实现。
*	形    参: TIMx : 定时器
*			  _ulFreq : 定时频率 （Hz）。 0 表示关闭。
*			  _PreemptionPriority : 抢占优先级
*			  _SubPriority : 子优先级
*	返 回 值: 无
*********************************************************************************************************
*/
/*	
TIM定时中断服务程序范例，必须清中断标志
void TIM6_DAC_IRQHandler(void)
{
	if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
	{
		TIM6->SR = ~ TIM_FLAG_UPDATE;
		//添加用户代码
	}
}
*/
void bsp_SetTIMforInt(TIM_TypeDef* TIMx, uint32_t _ulFreq, uint8_t _PreemptionPriority, uint8_t _SubPriority)
{
	TIM_HandleTypeDef   TimHandle = {0};
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	/* 使能TIM时钟 */
	bsp_RCC_TIM_Enable(TIMx);
	
	/*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器时钟 = 168M */
		uiTIMxCLK = SystemCoreClock;
	}
	else	
	{
		/* APB1 定时器 = 84M */
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (_ulFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else if (_ulFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / _ulFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / _ulFreq - 1;	/* 自动重装的值 */
	}

	/* 
       定时器中断更新周期 = TIMxCLK / usPrescaler + 1）/usPeriod + 1）
	*/
	TimHandle.Instance = TIMx;
	TimHandle.Init.Prescaler         = usPrescaler;
	TimHandle.Init.Period            = usPeriod;	
	TimHandle.Init.ClockDivision     = 0;
	TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	TimHandle.Init.RepetitionCounter = 0;
	TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}

	/* 使能定时器中断  */
	__HAL_TIM_ENABLE_IT(&TimHandle, TIM_IT_UPDATE);
	

	/* 配置TIM定时更新中断 (Update) */
	{
        uint8_t irq = 0;	/* 中断号, 定义在 stm32h7xx.h */

        if (TIMx == TIM1) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM2) irq = TIM2_IRQn;
        else if (TIMx == TIM3) irq = TIM3_IRQn;
        else if (TIMx == TIM4) irq = TIM4_IRQn;
        else if (TIMx == TIM5) irq = TIM5_IRQn;
        else if (TIMx == TIM6) irq = TIM6_DAC_IRQn;
        else if (TIMx == TIM7) irq = TIM7_IRQn;
        else if (TIMx == TIM8) irq = TIM8_UP_TIM13_IRQn;
		else if (TIMx == TIM9) irq = TIM1_BRK_TIM9_IRQn;
        else if (TIMx == TIM10) irq = TIM1_UP_TIM10_IRQn;
        else if (TIMx == TIM11) irq =  TIM1_TRG_COM_TIM11_IRQn;
        else if (TIMx == TIM12) irq = TIM8_BRK_TIM12_IRQn;
        else if (TIMx == TIM13) irq = TIM8_UP_TIM13_IRQn;
        else if (TIMx == TIM14) irq = TIM8_TRG_COM_TIM14_IRQn;
        else
        {
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
        }	
        HAL_NVIC_SetPriority((IRQn_Type)irq, _PreemptionPriority, _SubPriority);
        HAL_NVIC_EnableIRQ((IRQn_Type)irq);		
	}
	
	HAL_TIM_Base_Start(&TimHandle);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
