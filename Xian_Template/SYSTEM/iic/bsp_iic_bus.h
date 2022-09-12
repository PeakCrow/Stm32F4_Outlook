/*******************************************************************************
  * @file        bsp_iic_bus.h
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      TRX
  * @date        2022-5-3
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-5-3  TRX Created
*******************************************************************************/
#ifndef __BSP_IIC_BUS_H_
#define __BSP_IIC_BUS_H_

#include 	"stm32f4xx.h"
#include	"stdio.h"

#define		I2C_OWN_ADDRESS7		0x0A	/* stm32自身的IIC器件地址 */
#define		EEP_Firstpage				0x00	/* eeprom芯片数据起始地址 */

/* I2C引脚--PB8		PB9 */
#define		I2Cx						I2C1
#define		I2Cx_CLK_ENABLE()			__HAL_RCC_I2C1_CLK_ENABLE()
#define		I2Cx_SDA_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define		I2Cx_SCL_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()

#define		I2Cx_FORCE_RESET()			__HAL_RCC_I2C1_FORCE_RESET()
#define		I2Cx_RELEASE_RESET()		__HAL_RCC_I2C1_RELEASE_RESET()


#define		I2Cx_SCL_PIN				GPIO_PIN_8
#define		I2Cx_SCL_GPIO_PORT			GPIOB
#define		I2Cx_SCL_AF					GPIO_AF4_I2C1
#define		I2Cx_SDA_PIN				GPIO_PIN_9
#define		I2Cx_SDA_GPIO_PORT			GPIOB
#define		I2Cx_SDA_AF					GPIO_AF4_I2C1

#define		I2CT_FLAG_TIMEOUT			((uint32_t)0x1000)		/* 等待超时时间 */
#define		I2CT_LONG_TIMEOUT			((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define		EEPROM_PAGESIZE				8
#define		I2Cx_TIMEOUT_MAX			300
#define		I2Cx_MAX_TRIALS				300
#define		I2Cx_MAX_BYTE_NUM			256

extern uint8_t		I2Cx_BUFFER_WRITE[I2Cx_MAX_BYTE_NUM];
extern uint8_t		I2Cx_BUFFER_READ[I2Cx_MAX_BYTE_NUM];
extern I2C_HandleTypeDef	iic_handle;


/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
#define		EEPROM_ADDRESS				0xA0	/* IIC从机地址 */

#define		EEPROM_Block0_ADDRESS		0xA0
#define		EEPROM_Block1_ADDRESS		0xA2
#define		EEPROM_Block2_ADDRESS		0xA4
#define		EEPROM_Block3_ADDRESS		0xA6


void bsp_I2C_EE_Init(void);
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);
uint32_t I2C_EE_ByteWrite(uint8_t* pBuffer,uint8_t WriteAddr);
uint32_t I2C_EE_PageWrite(uint8_t* pBuffer,uint8_t WriteAddr,uint8_t NumByteToWrite);
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer,uint8_t ReadAdder,uint16_t NumByteToRead);
void I2C_EE_BufferWrite(uint8_t * pBuffer,uint8_t WriteAddr,uint16_t NumByteToWrite);



void DemoIicEeprom(void);



#endif


