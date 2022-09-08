#include "bsp_font_flash.h"

static uint8_t SeachStr_a(char *_ptr);
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align);
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf);
	static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
/*
*********************************************************************************************************
*	函 数 名: LCD_DispStr
*	功能说明: 在LCD指定坐标（左上角）显示一个字符串
*	形    参:
*		_usX : X坐标
*		_usY : Y坐标
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
	LCD_DispStrEx0(_usX, _usY, _ptr, _tFont, 0, 0);
}
/*
*********************************************************************************************************
*	函 数 名: LCD_DispStrEx0
*	功能说明: 在LCD指定坐标（左上角）显示一个字符串。 增强型函数。支持左\中\右对齐，支持定长清屏。
*	形    参:
*		_usX : X坐标
*		_usY : Y坐标
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数。可以指定RA8875字库显示汉字
*		_Width : 字符串显示区域的宽度. 0 表示不处理留白区域，此时_Align无效
*		_Align :字符串在显示区域的对齐方式，
*				ALIGN_LEFT = 0,
*				ALIGN_CENTER = 1,
*				ALIGN_RIGHT = 2
*	返 回 值: 无
*********************************************************************************************************
*/
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align)
{
	uint32_t i;
	uint8_t code1;
	uint8_t code2;
	//uint8_t buf[32 * 32 / 8];	/* 最大支持32点阵汉字 */
	uint8_t buf[96 * 40 / 8];	/* 最大支持96x40点阵字符 */
	uint8_t width;
	uint16_t m;
	uint8_t font_width = 0;
	uint8_t font_height = 0;
	uint16_t x, y;
	uint16_t offset;
	uint16_t str_width;	/* 字符串实际宽度  */
	uint8_t ra8875_use = 0;
	uint8_t ra8875_font_code = 0;
	uint16_t address;
	uint8_t a_flag = 0;
	uint8_t RA8875_flag = 0;
	
	uint8_t line_bytes;
	uint8_t asc_bytes = 0;
	uint8_t hz_bytes = 0;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_height = 12;
			font_width = 12;
			asc_bytes = 1;
			hz_bytes = 2;
			break;
		
		case FC_ST_16:
			font_height = 16;
			font_width = 16;
			asc_bytes = 1;
			hz_bytes = 2;
			break;

		case FC_ST_24:
			font_height = 24;
			font_width = 24;
			asc_bytes = 2;
			hz_bytes = 3;
			break;
						
		case FC_ST_32:	
			font_height = 32;
			font_width = 32;
			asc_bytes = 2;
			hz_bytes = 4;
			break;		

		case FC_ST_62X40:	
			font_height = 62;
			font_width = 80;
			asc_bytes = 5;
			hz_bytes = 10;
			break;			

		case FC_ST_96X40:	
			font_height = 96;
			font_width = 80;
			asc_bytes = 5;
			hz_bytes = 10;
			break;		
		
		case FC_RA8875_16:
			ra8875_font_code = RA_FONT_16;
			ra8875_use = 1;	/* 表示用RA8875字库 */
			break;
			
		case FC_RA8875_24:
			ra8875_font_code = RA_FONT_24;
			a_flag = SeachStr_a(_ptr);	/* 搜索字符串中是否有'\a' */
			if (a_flag == 0)	/**/
			{
				ra8875_use = 1;	/* 表示用RA8875字库 */
			}
			else
			{
				ra8875_use = 0;	/* 表示不用RA8875字库 */
				font_height = 24;
				font_width = 24;
				asc_bytes = 3;
				hz_bytes = 3;
			}
			break;
						
		case FC_RA8875_32:
			ra8875_font_code = RA_FONT_32;
			a_flag = SeachStr_a(_ptr);
			if (a_flag == 0)
			{
				ra8875_use = 1;	/* 表示用RA8875字库 */
			}
			else
			{
				ra8875_use = 0;	/* 表示不用RA8875字库 */
				font_height = 32;
				font_width = 32;
				asc_bytes = 4;
				hz_bytes = 4;
			}
			break;
	}
	
	str_width = LCD_GetStrWidth(_ptr, _tFont);	/* 计算字符串实际宽度(RA8875内部ASCII点阵宽度为变长 */
	offset = 0;
	if (_Width > str_width)
	{
		if (_Align == ALIGN_RIGHT)	/* 右对齐 */
		{
			offset = _Width - str_width;
		}
		else if (_Align == ALIGN_CENTER)	/* 左对齐 */
		{
			offset = (_Width - str_width) / 2;
		}
		else	/* 左对齐 ALIGN_LEFT */
		{
			;
		}
	}

	/* 左侧填背景色, 中间对齐和右边对齐  */
	if (offset > 0)
	{
		if (_tFont->BackColor != CL_MASK)	/* 透明色 */
		{
			LCD_Fill_Rect(_usX, _usY, LCD_GetFontHeight(_tFont), offset,  _tFont->BackColor);	
		}
		_usX += offset;
		
	}
	
	/* 右侧填背景色 */
	if (_Width > str_width)
	{
		if (_tFont->BackColor != CL_MASK)	/* 透明色 */
		{
			LCD_Fill_Rect(_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset,  _tFont->BackColor);
		}
	}
	
	/* 使用CPU内部字库. 点阵信息由CPU读取 */
	{
		/* 开始循环处理字符 */
		while (*_ptr != 0)
		{
			code1 = *_ptr;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
				
			if (code1 < 0x80)
			{
				if (a_flag == 0)
				{
					RA8875_flag = 0;
					/* 将ascii字符点阵复制到buf */
					_LCD_ReadAsciiDot(code1, _tFont->FontCode, buf);	/* 读取ASCII字符点阵 */
					
					//对秒进行特殊处理,避免宽度过大
					if (_tFont->FontCode == FC_ST_62X40 || _tFont->FontCode == FC_ST_96X40)
					{
						if (code1 == 0x5E)
						{
							width = 28;
						}
						else
						{
							width = font_width / 2;
						}	
					}
					else
					{
						width = font_width / 2;
					}
					
					
					line_bytes = asc_bytes;
				}	
				else
				{
					if (code1 == '\a')
					{
						RA8875_flag = 0;
						_ptr++;
						code1 = *_ptr;
						if (_tFont->FontCode == FC_RA8875_32)
						{
							m = 0;
							while(1)
							{
							   address = m * (128 + 2);
							   m++;
							   if (code1 == g_Ascii32_VarWidth[address + 0])
							   {
								  font_width = g_Ascii32_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* 字库搜索完毕，未找到，则填充全FF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
							}
						}
						else if (_tFont->FontCode == FC_RA8875_24)
						{
							m = 0;
							while(1)
							{
							   address = m * (72 + 2);
							   m++;
							   if (code1 == g_Ascii24_VarWidth[address + 0])
							   {
								  font_width = g_Ascii24_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* 字库搜索完毕，未找到，则填充全FF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}	
						_LCD_ReadSmallDot(code1, _tFont->FontCode, buf);
						
						width = font_width;
					
						line_bytes = asc_bytes;
					}
					else
					{
						RA8875_flag = 1;
						if (_tFont->FontCode == FC_RA8875_32)
						{
							font_width = g_RA8875_Ascii32_width[code1 - 0x20];
						}
						else if (_tFont->FontCode == FC_RA8875_24)
						{
							font_width = g_RA8875_Ascii24_width[code1 - 0x20];
						}
						width = font_width;
						line_bytes = asc_bytes;
					}
				}
			}
			else
			{
				RA8875_flag = 0;
				code2 = *++_ptr;
				if (code2 == 0)
				{
					break;
				}
				/* 读1个汉字的点阵 */
				_LCD_ReadHZDot(code1, code2, _tFont->FontCode, buf);
				width = font_width;
				line_bytes = hz_bytes;
			}
			
			y = _usY;
			if (RA8875_flag == 0)
			{
				/* 开始刷LCD */
				for (m = 0; m < font_height; m++)	/* 字符高度 */
				{
					x = _usX;
					for (i = 0; i < width; i++)	/* 字符宽度 */
					{
						if ((buf[m * line_bytes + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
						{
							LCD_PutPixel(x, y, _tFont->FrontColor);	/* 设置像素颜色为文字色 */
						}
						else
						{
							if (_tFont->BackColor != CL_MASK)	/* 透明色 */
							{
								LCD_PutPixel(x, y, _tFont->BackColor);	/* 设置像素颜色为文字背景色 */
							}
						}
		
						x++;
					}
					
					for (i = 0; i < _tFont->Space; i++)	/* 字符宽度 */
					{
						if (_tFont->BackColor != CL_MASK)	/* 透明色 */
						{						
							/* 如果文字底色按_tFont->usBackColor，并且字间距大于点阵的宽度，那么需要在文字之间填充(暂时未实现) */
							LCD_PutPixel(x + i, y, _tFont->BackColor);	/* 设置像素颜色为文字背景色 */
						}
					}
					y++;					
				}
			}
			else
			{
				if (_tFont->BackColor == CL_MASK)	/* 透明色 */
				{
//					RA8875_SetTextTransp(1);
				}
//				RA8875_SetFrontColor(_tFont->FrontColor);			/* 设置字体前景色 */
//				RA8875_SetBackColor(_tFont->BackColor);				/* 设置字体背景色 */
//				RA8875_SetFont(ra8875_font_code, 0, _tFont->Space);	/* 字体代码，行间距，字间距 */
//				RA8875_DispStr(_usX, _usY, (char *)&code1);
				if (_tFont->BackColor == CL_MASK)	/* 透明色 */
				{
//					RA8875_SetTextTransp(0);
				}	
			}
	
			_usX += width + _tFont->Space;	/* 列地址递增 */
			_ptr++;			/* 指向下一个字符 */
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SeachStr_a
*	功能说明: 搜索一个字符串是否有‘\a’
*	形    参:
*		_ptr  : 字符串指针
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t SeachStr_a(char *_ptr)
{
	uint16_t j = 0;
	uint8_t a_flag = 0;
	
	while(_ptr[j] != 0)
	{
		if (_ptr[j] == '\a')
		{
			a_flag = 1;
			break;
		}
		j++;
	}
	
	return a_flag;
}
/*
*********************************************************************************************************
*	函 数 名: LCD_GetStrWidth
*	功能说明: 计算字符串宽度(像素单位)
*	形    参:
*		_ptr  : 字符串指针
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont)
{
	char *p = _ptr;
	uint16_t width = 0;
	uint8_t code1, code2;
	uint16_t font_width;
	uint16_t m;
	uint16_t address;
	uint8_t a_flag = 0;

	font_width = LCD_GetFontWidth(_tFont);

	while (*p != 0)
	{
		code1 = *p;	/* 读取字符串数据， 该数据可能是ascii代码，也可能汉字代码的高字节 */
		if (code1 < 0x80)	/* ASCII */
		{
			if (code1 == '\a')
			{
				a_flag = 1;
				p++;
				code1 = *p;
			}
			else
			{
				a_flag = 0;
			}
			
			switch(_tFont->FontCode)
			{
				case FC_RA8875_16:
					font_width = g_RA8875_Ascii16_width[code1 - 0x20];
					break;
				
				case FC_RA8875_24:
					if (a_flag == 0)
					{
						font_width = g_RA8875_Ascii24_width[code1 - 0x20];
					}
					else
					{
						{
							m = 0;
							while(1)
							{
							   address = m * (72 + 2);
							   m++;
							   if (code1 == g_Ascii24_VarWidth[address + 0])
							   {
								  font_width = g_Ascii24_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii24_VarWidth[address + 0] == 0xFF) && (g_Ascii24_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* 字库搜索完毕，未找到，则填充全FF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}
					}
					break;
				
				case FC_RA8875_32:
					if (a_flag == 0)
					{
						font_width = g_RA8875_Ascii32_width[code1 - 0x20];
					}
					else
					{
						{
							m = 0;
							while(1)
							{
							   address = m * (128 + 2);
							   m++;
							   if (code1 == g_Ascii32_VarWidth[address + 0])
							   {
								  font_width = g_Ascii32_VarWidth[address + 1];
								  break;
							   }
							   else if ((g_Ascii32_VarWidth[address + 0] == 0xFF) && (g_Ascii32_VarWidth[address + 1] == 0xFF))
							   {
	//							  /* 字库搜索完毕，未找到，则填充全FF */
	//							  memset(g_Ascii32_VarWidth, 0xFF, 128);
								  break;
							   }	   
						   }
						}
					}
					break;
				
				case FC_ST_12:
					font_width = 6;
					break;

				case FC_ST_16:		
					font_width = 8;
					break;
					
				case FC_ST_24:			
					font_width = 12;
					break;
					
				case FC_ST_32:
					font_width = 16;
					break;

				case FC_ST_62X40:
					//对秒进行特殊处理
					if (code1 == 0x5E)
					{
						font_width = 28;
					}
					else
					{
						font_width = 40;
					}
					break;

				case FC_ST_96X40:
					//对秒进行特殊处理
					if (code1 == 0x5E)
					{
						font_width = 28;
					}
					else
					{
						font_width = 40;
					}
					break;
									
				default:
					font_width = 8;
					break;					
			}
			
		}
		else	/* 汉字 */
		{
			code2 = *++p;
			if (code2 == 0)
			{
				break;
			}
			font_width = LCD_GetFontWidth(_tFont);
			
		}
		width += (font_width + _tFont->Space);
		p++;
	}

	return width;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_Fill_Rect
*	功能说明: 用一个颜色值填充一个矩形。【emWin 中有同名函数 LCD_FillRect，因此加了下划线区分】
*	形    参:
*			_usX,_usY: 矩形左上角的坐标
*			_usHeight : 矩形的高度
*			_usWidth  : 矩形的宽度
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	//g_tLCD.FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);
	LCD_Fill(_usX,_usY,_usX+_usWidth,_usY+_usHeight,(uint32_t)_usColor);
}

/*
*********************************************************************************************************
*	函 数 名: _LCD_ReadAsciiDot
*	功能说明: 读取1个ASCII字符的点阵数据
*	形    参:
*		_code : ASCII字符的编码，1字节。1-128
*		_fontcode ：字体代码
*		_pBuf : 存放读出的字符点阵数据
*	返 回 值: 文字宽度
*********************************************************************************************************
*/
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* 使用CPU 内部Flash 小字库 */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1表示程序中内嵌全部的ASCII字符集 */

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* 缺省是16点阵 */
			font_bytes = 32 / 2;
			pAscDot = g_Ascii16;
			fAllHz = 1;
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = g_Ascii24;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = g_Ascii32;
			break;

		case FC_ST_62X40:
			font_bytes = 310;
			pAscDot = g_Ascii62x40;
			break;	
		
		case FC_ST_96X40:
			font_bytes = 480;
			pAscDot = g_Ascii96x40;
			break;
		
		default:
			return;
	}	

	if (fAllHz == 1)	/* 内嵌全部ASCII字符点阵 */
	{
		/* 将CPU内部Flash中的ascii字符点阵复制到buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));		
	}
	else	/* 内嵌部分字符，字模数组首字节是ASCII码 */
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 1);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 1;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* 字库搜索完毕，未找到，则填充全FF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* 用全字库 */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* 使用CPU内嵌的16点阵字符 */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* 使用CPU内嵌的16点阵字符 */
			#endif
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = ASC24_ADDR;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = ASC32_ADDR;
			break;
		
		default:
			return;
	}			
	if (_code >=0x20 && _code <= 0x7E)
	{		
		pAscDot = ((uint32_t)_code - 0x20)*font_bytes + pAscDot;
	}

#ifdef USE_NOR_FONT		/* NOR Flash全字库 */
	/* 将CPU内部Flash中的ascii字符点阵复制到buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* 串行 Flash全字库 */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* 字库芯片的12点阵和16点阵字符不好看,笔画细了，而且是非等宽字体 */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}
/*
*********************************************************************************************************
*	函 数 名: LCD_PutPixel
*	功能说明: 画1个像素
*	形    参:
*			_usX,_usY : 像素坐标
*			_usColor  : 像素颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_PutPixel(uint16_t _usX, uint16_t _usY, uint16_t _usColor)
{
	//g_tLCD.PutPixel(_usX, _usY, _usColor);
	LCD_DrawPoint(_usX,_usY);
	POINT_COLOR = _usColor;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetFontWidth
*	功能说明: 读取字体的宽度（像素单位)
*	形    参:
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
uint16_t LCD_GetFontWidth(FONT_T *_tFont)
{
	uint16_t font_width = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			font_width = 12;
			break;

		case FC_ST_16:
		case FC_RA8875_16:			
			font_width = 16;
			break;
			
		case FC_RA8875_24:			
		case FC_ST_24:
			font_width = 24;
			break;
			
		case FC_ST_32:
		case FC_RA8875_32:	
			font_width = 32;
			break;			

		case FC_ST_62X40:
			font_width = 40;
			break;	
		
		case FC_ST_96X40:
			font_width = 40;
			break;			
	}
	return font_width;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_GetFontHeight
*	功能说明: 读取字体的高度（像素单位)
*	形    参:
*		_tFont : 字体结构体，包含颜色、背景色(支持透明)、字体代码、文字间距等参数
*	返 回 值: 字体的宽度（像素单位)
*********************************************************************************************************
*/
uint16_t LCD_GetFontHeight(FONT_T *_tFont)
{
	uint16_t height = 16;

	switch (_tFont->FontCode)
	{
		case FC_ST_12:
			height = 12;
			break;

		case FC_ST_16:
		case FC_RA8875_16:			
			height = 16;
			break;
			
		case FC_RA8875_24:			
		case FC_ST_24:
			height = 24;
			break;
			
		case FC_ST_32:
		case FC_RA8875_32:	
			height = 32;
			break;	
		
		case FC_ST_62X40:
			height = 62;
			break;		

		case FC_ST_96X40:
			height = 96;
			break;		
	}
	return height;
}

/*
*********************************************************************************************************
*	函 数 名: _LCD_ReadHZDot
*	功能说明: 读取1个汉字的点阵数据
*	形    参:
*		_code1, _cod2 : 汉字内码. GB2312编码
*		_fontcode ：字体代码
*		_pBuf : 存放读出的字符点阵数据
*	返 回 值: 无
*********************************************************************************************************
*/
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* 使用CPU 内部Flash 小字库 */
	uint8_t *pDot;
	uint8_t font_bytes = 0;
	uint32_t address;
	uint16_t m;

	pDot = 0;	/* 仅仅用于避免告警 */
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 24;
			pDot = (uint8_t *)g_Hz12;	
			break;
		
		case FC_ST_16:
			font_bytes = 32;
			pDot = (uint8_t *)g_Hz16;
			break;

		case FC_ST_24:
			font_bytes = 72;
			pDot = (uint8_t *)g_Hz24;
			break;			
			
		case FC_ST_32:	
			font_bytes = 128;
			pDot = (uint8_t *)g_Hz32;
			break;						
		
		default:
			return;
	}	

	m = 0;
	while(1)
	{
		address = m * (font_bytes + 2);
		m++;
		if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
		{
			address += 2;
			memcpy(_pBuf, &pDot[address], font_bytes);
			break;
		}
		else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
		{
			/* 字库搜索完毕，未找到，则填充全FF */
			memset(_pBuf, 0xFF, font_bytes);
			break;
		}
	}
#else	/* 用全字库 */
	uint32_t offset = 0;
	uint8_t font_bytes = 0;
		
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 24;
			offset = HZK12_ADDR;	
			break;
		
		case FC_ST_16:
			font_bytes = 32;
			offset = HZK16_ADDR;
			break;

		case FC_ST_24:
			font_bytes = 72;
			offset = HZK24_ADDR;
			break;			
			
		case FC_ST_32:	
			font_bytes = 128;
			offset = HZK32_ADDR;
			break;						
		
		default:
			return;
	}			

	/* 此处需要根据字库文件存放位置进行修改 
		GB2312范围： 0xA1A1 - 0xFEFE
		其中汉字范围 : 0xB0A1 - 0xF7FE
	
		GBK 范围： 0x8140 - 0xFEFE 
	
		安富莱自定义汉字编码错开GBK和GB2312编码空间： 0x8000 - 0x813F （319个）		
	*/
	if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
	}
	else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
	}
	else	/* 2018-03-13 增加自定义汉字编码，用于实现特殊图标符号 */
	{
		uint16_t code16;
		uint8_t *pDot;
		uint32_t address;
		uint16_t m;		
		
		code16 = _code1 * 256 + _code2;
		if (code16 >= 0x8000 && code16 <= 0x813F)	/* 自定义汉字点阵，固定使用CPU片内部小字库 */
		{
			pDot = 0;	/* 仅仅用于避免告警 */
			switch (_fontcode)
			{
				case FC_ST_12:		/* 12点阵 */
					font_bytes = 24;
					pDot = (uint8_t *)g_Hz12;	
					break;
				
				case FC_ST_16:
					font_bytes = 32;
					pDot = (uint8_t *)g_Hz16;
					break;

				case FC_ST_24:
					font_bytes = 72;
					pDot = (uint8_t *)g_Hz24;
					break;			
					
				case FC_ST_32:	
					font_bytes = 128;
					pDot = (uint8_t *)g_Hz32;
					break;						
				
				default:
					break;
			}	
			
			m = 0;
			while(1)
			{
				address = m * (font_bytes + 2);
				m++;
				if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
				{
					address += 2;
					memcpy(_pBuf, &pDot[address], font_bytes);
					break;
				}
				else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
				{
					/* 字库搜索完毕，未找到，则填充全FF */
					memset(_pBuf, 0xFF, font_bytes);
					break;
				}
			}	
			return;
		}
	}

#ifdef USE_NOR_FONT		/* NOR Flash全字库 */
	/* 将CPU内部Flash中的ascii字符点阵复制到buf */
	memcpy(_pBuf, (char *)offset, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* NOR Flash全字库 */
	sf_ReadBuffer(_pBuf, offset, font_bytes);
#endif
	
#endif
}


/*
*********************************************************************************************************
*	函 数 名: _LCD_ReadSmallDot
*	功能说明: 读取1个小语种字符的点阵数据
*	形    参:
*		_code : ASCII字符的编码，1字节。1-128
*		_fontcode ：字体代码
*		_pBuf : 存放读出的字符点阵数据
*	返 回 值: 文字宽度
*********************************************************************************************************
*/
static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* 使用CPU 内部Flash 小字库 */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1表示程序中内嵌全部的ASCII字符集 */
	
	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* 缺省是16点阵 */
			font_bytes = 32 / 2;
			pAscDot = g_Ascii16;
			fAllHz = 1;
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = g_Ascii24;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = g_Ascii32;
			break;

		case FC_ST_62X40:
			font_bytes = 310;
			pAscDot = g_Ascii62x40;
			break;	
		
		case FC_ST_96X40:
			font_bytes = 480;
			pAscDot = g_Ascii96x40;
			break;

		case FC_RA8875_24:
			font_bytes = 72;
			pAscDot = g_Ascii24_VarWidth;
			fAllHz = 2;
			break;
		
		case FC_RA8875_32:
			font_bytes = 128;
			pAscDot = g_Ascii32_VarWidth;
			fAllHz = 2;
			break;
		
		default:
			return;
	}	

	if (fAllHz == 1)	/* 内嵌全部ASCII字符点阵 */
	{
		/* 将CPU内部Flash中的ascii字符点阵复制到buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));		
	}
	else if (fAllHz == 2)
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 2);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 2;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* 字库搜索完毕，未找到，则填充全FF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
	}
	else	/* 内嵌部分字符，字模数组首字节是ASCII码 */
	{
		m = 0;
		while(1)
		{
		   address = m * (font_bytes + 1);
		   m++;
		   if (_code == pAscDot[address + 0])
		   {
			  address += 1;
			  memcpy(_pBuf, &pAscDot[address], font_bytes);
			  break;
		   }
		   else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
		   {
			  /* 字库搜索完毕，未找到，则填充全FF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* 用全字库 */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12点阵 */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* 使用CPU内嵌的16点阵字符 */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* 字库芯片的16点阵字符不好看,笔画细了，而且是非等宽字体 */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* 使用CPU内嵌的16点阵字符 */
			#endif
			break;
		
		case FC_ST_24:
			font_bytes = 48;
			pAscDot = ASC24_ADDR;
			break;
		
		case FC_ST_32:
			font_bytes = 64;
			pAscDot = ASC32_ADDR;
			break;
		
		default:
			return;
	}			
	if (_code >=0x20 && _code <= 0x7E)
	{		
		pAscDot = ((uint32_t)_code - 0x20)*font_bytes + pAscDot;
	}

#ifdef USE_NOR_FONT		/* NOR Flash全字库 */
	/* 将CPU内部Flash中的ascii字符点阵复制到buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* 串行 Flash全字库 */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* 字库芯片的12点阵和16点阵字符不好看,笔画细了，而且是非等宽字体 */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}


