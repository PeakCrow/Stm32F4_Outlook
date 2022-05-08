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
#include "EventRecorder.h"
#include <stdio.h>
#include "bsp_usart_fifo.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_key.h"
#include "bsp_spi_flash_demo.h"
#include "bsp_spi_bus.h"
#include "bsp_iic_bus.h"

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
		HAL_Init();                    	 			/* 初始化HAL库 */   
		Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */
		bsp_InitTimer();							/* 初始化滴答定时器 */
		bsp_InitUart();								/* 初始化串口1外设 */
		bsp_InitKey();
		bsp_InitSPIBus();							/* SPI总线初始化 */
		bsp_InitSFlash();							/* 初始化SPI FLASH芯片 */
		bsp_I2C_EE_Init();							/* 初始化IIC1总线(包含AT24C02芯片) */
		EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
		

		DemoIicEeprom();
}

