#if   1

#include "bsp_spi_flash_demo.h"

#define TEST_ADDR	0	/* 读写测试地址 */
#define	TEST_SIZE	4096/* 读写测试数据大小 */



static void sfDispMenu(void);
static void sfReadTest(void);
static void sfWriteTest(void);
static void sfErase(void);
static void sfViewData(uint32_t _uiAddr);
static void sfWriteAll(uint8_t _ch);
static void sfTestReadSpeed(void);

static uint8_t buf[TEST_SIZE];



/*******************************************************************************
  * @FunctionName: sfReadTest
  * @Author:       trx
  * @DateTime:     2022年4月25日21:57:20 
  * @Purpose:      测试串行flash读取速度，读取整个串行flash的数据，最后打印结果
  * @param:        void
  * @return:       none
*******************************************************************************/
void sfReadTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* 起始地址 = 0，数据长度为256 */
	iTime1 = bsp_GetRunTime();		/* 记下开始时间 */
	sf_ReadBuffer(buf,TEST_ADDR,TEST_SIZE);
	iTime2 = bsp_GetRunTime();		/* 记下结束时间 */
	printf("读串行flash成功，数据如下：\r\n");

	/* 打印数据 */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			if ((i > 0) && (i % 16) == 0)
				{
					printf(" - \r\n");	/* 每行显示16字节数据 */
					if((i > 0) && (i % 256) == 0)
						printf("\r\n");	/* 每个page进行换行，一个扇区有16个page */
				}
			printf(" %02x",buf[i]);
		}

	/* 打印读取速度 */
	printf(" - \r\n");
	printf("数据长度:%d字节，读取耗时:%dms,读取速度:%d Bytes/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}

/*******************************************************************************
  * @FunctionName: sfWriteTest
  * @Author:       trx
  * @DateTime:     2022年4月25日22:15:30 
  * @Purpose:      x写串行flash测试
  * @param:        void
  * @return:       none
*******************************************************************************/
static void sfWriteTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* 填充测试缓冲区 */
	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = i;
		}
	sf_EraseSector(TEST_ADDR);
	/* 写eeprom，起始地址 = 0，数据长度为256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	if (sf_WriteBuffer(buf,TEST_ADDR,TEST_SIZE) == 0)
		{
			printf("写串行flash出错！\r\n");
			return ;
		}
	else
		{
			iTime2 = bsp_GetRunTime();/* 记下结束时间 */
			printf("写串行flash成功！\r\n");
		}

	/* 打印读取速度 */
	printf("数据长度：%d字节，写耗时：%dms，写速度：%dB/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}


/*******************************************************************************
  * @FunctionName: sfWriteAll
  * @Author:       trx
  * @DateTime:     2022年4月25日22:17:03 
  * @Purpose:      写串行flash全部数据
  * @param:        _ch：数据内容
  * @return:       none
*******************************************************************************/
void sfWriteAll(uint8_t _ch)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* 填充测试缓冲区 */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = _ch;
		}

	/* 写eeprom，起始地址 = 0，数据长度为256 */
	iTime1 = bsp_GetRunTime();		/* 记下开始时间 */
	/* 16M字节，2048个扇区 */
	for (i = 0; i < g_tSF.TotalSize / g_tSF.SectorSize / 2; ++i)
		{
			if(sf_WriteBuffer(buf,i * g_tSF.SectorSize,g_tSF.SectorSize) == 0)
				{
					printf("写串行flash出错！\r\n");
					return;
				}
			printf("i: %d,	%d",i,g_tSF.TotalSize / g_tSF.SectorSize / 2);
			if (((i + 1) % 128) == 0)
				{
					printf("\r\n");
				}
		}
	iTime2 = bsp_GetRunTime();		/* 记下结束时间 */

	/* 打印读取速度 */
	printf("数据长度：%dK字节，写耗时：%dms,写速度：%dB/s\r\n",g_tSF.TotalSize / 1024,iTime2 - iTime1,(g_tSF.TotalSize * 1000) / (iTime2 - iTime1));
}
/*******************************************************************************
  * @FunctionName: sfErase
  * @Author:       trx
  * @DateTime:     2022年4月25日22:31:35 
  * @Purpose:      擦除串行flash测试
  * @param:        void
  * @return:       none
*******************************************************************************/
void sfErase(void)
{
	int32_t iTime1,iTime2;

	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	sf_EraseChip();
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */

	/* 打印擦除速度 */
	printf("擦除串行flash完成！耗时：%dms\r\n",iTime2 - iTime1);
	return;
}

/*******************************************************************************
  * @FunctionName: sfViewData
  * @Author:       trx
  * @DateTime:     2022年4月25日22:35:53 
  * @Purpose:      读取串行flash数据并显示，每次显示1K的内容
  * @param:        _uiAddr:串行flash读取数据地址
  * @return:       none
*******************************************************************************/
void sfViewData(uint32_t _uiAddr)
{
	uint16_t i;

	sf_ReadBuffer(buf,_uiAddr,1024);	/* 读数据 */
	printf("地址：0x%08X; 数据长度 = 1024\r\n",_uiAddr);

	/* 打印数据 */
	for (i = 0; i < 1024; ++i)
		{
			printf(" %02X",buf[i]);

			if ((i & 31) == 31)
				{
					printf("\r\n");		/* 每行显示16字节数据 */
				}
			else if((i & 31) == 15)
				{
					printf(" - ");
				}
		}
}

void sfTestReadSpeed(void)
{
	uint16_t i;
	int32_t iTime1, iTime2;
	uint32_t uiAddr;

	/* 起始地址 = 0， 数据长度为 256 */
	iTime1 = bsp_GetRunTime();	/* 记下开始时间 */
	uiAddr = 0;
	for (i = 0; i < g_tSF.TotalSize / TEST_SIZE; i++, uiAddr += TEST_SIZE)
	{
		sf_ReadBuffer(buf, uiAddr, TEST_SIZE);
	}
	iTime2 = bsp_GetRunTime();	/* 记下结束时间 */

	/* 打印读速度 */
	printf("数据长度: %d字节, 读耗时: %dms, 读速度: %lld Bytes/s\r\n", g_tSF.TotalSize, iTime2 - iTime1, (uint64_t)g_tSF.TotalSize * 1000 / (iTime2 - iTime1));
}



static void sfDispMenu(void)
{
	printf("\r\n*******************************************\r\n");
	printf("请选择操作命令:\r\n");
	printf("【1 - 读串行Flash, 地址:0x%X,长度:%d字节】\r\n", TEST_ADDR, TEST_SIZE);
	printf("【2 - 写串行Flash, 地址:0x%X,长度:%d字节】\r\n", TEST_ADDR, TEST_SIZE);
	printf("【3 - 擦除整个串行Flash】\r\n");
	printf("【4 - 写整个串行Flash, 全0x55】\r\n");
	printf("【5 - 读整个串行Flash, 测试读速度】\r\n");
	printf("【Z - 读取前1K，地址自动减少】\r\n");
	printf("【X - 读取后1K，地址自动增加】\r\n");
	printf("其他任意键 - 显示命令提示\r\n");
	printf("\r\n");
}
/*******************************************************************************
  * @FunctionName: DemoSpiFlash
  * @Author:       trx
  * @DateTime:     2022骞?鏈?6鏃?1:24:26 
  * @Purpose:      
  * @param:        void               
  * @return:       none
*******************************************************************************/
void DemoSpiFlash(void)
{
	uint8_t cmd;
	uint32_t uiReadPageNo = 0;
	/* 检测串行Flash OK */
	printf("检测到串行Flash，ID= %08X,型号：%s \r\n", g_tSF.ChipID , g_tSF.ChipName);
	printf("    容量 : %dM字节, 扇区容量: %d字节\r\n", g_tSF.TotalSize/(1024*1024), g_tSF.SectorSize);

	sfDispMenu();		/* 打印命令提示 */

	while(1)
	{
		if (comGetChar(COM1,&cmd))	/* 从串口读入一个字符(非阻塞方式) */
		{
			switch (cmd)
			{
				case '1':
					printf("\r\n【1 - 读串行Flash, 地址:0x%X,长度:%d字节】\r\n", TEST_ADDR, TEST_SIZE);
					sfReadTest();	/* 读串行Flash数据，并打印出来数据内容 */
					break;

				case '2':
					printf("\r\n【2 - 擦除指定扇区, 地址:0x%X,长度:%d字节】\r\n", TEST_ADDR, TEST_SIZE);
					sfWriteTest();
					break;
				case '3':
					printf("\r\n【3 - 擦除整个串行Flash】\r\n");
					printf("整个Flash擦除完毕大概需要20秒左右，请耐心等待");
					sfErase();		/* 擦除串行Flash数据，实际上就是写入全0xFF */
					break;

				case '4':
					printf("\r\n【4 - 写整个串行Flash, 全0x55】\r\n");
					printf("整个Flash写入完毕大概需要20秒左右，请耐心等待");
					sfWriteAll(0x55);/* 擦除串行Flash数据，实际上就是写入全0xFF */
					break;

				case '5':
					printf("\r\n【5 - 读整个串行Flash, %dM字节】\r\n", g_tSF.TotalSize/(1024*1024));
					sfTestReadSpeed(); /* 读整个串行Flash数据，测试速度 */
					break;

				case 'z':
				case 'Z': /* 读取前1K */
					if (uiReadPageNo > 0)
					{
						uiReadPageNo--;
					}
					else
					{
						printf("已经是最前\r\n");
					}
					sfViewData(uiReadPageNo * 1024);
					break;

				case 'x':
				case 'X': /* 读取后1K */
					if (uiReadPageNo < g_tSF.TotalSize / 1024 - 1)
					{
						uiReadPageNo++;
					}
					else
					{
						printf("已经是最后\r\n");
					}
					sfViewData(uiReadPageNo * 1024);
					break;
				default:
					sfDispMenu();	/* 无效命令，重新打印命令提示 */
					break;

			}
		}
	}
	
}


#endif

