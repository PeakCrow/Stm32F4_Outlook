#include "bsp_iic_bus.h"





static void I2C_Mode_Config(void);


/*******************************************************************************
  * @FunctionName: I2C_Mode_Config
  * @Author:       trx
  * @DateTime:     2022年5月5日 12:46:56 
  * @Purpose:      iic工作模式配置
  * @param:        void               
  * @return:       none
*******************************************************************************/
static void I2C_Mode_Config(void)
{
	I2C_HandleTypeDef	iic_handle;
	iic_handle.Init.AddressingMode		= I2C_ADDRESSINGMODE_7BIT;
	iic_handle.Init.ClockSpeed			= 400000;
	iic_handle.Init.DualAddressMode		= I2C_DUALADDRESS_DISABLE;
	iic_handle.Init.DutyCycle			= I2C_DUTYCYCLE_2;
	iic_handle.Init.GeneralCallMode		= I2C_GENERALCALL_DISABLE;
	iic_handle.Init.NoStretchMode		= I2C_NOSTRETCH_DISABLE;
	iic_handle.Init.OwnAddress1			= I2C_OWN_ADDRESS7;
	iic_handle.Init.OwnAddress2			= 0;

	/* 初始化I2C */
	HAL_I2C_Init(&iic_handle);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef * hi2c)
{
	GPIO_InitTypeDef	gpio_initstruct;

	/* 初始化iic通讯的引脚时钟 */
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();

	/* 使能iic总线时钟 */
	I2Cx_CLK_ENABLE();

	/* 配置iic总线通讯引脚 */
	gpio_initstruct.Pin			= I2Cx_SCL_PIN;
	gpio_initstruct.Mode		= GPIO_MODE_AF_OD;
	gpio_initstruct.Pull		= GPIO_NOPULL;
	gpio_initstruct.Speed		= GPIO_SPEED_FAST;
	gpio_initstruct.Alternate	= I2Cx_SCL_AF;
	
}

