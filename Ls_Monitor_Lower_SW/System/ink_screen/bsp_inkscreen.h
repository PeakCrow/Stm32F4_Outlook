#ifndef _BSP_INKSREEN_H_
#define _BSP_INKSREEN_H_

#include "sys.h"

/* Display resolution */
#define INKSCREEN_WIDTH       128
#define INKSCREEN_HEIGHT      296


/* command table */
#define INKSCREEN_BW		0x10
#define INKSCREEN_RED		0x13
#define INKSCREEN_DRF		0x12
#define INKSCREEN_POF		0x02
#define INKSCREEN_DSLP		0x07
#define INKSCREEN_CHC		0xA5
#define INKSCREEN_PTOUT		0x92



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


/* 墨水屏驱动延时函数 */
#define inkscreen_delay_ms(__xms) tx_thread_sleep(__xms)


void bsp_InkscreenInit(void);
void bsp_InkscreenExit(void);
void bsp_InkscreenReset(void);
void bsp_InkscreeenClear(void);
void bsp_InkscreeenSleep(void);
void bsp_InkscreenDisplay(const uint8_t *_blackimage,const uint8_t * _ryimage);


#endif





