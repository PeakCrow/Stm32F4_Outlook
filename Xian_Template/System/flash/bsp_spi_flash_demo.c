#if   1

#include "bsp_spi_flash_demo.h"

#define TEST_ADDR	0	/* ��д���Ե�ַ */
#define	TEST_SIZE	4096/* ��д�������ݴ�С */



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
  * @DateTime:     2022��4��25��21:57:20 
  * @Purpose:      ���Դ���flash��ȡ�ٶȣ���ȡ��������flash�����ݣ�����ӡ���
  * @param:        void
  * @return:       none
*******************************************************************************/
void sfReadTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* ��ʼ��ַ = 0�����ݳ���Ϊ256 */
	iTime1 = bsp_GetRunTime();		/* ���¿�ʼʱ�� */
	sf_ReadBuffer(buf,TEST_ADDR,TEST_SIZE);
	iTime2 = bsp_GetRunTime();		/* ���½���ʱ�� */
	printf("������flash�ɹ����������£�\r\n");

	/* ��ӡ���� */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			if ((i > 0) && (i % 16) == 0)
				{
					printf(" - \r\n");	/* ÿ����ʾ16�ֽ����� */
					if((i > 0) && (i % 256) == 0)
						printf("\r\n");	/* ÿ��page���л��У�һ��������16��page */
				}
			printf(" %02x",buf[i]);
		}

	/* ��ӡ��ȡ�ٶ� */
	printf(" - \r\n");
	printf("���ݳ���:%d�ֽڣ���ȡ��ʱ:%dms,��ȡ�ٶ�:%d Bytes/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}

/*******************************************************************************
  * @FunctionName: sfWriteTest
  * @Author:       trx
  * @DateTime:     2022��4��25��22:15:30 
  * @Purpose:      xд����flash����
  * @param:        void
  * @return:       none
*******************************************************************************/
static void sfWriteTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* �����Ի����� */
	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = i;
		}
	sf_EraseSector(TEST_ADDR);
	/* дeeprom����ʼ��ַ = 0�����ݳ���Ϊ256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	if (sf_WriteBuffer(buf,TEST_ADDR,TEST_SIZE) == 0)
		{
			printf("д����flash����\r\n");
			return ;
		}
	else
		{
			iTime2 = bsp_GetRunTime();/* ���½���ʱ�� */
			printf("д����flash�ɹ���\r\n");
		}

	/* ��ӡ��ȡ�ٶ� */
	printf("���ݳ��ȣ�%d�ֽڣ�д��ʱ��%dms��д�ٶȣ�%dB/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}


/*******************************************************************************
  * @FunctionName: sfWriteAll
  * @Author:       trx
  * @DateTime:     2022��4��25��22:17:03 
  * @Purpose:      д����flashȫ������
  * @param:        _ch����������
  * @return:       none
*******************************************************************************/
void sfWriteAll(uint8_t _ch)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* �����Ի����� */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = _ch;
		}

	/* дeeprom����ʼ��ַ = 0�����ݳ���Ϊ256 */
	iTime1 = bsp_GetRunTime();		/* ���¿�ʼʱ�� */
	/* 16M�ֽڣ�2048������ */
	for (i = 0; i < g_tSF.TotalSize / g_tSF.SectorSize / 2; ++i)
		{
			if(sf_WriteBuffer(buf,i * g_tSF.SectorSize,g_tSF.SectorSize) == 0)
				{
					printf("д����flash����\r\n");
					return;
				}
			printf("i: %d,	%d",i,g_tSF.TotalSize / g_tSF.SectorSize / 2);
			if (((i + 1) % 128) == 0)
				{
					printf("\r\n");
				}
		}
	iTime2 = bsp_GetRunTime();		/* ���½���ʱ�� */

	/* ��ӡ��ȡ�ٶ� */
	printf("���ݳ��ȣ�%dK�ֽڣ�д��ʱ��%dms,д�ٶȣ�%dB/s\r\n",g_tSF.TotalSize / 1024,iTime2 - iTime1,(g_tSF.TotalSize * 1000) / (iTime2 - iTime1));
}
/*******************************************************************************
  * @FunctionName: sfErase
  * @Author:       trx
  * @DateTime:     2022��4��25��22:31:35 
  * @Purpose:      ��������flash����
  * @param:        void
  * @return:       none
*******************************************************************************/
void sfErase(void)
{
	int32_t iTime1,iTime2;

	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	sf_EraseChip();
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */

	/* ��ӡ�����ٶ� */
	printf("��������flash��ɣ���ʱ��%dms\r\n",iTime2 - iTime1);
	return;
}

/*******************************************************************************
  * @FunctionName: sfViewData
  * @Author:       trx
  * @DateTime:     2022��4��25��22:35:53 
  * @Purpose:      ��ȡ����flash���ݲ���ʾ��ÿ����ʾ1K������
  * @param:        _uiAddr:����flash��ȡ���ݵ�ַ
  * @return:       none
*******************************************************************************/
void sfViewData(uint32_t _uiAddr)
{
	uint16_t i;

	sf_ReadBuffer(buf,_uiAddr,1024);	/* ������ */
	printf("��ַ��0x%08X; ���ݳ��� = 1024\r\n",_uiAddr);

	/* ��ӡ���� */
	for (i = 0; i < 1024; ++i)
		{
			printf(" %02X",buf[i]);

			if ((i & 31) == 31)
				{
					printf("\r\n");		/* ÿ����ʾ16�ֽ����� */
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

	/* ��ʼ��ַ = 0�� ���ݳ���Ϊ 256 */
	iTime1 = bsp_GetRunTime();	/* ���¿�ʼʱ�� */
	uiAddr = 0;
	for (i = 0; i < g_tSF.TotalSize / TEST_SIZE; i++, uiAddr += TEST_SIZE)
	{
		sf_ReadBuffer(buf, uiAddr, TEST_SIZE);
	}
	iTime2 = bsp_GetRunTime();	/* ���½���ʱ�� */

	/* ��ӡ���ٶ� */
	printf("���ݳ���: %d�ֽ�, ����ʱ: %dms, ���ٶ�: %lld Bytes/s\r\n", g_tSF.TotalSize, iTime2 - iTime1, (uint64_t)g_tSF.TotalSize * 1000 / (iTime2 - iTime1));
}



static void sfDispMenu(void)
{
	printf("\r\n*******************************************\r\n");
	printf("��ѡ���������:\r\n");
	printf("��1 - ������Flash, ��ַ:0x%X,����:%d�ֽڡ�\r\n", TEST_ADDR, TEST_SIZE);
	printf("��2 - д����Flash, ��ַ:0x%X,����:%d�ֽڡ�\r\n", TEST_ADDR, TEST_SIZE);
	printf("��3 - ������������Flash��\r\n");
	printf("��4 - д��������Flash, ȫ0x55��\r\n");
	printf("��5 - ����������Flash, ���Զ��ٶȡ�\r\n");
	printf("��Z - ��ȡǰ1K����ַ�Զ����١�\r\n");
	printf("��X - ��ȡ��1K����ַ�Զ����ӡ�\r\n");
	printf("��������� - ��ʾ������ʾ\r\n");
	printf("\r\n");
}
/*******************************************************************************
  * @FunctionName: DemoSpiFlash
  * @Author:       trx
  * @DateTime:     2022�?�?6�?1:24:26 
  * @Purpose:      
  * @param:        void               
  * @return:       none
*******************************************************************************/
void DemoSpiFlash(void)
{
	uint8_t cmd;
	uint32_t uiReadPageNo = 0;
	/* ��⴮��Flash OK */
	printf("��⵽����Flash��ID= %08X,�ͺţ�%s \r\n", g_tSF.ChipID , g_tSF.ChipName);
	printf("    ���� : %dM�ֽ�, ��������: %d�ֽ�\r\n", g_tSF.TotalSize/(1024*1024), g_tSF.SectorSize);

	sfDispMenu();		/* ��ӡ������ʾ */

	while(1)
	{
		if (comGetChar(COM1,&cmd))	/* �Ӵ��ڶ���һ���ַ�(��������ʽ) */
		{
			switch (cmd)
			{
				case '1':
					printf("\r\n��1 - ������Flash, ��ַ:0x%X,����:%d�ֽڡ�\r\n", TEST_ADDR, TEST_SIZE);
					sfReadTest();	/* ������Flash���ݣ�����ӡ������������ */
					break;

				case '2':
					printf("\r\n��2 - ����ָ������, ��ַ:0x%X,����:%d�ֽڡ�\r\n", TEST_ADDR, TEST_SIZE);
					sfWriteTest();
					break;
				case '3':
					printf("\r\n��3 - ������������Flash��\r\n");
					printf("����Flash������ϴ����Ҫ20�����ң������ĵȴ�");
					sfErase();		/* ��������Flash���ݣ�ʵ���Ͼ���д��ȫ0xFF */
					break;

				case '4':
					printf("\r\n��4 - д��������Flash, ȫ0x55��\r\n");
					printf("����Flashд����ϴ����Ҫ20�����ң������ĵȴ�");
					sfWriteAll(0x55);/* ��������Flash���ݣ�ʵ���Ͼ���д��ȫ0xFF */
					break;

				case '5':
					printf("\r\n��5 - ����������Flash, %dM�ֽڡ�\r\n", g_tSF.TotalSize/(1024*1024));
					sfTestReadSpeed(); /* ����������Flash���ݣ������ٶ� */
					break;

				case 'z':
				case 'Z': /* ��ȡǰ1K */
					if (uiReadPageNo > 0)
					{
						uiReadPageNo--;
					}
					else
					{
						printf("�Ѿ�����ǰ\r\n");
					}
					sfViewData(uiReadPageNo * 1024);
					break;

				case 'x':
				case 'X': /* ��ȡ��1K */
					if (uiReadPageNo < g_tSF.TotalSize / 1024 - 1)
					{
						uiReadPageNo++;
					}
					else
					{
						printf("�Ѿ������\r\n");
					}
					sfViewData(uiReadPageNo * 1024);
					break;
				default:
					sfDispMenu();	/* ��Ч������´�ӡ������ʾ */
					break;

			}
		}
	}
	
}


#endif

