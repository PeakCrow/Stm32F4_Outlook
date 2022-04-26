#include "bsp_spi_flash_demo.h"

#define TEST_ADDR	0	/* ¶ÁĞ´²âÊÔµØÖ· */
#define	TEST_SIZE	4096/* ¶ÁĞ´²âÊÔÊı¾İ´óĞ¡ */



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
  * @DateTime:     2022Äê4ÔÂ25ÈÕ21:57:20 
  * @Purpose:      ²âÊÔ´®ĞĞflash¶ÁÈ¡ËÙ¶È£¬¶ÁÈ¡Õû¸ö´®ĞĞflashµÄÊı¾İ£¬×îºó´òÓ¡½á¹û
  * @param:        void
  * @return:       none
*******************************************************************************/
static void sfReadTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* ÆğÊ¼µØÖ· = 0£¬Êı¾İ³¤¶ÈÎª256 */
	iTime1 = bsp_GetRunTime();		/* ¼ÇÏÂ¿ªÊ¼Ê±¼ä */
	sf_ReadBuffer(buf,TEST_ADDR,TEST_SIZE);
	iTime2 = bsp_GetRunTime();		/* ¼ÇÏÂ½áÊøÊ±¼ä */
	printf("¶Á´®ĞĞflash³É¹¦£¬Êı¾İÈçÏÂ£º\r\n");

	/* ´òÓ¡Êı¾İ */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			printf(" %02x",buf[i]);

			if ((i & 31) == 31)
				{
					printf("\r\n");	/* Ã¿ĞĞÏÔÊ¾16×Ö½ÚÊı¾İ */
				}
			else if((i & 31) == 15)
				{
					printf(" - ");
				}
		}

	/* ´òÓ¡¶ÁÈ¡ËÙ¶È */
	printf("Êı¾İ³¤¶È:%d×Ö½Ú£¬¶ÁÈ¡ºÄÊ±:%dms,¶ÁÈ¡ËÙ¶È:%d Bytes/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}

/*******************************************************************************
  * @FunctionName: sfWriteTest
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ22:15:30 
  * @Purpose:      xĞ´´®ĞĞflash²âÊÔ
  * @param:        void
  * @return:       none
*******************************************************************************/
static void sfWriteTest(void)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* Ìî³ä²âÊÔ»º³åÇø */
	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = i;
		}

	/* Ğ´eeprom£¬ÆğÊ¼µØÖ· = 0£¬Êı¾İ³¤¶ÈÎª256 */
	iTime1 = bsp_GetRunTime();	/* ¼ÇÏÂ¿ªÊ¼Ê±¼ä */
	if (sf_WriteBuffer(buf,TEST_ADDR,TEST_SIZE) == 0)
		{
			printf("Ğ´´®ĞĞflash³ö´í£¡\r\n");
			return ;
		}
	else
		{
			iTime2 = bsp_GetRunTime();/* ¼ÇÏÂ½áÊøÊ±¼ä */
			printf("Ğ´´®ĞĞflash³É¹¦£¡\r\n");
		}

	/* ´òÓ¡¶ÁÈ¡ËÙ¶È */
	printf("Êı¾İ³¤¶È£º%d×Ö½Ú£¬Ğ´ºÄÊ±£º%dms£¬Ğ´ËÙ¶È£º%dB/s\r\n",TEST_SIZE,iTime2 - iTime1,(TEST_SIZE * 1000) / (iTime2 - iTime1));
}


/*******************************************************************************
  * @FunctionName: sfWriteAll
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ22:17:03 
  * @Purpose:      Ğ´´®ĞĞflashÈ«²¿Êı¾İ
  * @param:        _ch£ºÊı¾İÄÚÈİ
  * @return:       none
*******************************************************************************/
void sfWriteAll(uint8_t _ch)
{
	uint16_t i;
	int32_t iTime1,iTime2;

	/* Ìî³ä²âÊÔ»º³åÇø */

	for (i = 0; i < TEST_SIZE; ++i)
		{
			buf[i] = _ch;
		}

	/* Ğ´eeprom£¬ÆğÊ¼µØÖ· = 0£¬Êı¾İ³¤¶ÈÎª256 */
	iTime1 = bsp_GetRunTime();		/* ¼ÇÏÂ¿ªÊ¼Ê±¼ä */
	for (i = 0; i < g_tSF.TotalSize / g_tSF.SectorSize; ++i)
		{
			if(sf_WriteBuffer(buf,i * g_tSF.SectorSize,g_tSF.SectorSize) == 0)
				{
					printf("Ğ´´®ĞĞflash³ö´í£¡\r\n");
					return;
				}
			printf(".");
			if (((i + 1) & 128) == 0)
				{
					printf("\r\n");
				}
		}
	iTime2 = bsp_GetRunTime();		/* ¼ÇÏÂ½áÊøÊ±¼ä */

	/* ´òÓ¡¶ÁÈ¡ËÙ¶È */
	printf("Êı¾İ³¤¶È£º%dK×Ö½Ú£¬Ğ´ºÄÊ±£º%dms,Ğ´ËÙ¶È£º%dB/s\r\n",g_tSF.TotalSize / 1024,iTime2 - iTime1,(g_tSF.TotalSize * 1000) / (iTime2 - iTime1));
}
/*******************************************************************************
  * @FunctionName: sfErase
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ22:31:35 
  * @Purpose:      ²Á³ı´®ĞĞflash²âÊÔ
  * @param:        void
  * @return:       none
*******************************************************************************/
void sfErase(void)
{
	int32_t iTime1,iTime2;

	iTime1 = bsp_GetRunTime();	/* ¼ÇÏÂ¿ªÊ¼Ê±¼ä */
	sf_EraseChip();
	iTime2 = bsp_GetRunTime();	/* ¼ÇÏÂ½áÊøÊ±¼ä */

	/* ´òÓ¡²Á³ıËÙ¶È */
	printf("²Á³ı´®ĞĞflashÍê³É£¡ºÄÊ±£º%dms\r\n",iTime2 - iTime1);
	return;
}

/*******************************************************************************
  * @FunctionName: sfViewData
  * @Author:       trx
  * @DateTime:     2022Äê4ÔÂ25ÈÕ22:35:53 
  * @Purpose:      ¶ÁÈ¡´®ĞĞflashÊı¾İ²¢ÏÔÊ¾£¬Ã¿´ÎÏÔÊ¾1KµÄÄÚÈİ
  * @param:        _uiAddr:´®ĞĞflash¶ÁÈ¡Êı¾İµØÖ·
  * @return:       none
*******************************************************************************/
void sfViewData(uint32_t _uiAddr)
{
	uint16_t i;

	sf_ReadBuffer(buf,_uiAddr,1024);	/* ¶ÁÊı¾İ */
	printf("µØÖ·£º0x%08X; Êı¾İ³¤¶È = 1024\r\n",_uiAddr);

	/* ´òÓ¡Êı¾İ */
	for (i = 0; i < 1024; ++i)
		{
			printf(" %02X",buf[i]);

			if ((i & 31) == 31)
				{
					printf("\r\n");		/* Ã¿ĞĞÏÔÊ¾16×Ö½ÚÊı¾İ */
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

	/* ÆğÊ¼µØÖ· = 0£¬ Êı¾İ³¤¶ÈÎª 256 */
	iTime1 = bsp_GetRunTime();	/* ¼ÇÏÂ¿ªÊ¼Ê±¼ä */
	uiAddr = 0;
	for (i = 0; i < g_tSF.TotalSize / TEST_SIZE; i++, uiAddr += TEST_SIZE)
	{
		sf_ReadBuffer(buf, uiAddr, TEST_SIZE);
	}
	iTime2 = bsp_GetRunTime();	/* ¼ÇÏÂ½áÊøÊ±¼ä */

	/* ´òÓ¡¶ÁËÙ¶È */
	printf("Êı¾İ³¤¶È: %d×Ö½Ú, ¶ÁºÄÊ±: %dms, ¶ÁËÙ¶È: %lld Bytes/s\r\n", g_tSF.TotalSize, iTime2 - iTime1, (uint64_t)g_tSF.TotalSize * 1000 / (iTime2 - iTime1));
}



static void sfDispMenu(void)
{
	printf("\r\n*******************************************\r\n");
	printf("ÇëÑ¡Ôñ²Ù×÷ÃüÁî:\r\n");
	printf("¡¾1 - ¶Á´®ĞĞFlash, µØÖ·:0x%X,³¤¶È:%d×Ö½Ú¡¿\r\n", TEST_ADDR, TEST_SIZE);
	printf("¡¾2 - Ğ´´®ĞĞFlash, µØÖ·:0x%X,³¤¶È:%d×Ö½Ú¡¿\r\n", TEST_ADDR, TEST_SIZE);
	printf("¡¾3 - ²Á³ıÕû¸ö´®ĞĞFlash¡¿\r\n");
	printf("¡¾4 - Ğ´Õû¸ö´®ĞĞFlash, È«0x55¡¿\r\n");
	printf("¡¾5 - ¶ÁÕû¸ö´®ĞĞFlash, ²âÊÔ¶ÁËÙ¶È¡¿\r\n");
	printf("¡¾Z - ¶ÁÈ¡Ç°1K£¬µØÖ·×Ô¶¯¼õÉÙ¡¿\r\n");
	printf("¡¾X - ¶ÁÈ¡ºó1K£¬µØÖ·×Ô¶¯Ôö¼Ó¡¿\r\n");
	printf("ÆäËûÈÎÒâ¼ü - ÏÔÊ¾ÃüÁîÌáÊ¾\r\n");
	printf("\r\n");
}
/*******************************************************************************
  * @FunctionName: DemoSpiFlash
  * @Author:       trx
  * @DateTime:     2022å¹´4æœˆ26æ—¥21:24:26 
  * @Purpose:      
  * @param:        void               
  * @return:       none
*******************************************************************************/
void DemoSpiFlash(void)
{
	uint8_t cmd;
	uint32_t uiReadPageNo = 0;

	
	/* ¼ì²â´®ĞĞFlash OK */
	printf("¼ì²âµ½´®ĞĞFlash£¬ID= %08X,ĞÍºÅ£º%s \r\n", g_tSF.ChipID , g_tSF.ChipName);
	printf("    ÈİÁ¿ : %dM×Ö½Ú, ÉÈÇøÈİÁ¿: %d×Ö½Ú\r\n", g_tSF.TotalSize/(1024*1024), g_tSF.SectorSize);

//	sfDispMenu();		/* ´òÓ¡ÃüÁîÌáÊ¾ */

//	while(1)
//	{
//		if (comGetChar(COM1, &cmd))	/* ´Ó´®¿Ú¶ÁÈëÒ»¸ö×Ö·û(·Ç×èÈû·½Ê½) */
//		{
//			switch (cmd)
//			{
//				case '1':
//					printf("\r\n¡¾1 - ¶Á´®ĞĞFlash, µØÖ·:0x%X,³¤¶È:%d×Ö½Ú¡¿\r\n", TEST_ADDR, TEST_SIZE);
//					sfReadTest();	/* ¶Á´®ĞĞFlashÊı¾İ£¬²¢´òÓ¡³öÀ´Êı¾İÄÚÈİ */
//					break;

//				case '2':
//					printf("\r\n¡¾2 - Ğ´´®ĞĞFlash, µØÖ·:0x%X,³¤¶È:%d×Ö½Ú¡¿\r\n", TEST_ADDR, TEST_SIZE);
//					sfWriteTest();	/* Ğ´´®ĞĞFlashÊı¾İ£¬²¢´òÓ¡Ğ´ÈëËÙ¶È */
//					break;

//				case '3':
//					printf("\r\n¡¾3 - ²Á³ıÕû¸ö´®ĞĞFlash¡¿\r\n");
//					printf("Õû¸öFlash²Á³ıÍê±Ï´ó¸ÅĞèÒª20Ãë×óÓÒ£¬ÇëÄÍĞÄµÈ´ı");
//					sfErase();		/* ²Á³ı´®ĞĞFlashÊı¾İ£¬Êµ¼ÊÉÏ¾ÍÊÇĞ´ÈëÈ«0xFF */
//					break;

//				case '4':
//					printf("\r\n¡¾4 - Ğ´Õû¸ö´®ĞĞFlash, È«0x55¡¿\r\n");
//					printf("Õû¸öFlashĞ´ÈëÍê±Ï´ó¸ÅĞèÒª20Ãë×óÓÒ£¬ÇëÄÍĞÄµÈ´ı");
//					sfWriteAll(0x55);/* ²Á³ı´®ĞĞFlashÊı¾İ£¬Êµ¼ÊÉÏ¾ÍÊÇĞ´ÈëÈ«0xFF */
//					break;

//				case '5':
//					printf("\r\n¡¾5 - ¶ÁÕû¸ö´®ĞĞFlash, %dM×Ö½Ú¡¿\r\n", g_tSF.TotalSize/(1024*1024));
//					sfTestReadSpeed(); /* ¶ÁÕû¸ö´®ĞĞFlashÊı¾İ£¬²âÊÔËÙ¶È */
//					break;

//				case 'z':
//				case 'Z': /* ¶ÁÈ¡Ç°1K */
//					if (uiReadPageNo > 0)
//					{
//						uiReadPageNo--;
//					}
//					else
//					{
//						printf("ÒÑ¾­ÊÇ×îÇ°\r\n");
//					}
//					sfViewData(uiReadPageNo * 1024);
//					break;

//				case 'x':
//				case 'X': /* ¶ÁÈ¡ºó1K */
//					if (uiReadPageNo < g_tSF.TotalSize / 1024 - 1)
//					{
//						uiReadPageNo++;
//					}
//					else
//					{
//						printf("ÒÑ¾­ÊÇ×îºó\r\n");
//					}
//					sfViewData(uiReadPageNo * 1024);
//					break;

//				default:
//					sfDispMenu();	/* ÎŞĞ§ÃüÁî£¬ÖØĞÂ´òÓ¡ÃüÁîÌáÊ¾ */
//					break;

//			}
//		}
//	}
	
}



