#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f407xx.h"
#include "bsp_spi_bus.h"
#include "stdlib.h"
#include "string.h"
#include "sys.h"

#define SF_MAX_PAGE_SIZE    (4 * 1024)

/* 定义串行Flash ID */
/* 查看芯片数据手册 */
enum
{
    SST25VF016B_ID  = 0xBF2541,
    MX25L1606E_ID   = 0xC22015,
    W25Q64BV_ID     = 0xEF4017,
    W25Q128FV_ID	= 0xEF4018,
    N25Q128_ID      = 0x522118
};

typedef struct
{
	uint32_t 	ChipID;			/* 芯片ID */
	char 		ChipName[16];	/* 芯片型号字符串，主要用于显示 */
	uint32_t 	TotalSize;		/* 总容量 */
	uint16_t 	SectorSize;		/* 扇区大小 */
}SFLASH_T;

void bsp_InitSFlash(void);
uint32_t sf_ReadID(void);
void sf_EraseChip(void);
void sf_EraseSector(uint32_t _uiSectorAddr);
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize);
uint8_t sf_WriteBuffer(uint8_t* _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize);
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize);
void sf_ReadInfo(void);

extern SFLASH_T g_tSF;



#endif

