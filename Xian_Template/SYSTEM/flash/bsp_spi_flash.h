#ifndef _BSP_SPI_FLASH_H
#define _BSP_SPI_FLASH_H

#include "stm32f407xx.h"

#define SF_MAX_PAGE_SIZE    (4 * 1024)

/* 定义串行Flash ID */
/* 查看芯片数据手册 */
enum
{
    SST25VF016B_ID  = 0xBF2541,
    MX25L1606E_ID   = 0xC22015,
    W25164BV_ID     = 0xEF4017,
    W25Q128_ID      = 0xEF4018
};


#endif

