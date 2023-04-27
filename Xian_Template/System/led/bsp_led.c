/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2019-04-24 armfly  正式发布
*
*	Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp_led.h"


/*
	安富莱STM32-V5 开发板LED口线分配：
		LD1     : PI10/TP_NCS          (低电平点亮，高电平熄灭)
		LD2     : PF7/NRF24L01_CSN     (低电平点亮，高电平熄灭)
		LD3     : PF8/SF_CS            (低电平点亮，高电平熄灭)
		LD4     : PC2/NRF905_CSN/VS1053_XCS  (低电平点亮，高电平熄灭)
*/

/* LED口对应的RCC时钟 */
#define LED_GPIO_CLK_ALLENABLE() __HAL_RCC_GPIOF_CLK_ENABLE();



#define GPIO_PORT_LED1  GPIOF
#define GPIO_PIN_LED1	GPIO_PIN_9

#define GPIO_PORT_LED2  GPIOF
#define GPIO_PIN_LED2	GPIO_PIN_10



/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* 打开GPIO时钟 */
	LED_GPIO_CLK_ALLENABLE();
	
	/*
		配置所有的LED指示灯GPIO为推挽输出模式
		由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
		这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
	*/
	bsp_LedOff(1);
	bsp_LedOn(2);


	/* 配置LED */
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   		/* 设置推挽输出 */
	GPIO_InitStruct.Pull = GPIO_NOPULL;                 /* 上下拉电阻不使能 */
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  		/* GPIO速度等级 */

	GPIO_InitStruct.Pin = GPIO_PIN_LED1;
	HAL_GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_LED2;
	HAL_GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStruct);
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRR = (uint32_t)GPIO_PIN_LED1 << 16U;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRR = (uint32_t)GPIO_PIN_LED2 << 16U;
	}

}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRR = GPIO_PIN_LED2;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
	if (_no == 1)
	{
		GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED2->ODR ^= GPIO_PIN_LED2;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 1)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_LED2->ODR & GPIO_PIN_LED2) == 1)
		{
			return 1;
		}
		return 0;
	}

	return 0;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
