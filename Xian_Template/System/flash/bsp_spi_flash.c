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

/* 芯片ID 	0x5217   21015 */

/* 串行Flash的片选GPIO端口， PB14  */
#define SF_CS_CLK_ENABLE() 			__HAL_RCC_GPIOB_CLK_ENABLE()
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

void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

//static uint8_t g_spiTxBuf[4*1024];	/* 用于写函数，先读出整个扇区，修改缓冲区后，再整个扇区回写 */

/*******************************************************************************
  * @FunctionName: sf_SetCS
  * @Author:       trx
  * @DateTime:     2022年4月29日 13:13:02 
  * @Purpose:      串行flash片选控制函数√√√
  * @param:        _Level：0：片选；1：禁止片选
  * @return:       none
*******************************************************************************/
void sf_SetCS(uint8_t _Level)
{
	if (_Level == 0)
	{
		bsp_Spi1BusEnter();	
		/* 最高速写flash会出错，所以这里使用次高速 */
		bsp_InitSPI1Param(SPI_BAUDRATEPRESCALER_21M, SPI_PHASE_2EDGE, SPI_POLARITY_HIGH);
		SF_CS_0();
	}
	else
	{		
		SF_CS_1();	
		bsp_Spi1BusExit();		
	}
}


/*
*********************************************************************************************************
*	函 数 名: sf_WriteEnable
*	功能说明: 向器件发送写使能命令√√√
*	形    参: 无 
*	返 回 值: 无
*********************************************************************************************************
*/
void sf_WriteEnable(void)
{
	sf_SetCS(0);									/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_WREN);			/* 发送命令 */
	bsp_spi1Transfer();
	sf_SetCS(1);									/* 禁能片选 */
}
/*
*********************************************************************************************************
*	函 数 名: sf_WaitForWriteEnd
*	功能说明: 采用循环查询的方式等待器件内部写操作完成
*	形    参: 无	√√√
*	返 回 值: 无
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	sf_SetCS(0);									/* 使能片选 */
	g_spiTxBuf[0] = (CMD_RDSR);						/* 发送命令， 读状态寄存器 */
	g_spiLen = 2;
	bsp_spi1Transfer();	
	sf_SetCS(1);									/* 禁能片选 */
	
	while(1)
	{
		sf_SetCS(0);								/* 使能片选 */
		g_spiTxBuf[0] = (CMD_RDSR);					/* 发送命令， 读状态寄存器 */
		g_spiTxBuf[1] = 0;							/* 无关数据 */
		g_spiLen = 2;
		bsp_spi1Transfer();	
		sf_SetCS(1);								/* 禁能片选 */
		/* 发送读状态器指令后,判断接收到字节的第0位是否为0 */
		/* 0代表该设备已准备好进一步的指示(通过查数据手册得知) */
		if ((g_spiRxBuf[1] & WIP_FLAG) != SET)		/* 判断状态寄存器的忙标志位 */
		{
			break;
		}		
	}
	//printf("器件内部操作完成\r\n");
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
				strcpy(g_tSF.ChipName, "W25Q64BV");
				g_tSF.TotalSize = 8 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;
			
			case N25Q128_ID:
				strcpy(g_tSF.ChipName, "N25Q128FV");
				g_tSF.TotalSize = 16 * 1024 * 1024;	/* 总容量 = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* 扇区大小 = 4K */
				break;			
			case W25Q128FV_ID:
				strcpy(g_tSF.ChipName,"W25Q128FV");
				g_tSF.TotalSize = 16 * 1024 * 1024;
				g_tSF.SectorSize = 4 * 1024;
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
*	功能说明: 读取器件制造商ID
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
	bsp_spi1Transfer();
	
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
*	功能说明: 擦除整个芯片√√√
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
	bsp_spi1Transfer();
	sf_SetCS(1);									/* 禁能片选 */

	sf_WaitForWriteEnd();							/* 等待串行Flash内部写操作完成 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSFlash
*	功能说明: 串行falsh硬件初始化。 配置CS GPIO片选信号， 读取ID。
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
		gpio_init.Pull = GPIO_PULLUP;			/* 上下拉电阻不使能 */
		gpio_init.Speed = GPIO_SPEED_HIGH;  	/* GPIO速度等级 */	
		gpio_init.Pin = SF_CS_PIN;	
		HAL_GPIO_Init(SF_CS_GPIO, &gpio_init);	
	}
	
	/* 读取芯片ID, 自动识别芯片型号 */
	sf_ReadInfo();
}	
/*******************************************************************************
  * @FunctionName: sf_NeedErase
  * @Author:       trx
  * @DateTime:     2022年4月25日 13:28:19 
  * @Purpose:      判断写PAGE之前是否需要擦除√√√
  * @param:        _ucpOldBuf：旧数据
  * @param:        _ucpNewBuf：新数据
  * @param:        _usLen    ：数据个数
  * @return:		0；不需要擦除；1：需要擦除
*******************************************************************************/
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t * _ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	算法第一步：old求反，new不变
				1101	0101
			~	
			=	0010	0101
	算法第二部：old求反结果位与new
				0010
			&	0101
			=	0000
	算法第三步：结果为0，则表示无需擦除，否则表示需要擦除
	*/

	for (i = 0; i < _usLen; ++i)
		{
			ucOld = *_ucpOldBuf++;
			ucOld = ~ucOld;

			if ((ucOld & (*_ucpNewBuf++)) != 0)
				{
					return 1;
				}
		}
	return 0;
}

/*******************************************************************************
  * @FunctionName: sf_CmpData
  * @Author:       trx
  * @DateTime:     2022年4月25日 14:07:40 
  * @Purpose:      比较flash的数据√√√
  * @param:        _uiSrcAddr：数据缓冲区
  * @param:        _ucpTar     falsh地址
  * @param:        _uiSize     数据个数，不能超出芯片总容量
  * @return:       0：相等；1；不相等；
*******************************************************************************/
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t * _ucpTar, uint32_t _uiSize)
{
	uint16_t i,j;
	uint16_t rem;
	
	/* 如果读取的数据长度为0或者超出串行flash地址空间，则直接返回 */

	if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
		{
			return 	1;
		}

	if (_uiSize == 0)
		{
			return 0;
		}
	sf_SetCS(0);				/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_READ);
	g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff0000) >> 16);
	g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff00) >> 8);
	g_spiTxBuf[g_spiLen++] = (_uiSrcAddr & 0xff);
	bsp_spi1Transfer();

	/* 开始读数据，应为底层DMA缓冲区有限，必须分包读 */
	for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; ++i)
		{
			g_spiLen = SPI_BUFFER_SIZE;
			bsp_spi1Transfer();

			for (j = 0; j < SPI_BUFFER_SIZE; ++j)
				{
					if(g_spiRxBuf[j] != *_ucpTar++)
						{
							goto NOTEQ;			/* 不相等 */
						}
				}
		}
	rem = _uiSize % SPI_BUFFER_SIZE;			/* 剩余字节 */
	if (rem > 0)
		{
			g_spiLen = rem;
			bsp_spi1Transfer();

			for (j = 0; j < rem; ++j)
				{
					if (g_spiRxBuf[j] != *_ucpTar++)
						{
							goto NOTEQ;		/* 不相等 */
						}
				}
		}
	sf_SetCS(1);
	return 0;			/* 相等 */

NOTEQ:
	sf_SetCS(1);
	return 1;			/* 不相等 */
}


/*******************************************************************************
  * @FunctionName: sf_ReadBuffer
  * @Author:       trx
  * @DateTime:     2022年4月25日 14:47:32 
  * @Purpose:      连续读取若干字节，字节个数不能超出芯片数量√√√
  * @param:        _pBuf       数据源缓冲区
  * @param:        _uiReadAddr 首地址
  * @param:        _uiSize     数据个数，不能超出芯片总容量
*******************************************************************************/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	uint16_t rem;
	uint16_t i;

	/* 如果读取的数据长度为0或者超出串行flash地址空间，则直接返回 */
	if ((_uiSize == 0) || (_uiReadAddr + _uiSize) > g_tSF.TotalSize)
		{
			return ;
		}

	/* 擦除扇区操作 */
	sf_SetCS(0);								/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_READ);						/* 发送读指令 */
	g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff0000) >> 16);	/* 发送扇区地址的高8bit */
	g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff00) >> 8);		/* 发送扇区地址的中8bit */
	g_spiTxBuf[g_spiLen++] = (_uiReadAddr & 0xff);				/* 发送扇区地址低8bit */
	bsp_spi1Transfer();			/* 第一次发送0x03 */

	/* 开始读数据，因为底层DMA缓冲区有限，必须分包读 */
	for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; ++i)
		{
			g_spiLen = SPI_BUFFER_SIZE;							/* 每次读取4k大小的扇区 */
			bsp_spi1Transfer();	/* 第二次发送0x03 */
			/* 从存储区g_spiRxBuf复制SPI_BUFFER_SIZE个字节到_pBuf */
			/* 返回一个指向_pBuf存储区的指针 */
			memcpy(_pBuf, g_spiRxBuf,SPI_BUFFER_SIZE);
			_pBuf += SPI_BUFFER_SIZE;							/* 地址运算，将_pBuf指针指向下一个扇区的首地址 */
		}
	rem = _uiSize % SPI_BUFFER_SIZE;			/* 剩余字节，计算要读取的数据是否是整扇区 */

	if (rem > 0)
		{
			g_spiLen = rem;
			bsp_spi1Transfer();
			memcpy(_pBuf,g_spiRxBuf,rem);		/* 将多余的字节再次进行复制 */
		}
	sf_SetCS(1);								/* 禁止片选 */
}
/*******************************************************************************
  * @FunctionName: sf_AutoWriteSector
  * @Author:       trx
  * @DateTime:     2022年4月25日 15:47:44 
  * @Purpose:      写一个扇区并校验，如果不正确则再重写两次，本函数自动完成√√√
  * @param:        _ucpSrc     数据源缓冲区
  * @param:        _uiWrAddr   目标区域首地址
  * @param:        _usWrLen    数据个数，不能超过扇区大小
  * @return:       0：错误；1：成功；
*******************************************************************************/
uint8_t sf_AutoWriteSector(uint8_t * _ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;				/* 用于延时 */
	uint32_t uiFirstAddr;	/* 扇区首地址 */
	uint8_t ucNeedErase;	/* 1表示需要擦除 */
	uint8_t cRet;
	
	/* 长度为0时不继续操作，直接认为成功 */
	if (_usWrLen == 0)
		{
			return 1;
		}

	/* 如果偏移地址超过芯片容量则退出 */
	if (_uiWrAddr >= g_tSF.TotalSize)
		{
			return 0;
		}

	/* 如果数据长度大于扇区容量，则退出 */
	if (_usWrLen > g_tSF.SectorSize)
		{
			return 0;
		}

	/* 如果flash中的数据没有变化，则不写flash */
	sf_ReadBuffer(g_spiTxBuf,_uiWrAddr,_usWrLen);
	if (memcmp(g_spiTxBuf,_ucpSrc,_usWrLen) == 0)
		{
			return 1;
		}

	/* 判断是否需要先擦除扇区 */
	/* 如果旧数据修改为新数据，所有位均是 1->0 或者 0->0,则无需擦除，提高flash寿命 */
	ucNeedErase = 0;			/* 0不需要擦除 */
	if (sf_NeedErase(g_spiTxBuf,_ucpSrc,_usWrLen))
		{
			ucNeedErase = 1;	/* 1需要擦除 */
		}

	uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

	if (_usWrLen == g_tSF.SectorSize)		/* 整个扇区都改写 */
		{
			for (i = 0; i < g_tSF.SectorSize; ++i)
				{
					g_spiTxBuf[i] = _ucpSrc[i];
				}
		}
	else									/* 改写部分数据 */
	{
		/* 先将整个扇区的数据读出 */
		sf_ReadBuffer(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);

		/* 再用新数据覆盖 */
		i = _uiWrAddr & (g_tSF.SectorSize - 1);
		memcpy(&g_spiTxBuf[i],_ucpSrc,_usWrLen);
	}

	/* 写完之后进行校验，如果不正确则重写，最多3次 */
	cRet = 0;
	for (i = 0; i < 3; ++i)
		{
			/* 如果旧数据修改为新数据，所有位均是1->0,或者0->0，则无需擦除，提高flash寿命 */
			if (ucNeedErase == 1)
				{
					sf_EraseSector(uiFirstAddr);	/* 擦除1个扇区 */
				}

			/* 编程一个扇区 */
			sf_PageWrite(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);
			
			if (sf_CmpData(_uiWrAddr,_ucpSrc,_usWrLen) == 0)
				{
					cRet = 1;
					break;
				}
			else
			{
				if (sf_CmpData(_uiWrAddr,_ucpSrc,_usWrLen) == 0)
					{
						cRet = 1;
						break;
					}
				/* 失败后延迟一段时间再重试 */
				for(j = 0;j < 10000;j++);
			}
		}
	return cRet;
}

/*******************************************************************************
  * @FunctionName: sf_WriteBuffer
  * @Author:       trx
  * @DateTime:     2022年4月25日 17:59:58 
  * @Purpose:      写一个扇区并校验，如果不正确则再重写两次，函数自动完成,一个扇区的大小为4K√√√
  * @param:        _pBuf        数据源缓冲区
  * @param:        _uiWriteAddr 目标区域首地址
  * @param:        _usWriteSize  数据个数，不允许超过芯片容量
  * @return:       1：成功；0：失败；
*******************************************************************************/
uint8_t sf_WriteBuffer(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
	uint32_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,temp = 0;

	Addr = _uiWriteAddr % g_tSF.SectorSize;			/* 扇区内的地址 */
	count = g_tSF.SectorSize - Addr;				/* 从起始地址开始要写几个扇区 */
	NumOfPage = _usWriteSize / g_tSF.SectorSize;	/* 页数 */
	NumOfSingle = _usWriteSize % g_tSF.SectorSize;	/* 剩余的字节数 */

	if (Addr == 0)				/* 起始地址是扇区首地址 */
		{
			if (NumOfPage == 0)	/* 数据长度小于扇区大小 */
				{
					if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize))
					{
						return 0;
					}
				}
			else				/* 数据长度大于扇区大小 */
				{
					while (NumOfPage--)
						{
							if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,g_tSF.SectorSize) == 0)
							{
								return 0;
							}
							_uiWriteAddr += g_tSF.SectorSize;
							_pBuf += g_tSF.SectorSize;
						}
					if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,NumOfSingle) == 0)
						{
							return 0;
						}
				}
		}
	else							/* 起始地址不是扇区首地址 */
		{
			if(NumOfPage == 0)		/* 数据长度小于扇区大小 */
			{
				if (NumOfSingle > count)
					{
						temp = NumOfSingle - count;

						if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,count) == 0)
							{
								return 0;
							}

						_uiWriteAddr += count;
						_pBuf += count;

						if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,temp) == 0)
							{
								return 0;
							}
					}
				else
					{
						if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize) == 0)
						{
							return 0;
						}
					}
			}
			else				/* 数据长度大于扇区大小 */
				{
					_usWriteSize -= count;
					NumOfPage = _usWriteSize / g_tSF.SectorSize;
					NumOfSingle = _usWriteSize % g_tSF.SectorSize;
					if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,count) == 0)
						{
							return 0;
						}
					_uiWriteAddr += count;
					_pBuf += count;

					while (NumOfPage--)
						{
							if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,g_tSF.SectorSize) == 0)
								{
									return 0;
								}
							_uiWriteAddr += g_tSF.SectorSize;
							_pBuf += g_tSF.SectorSize;
						}
					if (NumOfSingle != 0)
						{
							if (sf_AutoWriteSector(_pBuf,_uiWriteAddr,NumOfSingle) == 0)
								{
									return 0;
								}
						}
				}
		}
	return 1;			/* 成功 */
}

/*******************************************************************************
  * @FunctionName: sf_PageWrite
  * @Author:       trx
  * @DateTime:     2022年4月25日 17:03:59 
  * @Purpose:      页编程√√√
  * @param:        _pBuf        数据源缓冲区
  * @param:        _uiWriteAddr 目标区域首地址
  * @param:        _usSize       数据个数，页大小的整数倍(256字节的整数倍)
*******************************************************************************/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i,j;
	if (g_tSF.ChipID == SST25VF016B_ID)
		{
			/* AAI指令要求传入的数据个数是偶数 */
			if ((_usSize < 2) && (_usSize % 2))
				{
					return ;
				}
			sf_WriteEnable();		/* 发送写使能命令 */

			sf_SetCS(0);			/* 使能片选 */
			g_spiLen = 0;
			g_spiTxBuf[g_spiLen++] = CMD_AAI;		/* 发送AAI命令(地址自动增加编程) */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);	/* 发送扇区地址的高8bit */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);	/* 发送扇区地址中8bit */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff));			/*发送扇区地址低8bit */
			g_spiTxBuf[g_spiLen++] = (*_pBuf++);						/* 发送第一个数据 */
			g_spiTxBuf[g_spiLen++] = (*_pBuf++);						/* 发送第2个数据 */
			bsp_spi1Transfer();
			sf_SetCS(0);			/* 禁止片选 */

			sf_WaitForWriteEnd();	/* 等待串行flash内部写操作完成 */

			_usSize -= 2;			/* 计算剩余字节数目 */

			for (i = 0; i < _usSize / 2; ++i)
				{
					sf_SetCS(0);	/* 使能片选 */
					g_spiLen = 0;
					g_spiTxBuf[g_spiLen++] = (CMD_AAI);
					g_spiTxBuf[g_spiLen++] = (*_pBuf++);
					g_spiTxBuf[g_spiLen++] = (*_pBuf++);
					bsp_spi1Transfer();
					sf_SetCS(1);	/* 禁止片选 */
					sf_WaitForWriteEnd();/* 等待串行flash内部操作完成 */
				}

			/* 进入写保护状态 */
			sf_SetCS(0);
			g_spiLen = 0;
			g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
			bsp_spi1Transfer();
			sf_SetCS(1);

			sf_WaitForWriteEnd();
		}
	else	/* for MX25L1606E,W25164BV */
		{
			for (j = 0; j < _usSize / 256; ++j)
				{
					sf_WriteEnable();	/* 发送写使能命令 */

					sf_SetCS(0);		/* 使能片选 */
					g_spiLen = 0;
					g_spiTxBuf[g_spiLen++] = (0x02);								/* 发送AAI命令(地址自动增加编程) */
					g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);
					g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);
					g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xff);
					for (i = 0; i < 256; ++i)
						{
							g_spiTxBuf[g_spiLen++] = (*_pBuf++);			/* 发送数据 */
						}
					bsp_spi1Transfer();
					sf_SetCS(1);		/* 禁止片选 */

					sf_WaitForWriteEnd();/* 等待串行flash内部操作完成 */
					_uiWriteAddr += 256;
				}

			/* 进入写保护状态 */
			sf_SetCS(0);
			g_spiLen = 0;
			g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
			bsp_spi1Transfer();
			sf_SetCS(1);

			sf_WaitForWriteEnd();		/* 等待串行flash内部操作完成 */
		}
}


/*******************************************************************************
  * @FunctionName: sf_EraseSector
  * @Author:       trx
  * @DateTime:     2022年4月25日 17:21:35 
  * @Purpose:      擦除指定的扇区√√√
  * @param:        _uiSectorAddr      扇区地址
  * @return:       none
*******************************************************************************/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();		/* 发送写使能命令 */

	/* 擦除扇区操作 */
	sf_SetCS(0);			/* 使能片选 */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = CMD_SE;
	/* 在发送擦除指令后,紧接这发送24位的地址 */
	g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xf0000) >> 16);
	g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xff00) >> 8);
	g_spiTxBuf[g_spiLen++] = (_uiSectorAddr & 0xff);
	bsp_spi1Transfer();
	sf_SetCS(1);			/* 禁止片选 */

	sf_WaitForWriteEnd();	/* 等待串行flash内部操作完成 */
}


