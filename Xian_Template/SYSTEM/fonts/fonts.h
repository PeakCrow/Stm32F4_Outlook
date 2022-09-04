/*										
*********************************************************************************************************
*	                                  
*	ģ������ : �ֿ�ģ��
*	�ļ����� : fonts.h
*	��    �� : V1.1
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2010-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __FONTS_H_
#define __FONTS_H_

//#define USE_SMALL_FONT	/* ������б�ʾʹ��CPU��ǶС�ֿ⣬ �����ֻ��bsp_tft+lcd.c��ʹ�� */
#define	USE_SPI_FONT		/* ������б�ʾʹ�ô���Flashȫ�ֿ� */
//#define	USE_NOR_FONT		/* ������б�ʾʹ��NOR Flashȫ�ֿ� */

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

#endif
