#ifndef __BSP_SPI_BUS_H_
#define __BSP_SPI_BUS_H_

#include "stm32f4xx.h"
#include "stdio.h"


/* 最小擦除单位(扇区)--4k */
#define SPI_BUFFER_SIZE		(4 * 1024)

/* 重定义SPI SCK时钟，方便移植 */
/* APB2总线84Mhz，经过分频后的速度重新进行宏定义 */
#define SPI_BAUDRATEPRESCALER_42M				SPI_BAUDRATEPRESCALER_2
#define	SPI_BAUDRATEPRESCALER_21M				SPI_BAUDRATEPRESCALER_4
#define SPI_BAUDRATEPRESCALER_10_5M				SPI_BAUDRATEPRESCALER_8
#define SPI_BAUDRATEPRESCALER_5_25M				SPI_BAUDRATEPRESCALER_16
#define SPI_BAUDRATEPRESCALER_2_625M			SPI_BAUDRATEPRESCALER_32
#define SPI_BAUDRATEPRESCALER_1_3125M			SPI_BAUDRATEPRESCALER_64
#define SPI_BAUDRATEPRESCALER_656_25K			SPI_BAUDRATEPRESCALER_128
#define SPI_BAUDRATEPRESCALER_328_125K			SPI_BAUDRATEPRESCALER_256	/* 默认波特率 */

/* 重定义SPI2 CLK时钟，方便移植 */
/* APB1总线42Mhz，经过分频后的速度重新进行宏定义 */
#define	SPI_APB1_BAUDRATEPRESCALER_21M			SPI_BAUDRATEPRESCALER_2
#define	SPI_APB1_BAUDRATEPRESCALER_10_5M		SPI_BAUDRATEPRESCALER_4
#define SPI_APB1_BAUDRATEPRESCALER_5_25M		SPI_BAUDRATEPRESCALER_8
#define	SPI_APB1_BAUDRATEPRESCALER_2_625M		SPI_BAUDRATEPRESCALER_16
#define	SPI_APB1_BAUDRATEPRESCALER_1_315M		SPI_BAUDRATEPRESCALER_32
#define	SPI_APB1_BAUDRATEPRESCALER_656_25K		SPI_BAUDRATEPRESCALER_64
#define	SPI_APB1_BAUDRATEPRESCALER_328_125K		SPI_BAUDRATEPRESCALER_128
#define	SPI_APB1_BAUDRATEPRESCALER_164_0625K	SPI_BAUDRATEPRESCALER_256	/* 默认波特率 */

//#define USE_SPI_DMA		/* DMA方式 */
#define USE_SPI_INT		/* 中断方式 */
//#define USE_SPI_POLL		/* 查询方式 */

void bsp_InitSPI1Bus(void);
void bsp_spi1Transfer(void);
void bsp_InitSPI1Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity);

void bsp_Spi1BusEnter(void);
void bsp_Spi1BusExit(void);
uint8_t bsp_SpiBusBusy(void);

void bsp_InitSPI2Bus(void);
void bsp_InitSPI2Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity);
void bsp_spi2Transfer(uint8_t _value);

extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
extern uint32_t g_spiLen;

extern uint8_t g_spi_busy;



#endif




