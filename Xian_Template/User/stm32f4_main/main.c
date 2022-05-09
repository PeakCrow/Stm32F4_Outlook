/*******************************************************************************
  * @file        main.c
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      TRX
  * @date        2022-4-25
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-4-25  TRX Created
*******************************************************************************/
#include "sys.h"
#include "ticktim.h"
#include <stdio.h>
#include "bsp_usart_fifo.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_key.h"
#include "bsp_spi_flash_demo.h"
#include "bsp_spi_bus.h"
#include "bsp_iic_bus.h"


#if	DEBUG_SWITCH_EN == 1
#include "EventRecorder.h"
#endif


/*******************************************************************************
  * @FunctionName: main
  * @Author:       trx
  * @DateTime:     2022骞?鏈?5鏃0:45:22 
  * @Purpose:      
  * @param:        void               
  * @return:
*******************************************************************************/
int main(void)
{
	uint8_t ucKeyCode;							/* 按键代码 */
	HAL_Init();                    	 			/* 初始化HAL库 */   
	Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */
	bsp_InitTimer();							/* 初始化滴答定时器 */
	bsp_InitUart();								/* 初始化串口1外设 */
	bsp_InitKey();
	bsp_InitSPI1Bus();							/* SPI总线初始化 */
	bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
	bsp_I2C_EE_Init();							/* 初始化IIC1总线(包含AT24C02芯片) */
#if	DEBUG_SWITCH_EN == 1
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
#endif

	while (1)
		{
			ucKeyCode = bsp_GetKey();	/* 读取键值, 无键按下时返回 KEY_NONE = 0 */
			if (ucKeyCode != KEY_NONE)
			{
				switch (ucKeyCode)
				{
					case KEY_0_DOWN:			/* K1键按下(长按超过1s时，连发按下) */
						printf("K0键按下\r\n");
						break;
					case KEY_0_UP:				/* K1键弹起 */
						printf("K0键弹起\r\n");
						break;
					case KEY_0_LONG:			/* K1键长按(一次) */
						printf("K0键长按\r\n");
						break;
					case KEY_UP_DOWN:			/* Kup键按下(长按超过1s时，连发按下) */
						printf("KUP键按下\r\n");
						break;
					case KEY_UP_UP:				/* Kup键弹起 */
						printf("KUP键弹起\r\n");
						break;
					case KEY_UP_LONG:			/* Kup键长按(一次) */
						printf("kup键长按\r\n");
						break;					/* 组合按键长按(一次) */
					case KEY_MULTI_LONG:
						printf("组合键长按\r\n");
						break;
					case KEY_MULTI_DOWM:		/* 组合按键按下(长按超过1s时，连发按下) */
						printf("组合键按下\r\n");
						break;
					case KEY_MULTI_UP:			/* 组合按键弹起 */
						printf("组合按键弹起\r\n");
						break;
					default:
						/* 其它的键值不处理 */
						break;
				}
			}
		}
}

void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();
}

