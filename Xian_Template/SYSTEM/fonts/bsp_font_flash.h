#ifndef __BSP_FONT_FLASH_H_
#define __BSP_FONT_FLASH_H_
#include "sys.h"

#define	USE_SPI_FONT		/* 定义此行表示使用串行Flash全字库 */

#ifdef USE_NOR_FONT		/* SPI串行Flash存储全字库 */
	#define ZK_BASE_ADDR	0x64000000	/* 字库基地址, V5开发板指向NOR Flash首地址 */
#endif

#ifdef USE_SPI_FONT		/* SPI串行Flash存储全字库 */
	#define ZK_BASE_ADDR	0x00000000	/* 字库基地址, V5开发板指向NOR Flash首地址 */
#endif


#define HZK12_ADDR (ZK_BASE_ADDR + 0x0)		/* 12点阵汉字库地址 */
#define HZK16_ADDR (ZK_BASE_ADDR + 0x2C9D0)	/* 16点阵汉字库地址 */
#define HZK24_ADDR (ZK_BASE_ADDR + 0x68190)	/* 24点阵汉字库地址 */
#define HZK32_ADDR (ZK_BASE_ADDR + 0xEDF00)	/* 32点阵汉字库地址 */


#define ASC12_ADDR (ZK_BASE_ADDR + 0x1DBE00)	/* 12点阵汉字库地址 */
#define ASC16_ADDR (ZK_BASE_ADDR + 0x1DD780)	/* 16点阵汉字库地址 */
#define ASC24_ADDR (ZK_BASE_ADDR + 0x1DFF00)	/* 24点阵汉字库地址 */
#define ASC32_ADDR (ZK_BASE_ADDR + 0x1E5A50)	/* 32点阵汉字库地址 */

/* CPU内置小字库的常量数组 */
extern unsigned char const g_Ascii12[];		/* 12点阵 ASCII,全字符 */
extern unsigned char const g_Ascii16[];		/* 16点阵 ASCII,全字符 */
extern unsigned char const g_Ascii24[];		/* 24点阵 ASCII,部分字符 */
extern unsigned char const g_Ascii32[];		/* 32点阵 ASCII,部分字符 */

extern unsigned char const g_Ascii24_VarWidth[];
extern unsigned char const g_Ascii32_VarWidth[];

extern unsigned char const g_Ascii62x40[];	/* 62 * 40 ASCII,部分字符 */
extern unsigned char const g_Ascii96x40[];	/* 96 * 40 ASCII,部分字符 */

extern unsigned char const g_Hz12[];		/* 12点阵汉字点阵,部分汉字 */	
extern unsigned char const g_Hz16[];		/* 16点阵汉字点阵,部分汉字 */	
extern unsigned char const g_Hz24[];		/* 24点阵汉字点阵,部分汉字 */	
extern unsigned char const g_Hz32[];		/* 32点阵汉字点阵,部分汉字 */	

/* RA8875非等宽字符的宽度查找表 */
extern unsigned char const g_RA8875_Ascii12_width[];
extern unsigned char const g_RA8875_Ascii16_width[];
extern unsigned char const g_RA8875_Ascii24_width[];
extern unsigned char const g_RA8875_Ascii32_width[];
enum
{
	RA_FONT_16 = 0,		/* RA8875 字体 16点阵 */
	RA_FONT_24 = 1,		/* RA8875 字体 24点阵 */
	RA_FONT_32 = 2		/* RA8875 字体 32点阵 */
};
/* 文字对齐方式 */
enum
{
	ALIGN_LEFT = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2
};

/*
	LCD 颜色代码，CL_是Color的简写
	16Bit由高位至低位， RRRR RGGG GGGB BBBB

	下面的RGB 宏将24位的RGB值转换为16位格式。
	启动windows的画笔程序，点击编辑颜色，选择自定义颜色，可以获得的RGB值。

	推荐使用迷你取色器软件获得你看到的界面颜色。
*/
#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* 将8位R,G,B转化为 16位RGB565格式 */
enum
{
	CL_WHITE        = RGB(255,255,255),	/* 白色 */
	CL_BLACK        = RGB(  0,  0,  0),	/* 黑色 */
	CL_RED          = RGB(255,	0,  0),	/* 红色 */
	CL_GREEN        = RGB(  0,255,  0),	/* 绿色 */
	CL_BLUE         = RGB(  0,	0,255),	/* 蓝色 */
	CL_YELLOW       = RGB(255,255,  0),	/* 黄色 */

	CL_GREY			= RGB( 98, 98, 98), 	/* 深灰色 */
	CL_GREY1		= RGB( 150, 150, 150), 	/* 浅灰色 */
	CL_GREY2		= RGB( 180, 180, 180), 	/* 浅灰色 */
	CL_GREY3		= RGB( 200, 200, 200), 	/* 最浅灰色 */
	CL_GREY4		= RGB( 230, 230, 230), 	/* 最浅灰色 */

	CL_BUTTON_GREY	= RGB( 220, 220, 220), /* WINDOWS 按钮表面灰色 */

	CL_MAGENTA      = 0xF81F,	/* 红紫色，洋红色 */
	CL_CYAN         = 0x7FFF,	/* 蓝绿色，青色 */

	CL_BLUE1        = RGB(  0,  0, 240),		/* 深蓝色 */
	CL_BLUE2        = RGB(  0,  0, 128),		/* 深蓝色 */
	CL_BLUE3        = RGB(  68, 68, 255),		/* 浅蓝色1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* 浅蓝色1 */

	/* UI 界面 Windows控件常用色 */
	CL_BTN_FACE		= RGB(236, 233, 216),	/* 按钮表面颜色(灰) */
	
	CL_BTN_FONT		= CL_BLACK,				/* 按钮字体颜色（黑） */
	
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* 分组框主线颜色 */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* 分组框阴影线颜色 */


	CL_MASK			= 0x9999,	/* 颜色掩码，用于文字背景透明 */
	
	
	
	/* 新增 */
	CL_BLUE5        = RGB(70, 130, 180),		/* 蓝色5(底色) */
	CL_BLUE6        = RGB(0, 255, 255),		/* 蓝色6(波形框色) */
	
	CL_GREY5		= RGB( 210, 220, 220), 	/* 按钮灰 */
	CL_GREY6		= RGB( 224, 238, 238), 	/* 按钮灰 */
	
	/* 按下显示 */
	CL_BLUE7		= RGB( 200, 255, 255), 	/* 按钮蓝 */
	CL_BLUE8		= RGB( 191, 238, 255), 	/* 按钮蓝 */
	CL_YELLOW2		= RGB( 255, 246, 143),  /* 淡黄色 */
};
/* 字体代码 */
typedef enum
{
	FC_ST_12 = 0,		/* 宋体12x12点阵 （宽x高） */
	FC_ST_16,			/* 宋体15x16点阵 （宽x高） */
	FC_ST_24,			/* 宋体24x24点阵 （宽x高） -- 暂时未支持 */
	FC_ST_32,			/* 宋体32x32点阵 （宽x高） -- 暂时未支持 */	

	FC_ST_62X40,		/* 64x40点阵(高62，宽40)，ASCII字符 */
	FC_ST_96X40,		/* 96x40点阵(高96，宽40)，ASCII字符 */
	
	FC_RA8875_16,		/* RA8875 内置字体 16点阵 */
	FC_RA8875_24,		/* RA8875 内置字体 24点阵 */
	FC_RA8875_32		/* RA8875 内置字体 32点阵 */	
}FONT_CODE_E;
/* 字体属性结构, 用于LCD_DispStr() */
typedef struct
{
	FONT_CODE_E FontCode;	/* 字体代码 FONT_CODE_E  */
	uint16_t FrontColor;/* 字体颜色 */
	uint16_t BackColor;	/* 文字背景颜色，透明 */
	uint16_t Space;		/* 文字间距，单位 = 像素 */
}FONT_T;

void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont);
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor);
uint16_t LCD_GetFontWidth(FONT_T *_tFont);
uint16_t LCD_GetFontHeight(FONT_T *_tFont);
#endif



