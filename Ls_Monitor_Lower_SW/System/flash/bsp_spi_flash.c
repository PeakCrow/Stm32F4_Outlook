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

/* оƬID 	0x5217   21015 */

/* ����Flash��ƬѡGPIO�˿ڣ� PB14  */
#define SF_CS_CLK_ENABLE() 			__HAL_RCC_GPIOB_CLK_ENABLE()
#define SF_CS_GPIO					GPIOB
#define SF_CS_PIN					GPIO_PIN_14

#define SF_CS_0()					SF_CS_GPIO->BSRR = ((uint32_t)SF_CS_PIN << 16U)
#define SF_CS_1()					SF_CS_GPIO->BSRR = SF_CS_PIN
	
#define CMD_AAI       0xAD  	/* AAI �������ָ��(FOR SST25VF016B) */
#define CMD_DISWR	  0x04		/* ��ֹд, �˳�AAI״̬ */
#define CMD_EWRSR	  0x50		/* ����д״̬�Ĵ��������� */
#define CMD_WRSR      0x01  	/* д״̬�Ĵ������� */
#define CMD_WREN      0x06		/* дʹ������ */
#define CMD_READ      0x03  	/* ������������ */
#define CMD_RDSR      0x05		/* ��״̬�Ĵ������� */
#define CMD_RDID      0x9F		/* ������ID���� */
#define CMD_SE        0x20		/* ������������ */
#define CMD_BE        0xC7		/* ������������ */
#define DUMMY_BYTE    0xA5		/* ���������Ϊ����ֵ�����ڶ����� */

#define WIP_FLAG      0x01		/* ״̬�Ĵ����е����ڱ�̱�־��WIP) */

SFLASH_T g_tSF;

void sf_WriteEnable(void);
static void sf_WaitForWriteEnd(void);
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t *_ucpNewBuf, uint16_t _usLen);
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t *_ucpTar, uint32_t _uiSize);
uint8_t sf_AutoWriteSector(uint8_t *_ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen);

//static uint8_t g_spiTxBuf[4*1024];	/* ����д�������ȶ��������������޸Ļ�������������������д */

/*******************************************************************************
  * @FunctionName: sf_SetCS
  * @Author:       trx
  * @DateTime:     2022��4��29�� 13:13:02 
  * @Purpose:      ����flashƬѡ���ƺ����̡̡�
  * @param:        _Level��0��Ƭѡ��1����ֹƬѡ
  * @return:       none
*******************************************************************************/
void sf_SetCS(uint8_t _Level)
{
	if (_Level == 0)
	{
		bsp_Spi1BusEnter();	
		/* �����дflash�������������ʹ�ôθ��� */
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
*	�� �� ��: sf_WriteEnable
*	����˵��: ����������дʹ������̡̡�
*	��    ��: �� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_WriteEnable(void)
{
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_WREN);			/* �������� */
	bsp_spi1Transfer();
	sf_SetCS(1);									/* ����Ƭѡ */
}
/*
*********************************************************************************************************
*	�� �� ��: sf_WaitForWriteEnd
*	����˵��: ����ѭ����ѯ�ķ�ʽ�ȴ������ڲ�д�������
*	��    ��: ��	�̡̡�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void sf_WaitForWriteEnd(void)
{
	sf_SetCS(0);									/* ʹ��Ƭѡ */
	g_spiTxBuf[0] = (CMD_RDSR);						/* ������� ��״̬�Ĵ��� */
	g_spiLen = 2;
	bsp_spi1Transfer();	
	sf_SetCS(1);									/* ����Ƭѡ */
	
	while(1)
	{
		sf_SetCS(0);								/* ʹ��Ƭѡ */
		g_spiTxBuf[0] = (CMD_RDSR);					/* ������� ��״̬�Ĵ��� */
		g_spiTxBuf[1] = 0;							/* �޹����� */
		g_spiLen = 2;
		bsp_spi1Transfer();	
		sf_SetCS(1);								/* ����Ƭѡ */
		/* ���Ͷ�״̬��ָ���,�жϽ��յ��ֽڵĵ�0λ�Ƿ�Ϊ0 */
		/* 0������豸��׼���ý�һ����ָʾ(ͨ���������ֲ��֪) */
		if ((g_spiRxBuf[1] & WIP_FLAG) != SET)		/* �ж�״̬�Ĵ�����æ��־λ */
		{
			break;
		}		
	}
	//printf("�����ڲ��������\r\n");
}


/*
*********************************************************************************************************
*	�� �� ��: sf_ReadInfo
*	����˵��: ��ȡ����ID,�������������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void sf_ReadInfo(void)
{
	/* �Զ�ʶ����Flash�ͺ� */
	{
		g_tSF.ChipID = sf_ReadID();	/* оƬID */

		switch (g_tSF.ChipID)
		{
			case SST25VF016B_ID:
				strcpy(g_tSF.ChipName, "SST25VF016B");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;

			case MX25L1606E_ID:
				strcpy(g_tSF.ChipName, "MX25L1606E");
				g_tSF.TotalSize = 2 * 1024 * 1024;	/* ������ = 2M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;

			case W25Q64BV_ID:
				strcpy(g_tSF.ChipName, "W25Q64BV");
				g_tSF.TotalSize = 8 * 1024 * 1024;	/* ������ = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
				break;
			
			case N25Q128_ID:
				strcpy(g_tSF.ChipName, "N25Q128FV");
				g_tSF.TotalSize = 16 * 1024 * 1024;	/* ������ = 8M */
				g_tSF.SectorSize = 4 * 1024;		/* ������С = 4K */
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
*	�� �� ��: sf_ReadID
*	����˵��: ��ȡ����������ID
*	��    ��:  ��
*	�� �� ֵ: 32bit������ID (���8bit��0����ЧIDλ��Ϊ24bit��
*********************************************************************************************************
*/
uint32_t sf_ReadID(void)
{
	uint32_t uiID;
	uint8_t id1, id2, id3;

	sf_SetCS(0);							/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[0] = (CMD_RDID);				/* ���Ͷ�ID���� */
	g_spiLen = 4;
	bsp_spi1Transfer();
	
	id1 = g_spiRxBuf[1];					/* ��ID�ĵ�1���ֽ� */
	id2 = g_spiRxBuf[2];					/* ��ID�ĵ�2���ֽ� */
	id3 = g_spiRxBuf[3];					/* ��ID�ĵ�3���ֽ� */
	sf_SetCS(1);							/* ����Ƭѡ */

	uiID = ((uint32_t)id1 << 16) | ((uint32_t)id2 << 8) | id3;
	return uiID;
}


/*
*********************************************************************************************************
*	�� �� ��: sf_EraseChip
*	����˵��: ��������оƬ�̡̡�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void sf_EraseChip(void)
{	
	sf_WriteEnable();								/* ����дʹ������ */

	/* ������������ */
	sf_SetCS(0);		/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = CMD_BE;				/* ������Ƭ�������� */
	bsp_spi1Transfer();
	sf_SetCS(1);									/* ����Ƭѡ */

	sf_WaitForWriteEnd();							/* �ȴ�����Flash�ڲ�д������� */
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSFlash
*	����˵��: ����falshӲ����ʼ���� ����CS GPIOƬѡ�źţ� ��ȡID��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitSFlash(void)
{
	/* ����CS GPIO */
	{
		GPIO_InitTypeDef gpio_init;

		/* ��GPIOʱ�� */
		SF_CS_CLK_ENABLE();
		
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ����������� */
		gpio_init.Pull = GPIO_PULLUP;			/* ���������費ʹ�� */
		gpio_init.Speed = GPIO_SPEED_HIGH;  	/* GPIO�ٶȵȼ� */	
		gpio_init.Pin = SF_CS_PIN;	
		HAL_GPIO_Init(SF_CS_GPIO, &gpio_init);	
	}
	
	/* ��ȡоƬID, �Զ�ʶ��оƬ�ͺ� */
	sf_ReadInfo();
}	
/*******************************************************************************
  * @FunctionName: sf_NeedErase
  * @Author:       trx
  * @DateTime:     2022��4��25�� 13:28:19 
  * @Purpose:      �ж�дPAGE֮ǰ�Ƿ���Ҫ�����̡̡�
  * @param:        _ucpOldBuf��������
  * @param:        _ucpNewBuf��������
  * @param:        _usLen    �����ݸ���
  * @return:		0������Ҫ������1����Ҫ����
*******************************************************************************/
static uint8_t sf_NeedErase(uint8_t * _ucpOldBuf, uint8_t * _ucpNewBuf, uint16_t _usLen)
{
	uint16_t i;
	uint8_t ucOld;

	/*
	�㷨��һ����old�󷴣�new����
				1101	0101
			~	
			=	0010	0101
	�㷨�ڶ�����old�󷴽��λ��new
				0010
			&	0101
			=	0000
	�㷨�����������Ϊ0�����ʾ��������������ʾ��Ҫ����
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
  * @DateTime:     2022��4��25�� 14:07:40 
  * @Purpose:      �Ƚ�flash�����ݡ̡̡�
  * @param:        _uiSrcAddr�����ݻ�����
  * @param:        _ucpTar     falsh��ַ
  * @param:        _uiSize     ���ݸ��������ܳ���оƬ������
  * @return:       0����ȣ�1������ȣ�
*******************************************************************************/
static uint8_t sf_CmpData(uint32_t _uiSrcAddr, uint8_t * _ucpTar, uint32_t _uiSize)
{
	uint16_t i,j;
	uint16_t rem;
	
	/* �����ȡ�����ݳ���Ϊ0���߳�������flash��ַ�ռ䣬��ֱ�ӷ��� */

	if ((_uiSrcAddr + _uiSize) > g_tSF.TotalSize)
		{
			return 	1;
		}

	if (_uiSize == 0)
		{
			return 0;
		}
	sf_SetCS(0);				/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_READ);
	g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff0000) >> 16);
	g_spiTxBuf[g_spiLen++] = ((_uiSrcAddr & 0xff00) >> 8);
	g_spiTxBuf[g_spiLen++] = (_uiSrcAddr & 0xff);
	bsp_spi1Transfer();

	/* ��ʼ�����ݣ�ӦΪ�ײ�DMA���������ޣ�����ְ��� */
	for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; ++i)
		{
			g_spiLen = SPI_BUFFER_SIZE;
			bsp_spi1Transfer();

			for (j = 0; j < SPI_BUFFER_SIZE; ++j)
				{
					if(g_spiRxBuf[j] != *_ucpTar++)
						{
							goto NOTEQ;			/* ����� */
						}
				}
		}
	rem = _uiSize % SPI_BUFFER_SIZE;			/* ʣ���ֽ� */
	if (rem > 0)
		{
			g_spiLen = rem;
			bsp_spi1Transfer();

			for (j = 0; j < rem; ++j)
				{
					if (g_spiRxBuf[j] != *_ucpTar++)
						{
							goto NOTEQ;		/* ����� */
						}
				}
		}
	sf_SetCS(1);
	return 0;			/* ��� */

NOTEQ:
	sf_SetCS(1);
	return 1;			/* ����� */
}


/*******************************************************************************
  * @FunctionName: sf_ReadBuffer
  * @Author:       trx
  * @DateTime:     2022��4��25�� 14:47:32 
  * @Purpose:      ������ȡ�����ֽڣ��ֽڸ������ܳ���оƬ�����̡̡�
  * @param:        _pBuf       ����Դ������
  * @param:        _uiReadAddr �׵�ַ
  * @param:        _uiSize     ���ݸ��������ܳ���оƬ������
*******************************************************************************/
void sf_ReadBuffer(uint8_t * _pBuf, uint32_t _uiReadAddr, uint32_t _uiSize)
{
	uint16_t rem;
	uint16_t i;

	/* �����ȡ�����ݳ���Ϊ0���߳�������flash��ַ�ռ䣬��ֱ�ӷ��� */
	if ((_uiSize == 0) || (_uiReadAddr + _uiSize) > g_tSF.TotalSize)
    {
        return ;
    }

	/* ������������ */
	sf_SetCS(0);								/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = (CMD_READ);						/* ���Ͷ�ָ�� */
	g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff0000) >> 16);	/* ����������ַ�ĸ�8bit */
	g_spiTxBuf[g_spiLen++] = ((_uiReadAddr & 0xff00) >> 8);		/* ����������ַ����8bit */
	g_spiTxBuf[g_spiLen++] = (_uiReadAddr & 0xff);				/* ����������ַ��8bit */
	bsp_spi1Transfer();			/* ��һ�η���0x03 */

	/* ��ʼ�����ݣ���Ϊ�ײ�DMA���������ޣ�����ְ��� */
	for (i = 0; i < _uiSize / SPI_BUFFER_SIZE; i++)
    {
        g_spiLen = SPI_BUFFER_SIZE;							/* ÿ�ζ�ȡ4k��С������ */
        bsp_spi1Transfer();	/* �ڶ��η���0x03 */
        /* �Ӵ洢��g_spiRxBuf����SPI_BUFFER_SIZE���ֽڵ�_pBuf */
        /* ����һ��ָ��_pBuf�洢����ָ�� */
        memcpy(_pBuf, g_spiRxBuf,SPI_BUFFER_SIZE);
        _pBuf += SPI_BUFFER_SIZE;							/* ��ַ���㣬��_pBufָ��ָ����һ���������׵�ַ */
    }
	rem = _uiSize % SPI_BUFFER_SIZE;			/* ʣ���ֽڣ�����Ҫ��ȡ�������Ƿ��������� */

	if (rem > 0)
    {
        g_spiLen = rem;
        bsp_spi1Transfer();
        memcpy(_pBuf,g_spiRxBuf,rem);		/* ��������ֽ��ٴν��и��� */
    }
	sf_SetCS(1);								/* ��ֹƬѡ */
}
/*******************************************************************************
  * @FunctionName: sf_AutoWriteSector
  * @Author:       trx
  * @DateTime:     2022��4��25�� 15:47:44 
  * @Purpose:      дһ��������У�飬�������ȷ������д���Σ��������Զ���ɡ̡̡�
  * @param:        _ucpSrc     ����Դ������
  * @param:        _uiWrAddr   Ŀ�������׵�ַ
  * @param:        _usWrLen    ���ݸ��������ܳ���������С
  * @return:       0������1���ɹ���
*******************************************************************************/
uint8_t sf_AutoWriteSector(uint8_t * _ucpSrc, uint32_t _uiWrAddr, uint16_t _usWrLen)
{
	uint16_t i;
	uint16_t j;				/* ������ʱ */
	uint32_t uiFirstAddr;	/* �����׵�ַ */
	uint8_t ucNeedErase;	/* 1��ʾ��Ҫ���� */
	uint8_t cRet;
	
	/* ����Ϊ0ʱ������������ֱ����Ϊ�ɹ� */
	if (_usWrLen == 0)
		{
			return 1;
		}

	/* ���ƫ�Ƶ�ַ����оƬ�������˳� */
	if (_uiWrAddr >= g_tSF.TotalSize)
		{
			return 0;
		}

	/* ������ݳ��ȴ����������������˳� */
	if (_usWrLen > g_tSF.SectorSize)
		{
			return 0;
		}

	/* ���flash�е�����û�б仯����дflash */
	sf_ReadBuffer(g_spiTxBuf,_uiWrAddr,_usWrLen);
	if (memcmp(g_spiTxBuf,_ucpSrc,_usWrLen) == 0)
		{
			return 1;
		}

	/* �ж��Ƿ���Ҫ�Ȳ������� */
	/* ����������޸�Ϊ�����ݣ�����λ���� 1->0 ���� 0->0,��������������flash���� */
	ucNeedErase = 0;			/* 0����Ҫ���� */
	if (sf_NeedErase(g_spiTxBuf,_ucpSrc,_usWrLen))
		{
			ucNeedErase = 1;	/* 1��Ҫ���� */
		}

	uiFirstAddr = _uiWrAddr & (~(g_tSF.SectorSize - 1));

	if (_usWrLen == g_tSF.SectorSize)		/* ������������д */
		{
			for (i = 0; i < g_tSF.SectorSize; ++i)
				{
					g_spiTxBuf[i] = _ucpSrc[i];
				}
		}
	else									/* ��д�������� */
	{
		/* �Ƚ��������������ݶ��� */
		sf_ReadBuffer(g_spiTxBuf,uiFirstAddr,g_tSF.SectorSize);

		/* ���������ݸ��� */
		i = _uiWrAddr & (g_tSF.SectorSize - 1);
		memcpy(&g_spiTxBuf[i],_ucpSrc,_usWrLen);
	}

	/* д��֮�����У�飬�������ȷ����д�����3�� */
	cRet = 0;
	for (i = 0; i < 3; ++i)
		{
			/* ����������޸�Ϊ�����ݣ�����λ����1->0,����0->0����������������flash���� */
			if (ucNeedErase == 1)
				{
					sf_EraseSector(uiFirstAddr);	/* ����1������ */
				}

			/* ���һ������ */
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
				/* ʧ�ܺ��ӳ�һ��ʱ�������� */
				for(j = 0;j < 10000;j++);
			}
		}
	return cRet;
}

/*******************************************************************************
  * @FunctionName: sf_WriteBuffer
  * @Author:       trx
  * @DateTime:     2022��4��25�� 17:59:58 
  * @Purpose:      дһ��������У�飬�������ȷ������д���Σ������Զ����,һ�������Ĵ�СΪ4K�̡̡�
  * @param:        _pBuf        ����Դ������
  * @param:        _uiWriteAddr Ŀ�������׵�ַ
  * @param:        _usWriteSize  ���ݸ�������������оƬ����
  * @return:       1���ɹ���0��ʧ�ܣ�
*******************************************************************************/
uint8_t sf_WriteBuffer(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint32_t _usWriteSize)
{
	uint32_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0,temp = 0;

	Addr = _uiWriteAddr % g_tSF.SectorSize;			/* �����ڵĵ�ַ */
	count = g_tSF.SectorSize - Addr;				/* ����ʼ��ַ��ʼҪд�������� */
	NumOfPage = _usWriteSize / g_tSF.SectorSize;	/* ҳ�� */
	NumOfSingle = _usWriteSize % g_tSF.SectorSize;	/* ʣ����ֽ��� */

	if (Addr == 0)				/* ��ʼ��ַ�������׵�ַ */
		{
			if (NumOfPage == 0)	/* ���ݳ���С��������С */
				{
					if(sf_AutoWriteSector(_pBuf,_uiWriteAddr,_usWriteSize))
					{
						return 0;
					}
				}
			else				/* ���ݳ��ȴ���������С */
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
	else							/* ��ʼ��ַ���������׵�ַ */
		{
			if(NumOfPage == 0)		/* ���ݳ���С��������С */
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
			else				/* ���ݳ��ȴ���������С */
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
	return 1;			/* �ɹ� */
}

/*******************************************************************************
  * @FunctionName: sf_PageWrite
  * @Author:       trx
  * @DateTime:     2022��4��25�� 17:03:59 
  * @Purpose:      ҳ��̡̡̡�
  * @param:        _pBuf        ����Դ������
  * @param:        _uiWriteAddr Ŀ�������׵�ַ
  * @param:        _usSize       ���ݸ�����ҳ��С��������(256�ֽڵ�������)
*******************************************************************************/
void sf_PageWrite(uint8_t * _pBuf, uint32_t _uiWriteAddr, uint16_t _usSize)
{
	uint32_t i,j;
	if (g_tSF.ChipID == SST25VF016B_ID)
		{
			/* AAIָ��Ҫ��������ݸ�����ż�� */
			if ((_usSize < 2) && (_usSize % 2))
				{
					return ;
				}
			sf_WriteEnable();		/* ����дʹ������ */

			sf_SetCS(0);			/* ʹ��Ƭѡ */
			g_spiLen = 0;
			g_spiTxBuf[g_spiLen++] = CMD_AAI;		/* ����AAI����(��ַ�Զ����ӱ��) */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);	/* ����������ַ�ĸ�8bit */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);	/* ����������ַ��8bit */
			g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff));			/*����������ַ��8bit */
			g_spiTxBuf[g_spiLen++] = (*_pBuf++);						/* ���͵�һ������ */
			g_spiTxBuf[g_spiLen++] = (*_pBuf++);						/* ���͵�2������ */
			bsp_spi1Transfer();
			sf_SetCS(0);			/* ��ֹƬѡ */

			sf_WaitForWriteEnd();	/* �ȴ�����flash�ڲ�д������� */

			_usSize -= 2;			/* ����ʣ���ֽ���Ŀ */

			for (i = 0; i < _usSize / 2; ++i)
				{
					sf_SetCS(0);	/* ʹ��Ƭѡ */
					g_spiLen = 0;
					g_spiTxBuf[g_spiLen++] = (CMD_AAI);
					g_spiTxBuf[g_spiLen++] = (*_pBuf++);
					g_spiTxBuf[g_spiLen++] = (*_pBuf++);
					bsp_spi1Transfer();
					sf_SetCS(1);	/* ��ֹƬѡ */
					sf_WaitForWriteEnd();/* �ȴ�����flash�ڲ�������� */
				}

			/* ����д����״̬ */
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
					sf_WriteEnable();	/* ����дʹ������ */

					sf_SetCS(0);		/* ʹ��Ƭѡ */
					g_spiLen = 0;
					g_spiTxBuf[g_spiLen++] = (0x02);								/* ����AAI����(��ַ�Զ����ӱ��) */
					g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff0000) >> 16);
					g_spiTxBuf[g_spiLen++] = ((_uiWriteAddr & 0xff00) >> 8);
					g_spiTxBuf[g_spiLen++] = (_uiWriteAddr & 0xff);
					for (i = 0; i < 256; ++i)
						{
							g_spiTxBuf[g_spiLen++] = (*_pBuf++);			/* �������� */
						}
					bsp_spi1Transfer();
					sf_SetCS(1);		/* ��ֹƬѡ */

					sf_WaitForWriteEnd();/* �ȴ�����flash�ڲ�������� */
					_uiWriteAddr += 256;
				}

			/* ����д����״̬ */
			sf_SetCS(0);
			g_spiLen = 0;
			g_spiTxBuf[g_spiLen++] = (CMD_DISWR);
			bsp_spi1Transfer();
			sf_SetCS(1);

			sf_WaitForWriteEnd();		/* �ȴ�����flash�ڲ�������� */
		}
}


/*******************************************************************************
  * @FunctionName: sf_EraseSector
  * @Author:       trx
  * @DateTime:     2022��4��25�� 17:21:35 
  * @Purpose:      ����ָ���������̡̡�
  * @param:        _uiSectorAddr      ������ַ
  * @return:       none
*******************************************************************************/
void sf_EraseSector(uint32_t _uiSectorAddr)
{
	sf_WriteEnable();		/* ����дʹ������ */

	/* ������������ */
	sf_SetCS(0);			/* ʹ��Ƭѡ */
	g_spiLen = 0;
	g_spiTxBuf[g_spiLen++] = CMD_SE;
	/* �ڷ��Ͳ���ָ���,�����ⷢ��24λ�ĵ�ַ */
	g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xf0000) >> 16);
	g_spiTxBuf[g_spiLen++] = ((_uiSectorAddr & 0xff00) >> 8);
	g_spiTxBuf[g_spiLen++] = (_uiSectorAddr & 0xff);
	bsp_spi1Transfer();
	sf_SetCS(1);			/* ��ֹƬѡ */

	sf_WaitForWriteEnd();	/* �ȴ�����flash�ڲ�������� */
}


