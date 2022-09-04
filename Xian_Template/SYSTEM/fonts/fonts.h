/*										
*********************************************************************************************************
*	                                  
*	模块名称 : 字库模块
*	文件名称 : fonts.h
*	版    本 : V1.1
*	说    明 : 头文件
*
*	Copyright (C), 2010-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __FONTS_H_
#define __FONTS_H_

//#define USE_SMALL_FONT	/* 定义此行表示使用CPU内嵌小字库， 这个宏只在bsp_tft+lcd.c中使用 */
#define	USE_SPI_FONT		/* 定义此行表示使用串行Flash全字库 */
//#define	USE_NOR_FONT		/* 定义此行表示使用NOR Flash全字库 */

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

#endif
