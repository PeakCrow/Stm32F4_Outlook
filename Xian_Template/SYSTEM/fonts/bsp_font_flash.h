#ifndef __BSP_FONT_FLASH_H_
#define __BSP_FONT_FLASH_H_
#include "sys.h"

#define	USE_SPI_FONT		/* ������б�ʾʹ�ô���Flashȫ�ֿ� */

#ifdef USE_NOR_FONT		/* SPI����Flash�洢ȫ�ֿ� */
	#define ZK_BASE_ADDR	0x64000000	/* �ֿ����ַ, V5������ָ��NOR Flash�׵�ַ */
#endif

#ifdef USE_SPI_FONT		/* SPI����Flash�洢ȫ�ֿ� */
	#define ZK_BASE_ADDR	0x00000000	/* �ֿ����ַ, V5������ָ��NOR Flash�׵�ַ */
#endif


#define HZK12_ADDR (ZK_BASE_ADDR + 0x0)		/* 12�����ֿ��ַ */
#define HZK16_ADDR (ZK_BASE_ADDR + 0x2C9D0)	/* 16�����ֿ��ַ */
#define HZK24_ADDR (ZK_BASE_ADDR + 0x68190)	/* 24�����ֿ��ַ */
#define HZK32_ADDR (ZK_BASE_ADDR + 0xEDF00)	/* 32�����ֿ��ַ */


#define ASC12_ADDR (ZK_BASE_ADDR + 0x1DBE00)	/* 12�����ֿ��ַ */
#define ASC16_ADDR (ZK_BASE_ADDR + 0x1DD780)	/* 16�����ֿ��ַ */
#define ASC24_ADDR (ZK_BASE_ADDR + 0x1DFF00)	/* 24�����ֿ��ַ */
#define ASC32_ADDR (ZK_BASE_ADDR + 0x1E5A50)	/* 32�����ֿ��ַ */

/* CPU����С�ֿ�ĳ������� */
extern unsigned char const g_Ascii12[];		/* 12���� ASCII,ȫ�ַ� */
extern unsigned char const g_Ascii16[];		/* 16���� ASCII,ȫ�ַ� */
extern unsigned char const g_Ascii24[];		/* 24���� ASCII,�����ַ� */
extern unsigned char const g_Ascii32[];		/* 32���� ASCII,�����ַ� */

extern unsigned char const g_Ascii24_VarWidth[];
extern unsigned char const g_Ascii32_VarWidth[];

extern unsigned char const g_Ascii62x40[];	/* 62 * 40 ASCII,�����ַ� */
extern unsigned char const g_Ascii96x40[];	/* 96 * 40 ASCII,�����ַ� */

extern unsigned char const g_Hz12[];		/* 12�����ֵ���,���ֺ��� */	
extern unsigned char const g_Hz16[];		/* 16�����ֵ���,���ֺ��� */	
extern unsigned char const g_Hz24[];		/* 24�����ֵ���,���ֺ��� */	
extern unsigned char const g_Hz32[];		/* 32�����ֵ���,���ֺ��� */	

/* RA8875�ǵȿ��ַ��Ŀ�Ȳ��ұ� */
extern unsigned char const g_RA8875_Ascii12_width[];
extern unsigned char const g_RA8875_Ascii16_width[];
extern unsigned char const g_RA8875_Ascii24_width[];
extern unsigned char const g_RA8875_Ascii32_width[];
enum
{
	RA_FONT_16 = 0,		/* RA8875 ���� 16���� */
	RA_FONT_24 = 1,		/* RA8875 ���� 24���� */
	RA_FONT_32 = 2		/* RA8875 ���� 32���� */
};
/* ���ֶ��뷽ʽ */
enum
{
	ALIGN_LEFT = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2
};

/*
	LCD ��ɫ���룬CL_��Color�ļ�д
	16Bit�ɸ�λ����λ�� RRRR RGGG GGGB BBBB

	�����RGB �꽫24λ��RGBֵת��Ϊ16λ��ʽ��
	����windows�Ļ��ʳ��򣬵���༭��ɫ��ѡ���Զ�����ɫ�����Ի�õ�RGBֵ��

	�Ƽ�ʹ������ȡɫ���������㿴���Ľ�����ɫ��
*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
enum
{
	CL_WHITE        = RGB(255,255,255),	/* ��ɫ */
	CL_BLACK        = RGB(  0,  0,  0),	/* ��ɫ */
	CL_RED          = RGB(255,	0,  0),	/* ��ɫ */
	CL_GREEN        = RGB(  0,255,  0),	/* ��ɫ */
	CL_BLUE         = RGB(  0,	0,255),	/* ��ɫ */
	CL_YELLOW       = RGB(255,255,  0),	/* ��ɫ */

	CL_GREY			= RGB( 98, 98, 98), 	/* ���ɫ */
	CL_GREY1		= RGB( 150, 150, 150), 	/* ǳ��ɫ */
	CL_GREY2		= RGB( 180, 180, 180), 	/* ǳ��ɫ */
	CL_GREY3		= RGB( 200, 200, 200), 	/* ��ǳ��ɫ */
	CL_GREY4		= RGB( 230, 230, 230), 	/* ��ǳ��ɫ */

	CL_BUTTON_GREY	= RGB( 220, 220, 220), /* WINDOWS ��ť�����ɫ */

	CL_MAGENTA      = 0xF81F,	/* ����ɫ�����ɫ */
	CL_CYAN         = 0x7FFF,	/* ����ɫ����ɫ */

	CL_BLUE1        = RGB(  0,  0, 240),		/* ����ɫ */
	CL_BLUE2        = RGB(  0,  0, 128),		/* ����ɫ */
	CL_BLUE3        = RGB(  68, 68, 255),		/* ǳ��ɫ1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* ǳ��ɫ1 */

	/* UI ���� Windows�ؼ�����ɫ */
	CL_BTN_FACE		= RGB(236, 233, 216),	/* ��ť������ɫ(��) */
	
	CL_BTN_FONT		= CL_BLACK,				/* ��ť������ɫ���ڣ� */
	
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* �����������ɫ */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* �������Ӱ����ɫ */


	CL_MASK			= 0x9999,	/* ��ɫ���룬�������ֱ���͸�� */
	
	
	
	/* ���� */
	CL_BLUE5        = RGB(70, 130, 180),		/* ��ɫ5(��ɫ) */
	CL_BLUE6        = RGB(0, 255, 255),		/* ��ɫ6(���ο�ɫ) */
	
	CL_GREY5		= RGB( 210, 220, 220), 	/* ��ť�� */
	CL_GREY6		= RGB( 224, 238, 238), 	/* ��ť�� */
	
	/* ������ʾ */
	CL_BLUE7		= RGB( 200, 255, 255), 	/* ��ť�� */
	CL_BLUE8		= RGB( 191, 238, 255), 	/* ��ť�� */
	CL_YELLOW2		= RGB( 255, 246, 143),  /* ����ɫ */
};
/* ������� */
typedef enum
{
	FC_ST_12 = 0,		/* ����12x12���� ����x�ߣ� */
	FC_ST_16,			/* ����15x16���� ����x�ߣ� */
	FC_ST_24,			/* ����24x24���� ����x�ߣ� -- ��ʱδ֧�� */
	FC_ST_32,			/* ����32x32���� ����x�ߣ� -- ��ʱδ֧�� */	

	FC_ST_62X40,		/* 64x40����(��62����40)��ASCII�ַ� */
	FC_ST_96X40,		/* 96x40����(��96����40)��ASCII�ַ� */
	
	FC_RA8875_16,		/* RA8875 �������� 16���� */
	FC_RA8875_24,		/* RA8875 �������� 24���� */
	FC_RA8875_32		/* RA8875 �������� 32���� */	
}FONT_CODE_E;
/* �������Խṹ, ����LCD_DispStr() */
typedef struct
{
	FONT_CODE_E FontCode;	/* ������� FONT_CODE_E  */
	uint16_t FrontColor;/* ������ɫ */
	uint16_t BackColor;	/* ���ֱ�����ɫ��͸�� */
	uint16_t Space;		/* ���ּ�࣬��λ = ���� */
}FONT_T;

void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont);
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetFontWidth(FONT_T *_tFont);
uint16_t LCD_GetFontHeight(FONT_T *_tFont);
#endif



