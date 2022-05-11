/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V2.0
* | Date        :   2018-10-30
* | Info        :
# ******************************************************************************
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
#include "DEV_Config.h"


static void GPIO_INK_Init(void);


/*******************************************************************************
  * @FunctionName: GPIO_INK_Init
  * @Author:       trx
  * @DateTime:     2022年5月10日 18:59:21 
  * @Purpose:      除去spi通讯以外的控制引脚的初始化
  * @param:        void
  * @return:       none
*******************************************************************************/
static void GPIO_INK_Init(void)
{
	GPIO_InitTypeDef gpio_initstruct = {0};

	/* 使能引脚时钟 */
	INK_BUSY_CLK_ENABLE();
	INK_RST_CLK_ENABLE();
	INK_DC_CLK_ENABLE();
	INK_CS_CLK_ENABLE();

	/* 配置引脚的输出等级 */
	//HAL_GPIO_WritePin(INK_BUSY_GPIO_PORT,INK_BUSY_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_RST_GPIO_PORT,INK_RST_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_DC_GPIO_PORT,INK_DC_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_CS_GPIO_PORT,INK_CS_GPIO_PIN,GPIO_PIN_RESET);
	

	/* 配置引脚模式 */
	/* RST PIN */
	gpio_initstruct.Pin		 = INK_RST_GPIO_PIN;
	gpio_initstruct.Mode	 = GPIO_MODE_OUTPUT_PP;
	gpio_initstruct.Pull	 = GPIO_NOPULL;
	gpio_initstruct.Speed	 = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(INK_RST_GPIO_PORT,&gpio_initstruct);

	/* DC PIN */
	gpio_initstruct.Pin		 = INK_DC_GPIO_PIN;
	HAL_GPIO_Init(INK_DC_GPIO_PORT,&gpio_initstruct);

	/* CS PIN */
	gpio_initstruct.Pin		 = INK_CS_GPIO_PIN;
	HAL_GPIO_Init(INK_CS_GPIO_PORT,&gpio_initstruct);

	/* BUSY PIN */
	gpio_initstruct.Pin		 = INK_BUSY_GPIO_PIN;
	gpio_initstruct.Mode	 = GPIO_MODE_INPUT;
	HAL_GPIO_Init(INK_BUSY_GPIO_PORT,&gpio_initstruct);
	
}

void DEV_SPI_WriteByte(UBYTE value)
{
	bsp_spi2Transfer(value);
}

int DEV_Module_Init(void)
{
	GPIO_INK_Init();
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_Digital_Write(EPD_RST_PIN, 1);
		return 0;
}

void DEV_Module_Exit(void)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);

    //close 5V
    DEV_Digital_Write(EPD_RST_PIN, 0);
}

