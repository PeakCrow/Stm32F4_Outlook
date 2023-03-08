/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V2.0
* | Date        :   2018-10-30
* | Info        :
* 1.add:
*   UBYTE\UWORD\UDOUBLE
* 2.Change:
*   EPD_RST -> EPD_RST_PIN
*   EPD_DC -> EPD_DC_PIN
*   EPD_CS -> EPD_CS_PIN
*   EPD_BUSY -> EPD_BUSY_PIN
* 3.Remote:
*   EPD_RST_1\EPD_RST_0
*   EPD_DC_1\EPD_DC_0
*   EPD_CS_1\EPD_CS_0
*   EPD_BUSY_1\EPD_BUSY_0
* 3.add:
*   #define DEV_Digital_Write(_pin, _value) bcm2835_gpio_write(_pin, _value)
*   #define DEV_Digital_Read(_pin) bcm2835_gpio_lev(_pin)
*   #define DEV_SPI_WriteByte(__value) bcm2835_spi_transfer(__value)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "bsp_spi_bus.h"
//#include "ticktim.h"

/* BUSY  PA1 */
#define	INK_BUSY_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define	INK_BUSY_GPIO_PORT			GPIOA
#define	INK_BUSY_GPIO_PIN			GPIO_PIN_1
#define	INK_BUSY_GPIO_READ			PAin(1)

/* RST  PA7 */
#define INK_RST_CLK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()
#define	INK_RST_GPIO_PORT			GPIOA
#define	INK_RST_GPIO_PIN			GPIO_PIN_7
#define	INK_RST_GPIO_WRITE			PAout(7)

/* DC  PB12 */
#define INK_DC_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE()
#define	INK_DC_GPIO_PORT			GPIOB
#define	INK_DC_GPIO_PIN				GPIO_PIN_12
#define	INK_DC_GPIO_WRITE			PBout(12)

/* CS  PC0 */
#define	INK_CS_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE()
#define	INK_CS_GPIO_PORT			GPIOC
#define	INK_CS_GPIO_PIN				GPIO_PIN_0
#define	INK_CS_GPIO_WRITE			PCout(0)


/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/**
 * e-Paper GPIO
**/
#define EPD_RST_PIN     INK_RST_GPIO_PORT, INK_RST_GPIO_PIN
#define EPD_DC_PIN      INK_DC_GPIO_PORT, INK_DC_GPIO_PIN
#define EPD_CS_PIN      INK_CS_GPIO_PORT, INK_CS_GPIO_PIN
#define EPD_BUSY_PIN    INK_BUSY_GPIO_PORT, INK_BUSY_GPIO_PIN


/**
 * GPIO read and write
**/
#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)

/**
 * delay x ms
**/
//#define DEV_Delay_ms(__xms) bsp_DelayMs(__xms);
#define DEV_Delay_ms(__xms) tx_thread_sleep(__xms);

void DEV_SPI_WriteByte(UBYTE value);

int DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif


