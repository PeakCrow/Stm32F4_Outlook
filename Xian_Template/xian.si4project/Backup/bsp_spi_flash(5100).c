/*******************************************************************************
  * @file        bsp_spi_flash.c
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      trx
  * @date        2022-4-24
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-4-24  trx Created
*******************************************************************************/
#include "bsp_spi_flash.h"


/* 串行Flash的片选GPIO端口， PB14  */
#define SF_CS_CLK_ENABLE() 			__HAL_RCC_GPIOF_CLK_ENABLE()
#define SF_CS_GPIO					GPIOB
#define SF_CS_PIN					GPIO_PIN_14

#define SF_CS_0()					SF_CS_GPIO->BSRR = ((uint32_t)SF_CS_PIN << 16U) 
#define SF_CS_1()					SF_CS_GPIO->BSRR = SF_CS_PIN
	
#define CMD_AAI       0xAD  	/* AAI 连续编程指令(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* 禁止写, 退出AAI状态 */
#define CMD_EWRSR	  0x50		/* 允许写状态寄存器的命令 */
#define CMD_WRSR      0x01  	/* 写状态寄存器命令 */
#define CMD_WREN      0x06		/* 写使能命令 */
#define CMD_READ      0x03  	/* 读数据区命令 */
#define CMD_RDSR      0x05		/* 读状态寄存器命令 */
#define CMD_RDID      0x9F		/* 读器件ID命令 */
#define CMD_SE        0x20		/* 擦除扇区命令 */
#define CMD_BE        0xC7		/* 批量擦除命令 */
#define DUMMY_BYTE    0xA5		/* 哑命令，可以为任意值，用于读操作 */

#define WIP_FLAG      0x01		/* 状态寄存器中的正在编程标志（WIP) */

SFLASH_T g_tSF;

#if 0
static void sf_WriteStatus(uint8_t _ucValue);
#endif

static void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _uiLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
static uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

static uint8_t s_spiBuf[4*1024];	/* 用于写函数，先读出整个扇区，修改缓冲区后，再整个扇区回写 */

/*
*********************************************************************************************************
*	函 数 名: sf_SetCS
*	功能说明: 串行FALSH片选控制函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_SetCS(uint8_t _Level)
{
	if (_Level == 0)
	{
		bsp_SpiBusEnter();	
		bsp_InitSPIParam(SPI_BAUDRATEPRESCALER_2, SPI_PHASE_1EDGE, SPI_POLARITY_LOW);		
		SF_CS_0();
	}
	else
	{		
		SF_CS_1();	
		bsp_SpiBusExit();		
	}
}

/*
*********************************************************************************************************
*	函 数 名: sf_WaitForWriteEnd
*	功能说明: 采用循环查询的方式等待器件内部写操作完成
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	sf_SetCS(0);									/* 使能片选 */
	g_spiTxBuf[0] = (CMD_RDSR);						/* 发送命令， 读状态寄存器 */
	g_spiLen = 2;
	bsp_spiTransfer();	
	sf_SetCS(1);									/* 禁能片选 */
	
	while(1)
	{
		sf_SetCS(0);									/* 使能片选 */
		g_spiTxBuf[0] = (CMD_RDSR);						/* 发送命令， 读状态寄存器 */
		g_spiTxBuf[1] = 0;								/* 无关数据 */
		g_spiLen = 2;
		bsp_spiTransfer();	
		sf_SetCS(1);									/* 禁能片选 */
		
		if ((g_spiRxBuf[1] & WIP_FLAG) != SET)			/* 判断状态寄存器的忙标志位 */
		{
			break;
		}		
		}
}


/*
*********************************************************************************************************
*	函 数 名: sf_WriteEnable
*	功能说明: 向器件发送写使能命令
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WriteEnable(void)
{
	sf_SetCS(0);									/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_WREN);			/* 发送命令 */
	bsp_spiTransfer();
	sf_SetCS(1);									/* 禁能片选 */
}



/*
*********************************************************************************************************
*	函 数 名: sf_ReadInfo
*	功能说明: 读取器件ID,并填充器件参数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void sf_ReadInfo(void)
{
	/* 自动识别串行Flash型号 */
	{
		g_tSF.ChipID = sf_ReadID();	/* 芯片ID */

		switch (g_tSF.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tSF.ChipName, "SST25VF016B");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tSF.ChipName, "MX25L1606E");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* 总容量 = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tSF.ChipName, "W25Q64");
				g_tSF.TotalSize = 8 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;
			
			case W25Q128_ID:
				strcpy(g_tSF.ChipName, "W25Q128");
				g_tSF.TotalSize = 16 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;			

			default:
				strcpy(g_tSF.ChipName, "Unknow Flash");
				g_tSF.TotalSize = 2 * 1024 * 1024;
				g_tSF.SectorSize = 4 * 1024;
				break;
		}
	}
}


/*
*********************************************************************************************************
*	函 数 名: sf_ReadID
*	功能说明: 读取器件ID
*	形    参:  无
*	返 回 值: 32bit的器件ID (最高8bit填0，有效ID位数为24bit）
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_SetCS(0);							/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[0] = (CMD_RDID);				/* 发送读ID命令 */
	g_spiLen = 4;
	bsp_spiTransfer();
	
	id1 = g_spiRxBuf[1];					/* 读ID的第1个字节 */
	id2 = g_spiRxBuf[2];					/* 读ID的第2个字节 */
	id3 = g_spiRxBuf[3];					/* 读ID的第3个字节 */
	sf_SetCS(1);							/* 禁能片选 */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;

	return uiID;
}


/*
*********************************************************************************************************
*	函 数 名: sf_EraseChip
*	功能说明: 擦除整个芯片
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_EraseChip(void)
{	
	sf_WriteEnable();								/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_SetCS(0);		/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = CMD_BE;				/* 发送整片擦除命令 */
	bsp_spiTransfer();
	sf_SetCS(1);									/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSFlash
*	功能说明: 串行falsh硬件初始化。 配置CS GPIO， 读取ID。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{
	/* 配置CS GPIO */
	{
		GPIO_InitTypeDef gpio_init;

		/* 打开GPIO时钟 */
		SF_CS_CLK_ENABLE();
		
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* 设置推挽输出 */
		gpio_init.Pull = GPIO_NOPULL;			/* 上下拉电阻不使能 */
		gpio_init.Speed = GPIO_SPEED_HIGH;  	/* GPIO速度等级 */	
		gpio_init.Pin = SF_CS_PIN;	
		HAL_GPIO_Init(SF_CS_GPIO, &gpio_init);	
	}
	
	/* 读取芯片ID, 自动识别芯片型号 */
	sf_ReadInfo();
}	


