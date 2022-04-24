#ifndef __BSP_SPI_BUS_H
#define __BSP_SPI_BUS_H

#include "stm32f4xx.h"
#include "stdio.h"


/* 最小擦除单位(扇区)--4k */
#define SPI_BUFFER_SIZE		(4 * 1024)

/* 重定义SPI SCK时钟，方便移植 */
#define SPI_BAUDRATEPRESCALER_100M		SPI_BAUDRATEPRESCALER_2
#define	SPI_BAUDRATEPRESCALER_50M		SPI_BAUDRATEPRESCALER_4
#define SPI_BAUDRATEPRESCALER_12_5M		SPI_BAUDRATEPRESCALER_8
#define SPI_BAUDRATEPRESCALER_6_25M		SPI_BAUDRATEPRESCALER_16
#define SPI_BAUDRATEPRESCALER_3_125M	SPI_BAUDRATEPRESCALER_32
#define SPI_BAUDRATEPRESCALER_1_5625M	SPI_BAUDRATEPRESCALER_64
#define SPI_BAUDRATEPRESCALER_781_25K	SPI_BAUDRATEPRESCALER_128
#define SPI_BAUDRATEPRESCALER_390_625K	SPI_BAUDRATEPRESCALER_256


void bsp_InitSPIBus(void);
void bsp_spiTransfer(void);
void bsp_InitSPIParam(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity);

void bsp_SpiBusEnter(void);
void bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);


extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
extern uint32_t g_spiLen;

extern uint8_t g_spi_busy;



#endif




