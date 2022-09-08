#include "bsp_font_flash.h"

static uint8_t SeachStr_a(char *_ptr);
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align);
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf);
	static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf);
/*
*********************************************************************************************************
*	�� �� ��: LCD_DispStr
*	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ���
*	��    ��:
*		_usX : X����
*		_usY : Y����
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_DispStr(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
	LCD_DispStrEx0(_usX, _usY, _ptr, _tFont, 0, 0);
}
/*
*********************************************************************************************************
*	�� �� ��: LCD_DispStrEx0
*	����˵��: ��LCDָ�����꣨���Ͻǣ���ʾһ���ַ����� ��ǿ�ͺ�����֧����\��\�Ҷ��룬֧�ֶ���������
*	��    ��:
*		_usX : X����
*		_usY : Y����
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���������ָ��RA8875�ֿ���ʾ����
*		_Width : �ַ�����ʾ����Ŀ��. 0 ��ʾ�������������򣬴�ʱ_Align��Ч
*		_Align :�ַ�������ʾ����Ķ��뷽ʽ��
*				ALIGN_LEFT = 0,
*				ALIGN_CENTER = 1,
*				ALIGN_RIGHT = 2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void LCD_DispStrEx0(uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
	uint8_t _Align)
{
	uint32_t i;
	uint8_t code1;
	uint8_t code2;
	//uint8_t buf[32 * 32 / 8];	/* ���֧��32������ */
	uint8_t buf[96 * 40 / 8];	/* ���֧��96x40�����ַ� */
	uint8_t width;
	uint16_t m;
	uint8_t font_width = 0;
	uint8_t font_height = 0;
	uint16_t x, y;
	uint16_t offset;
	uint16_t str_width;	/* �ַ���ʵ�ʿ��  */
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
		case FC_ST_12:		/* 12���� */
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
			ra8875_use = 1;	/* ��ʾ��RA8875�ֿ� */
			break;
			
		case FC_RA8875_24:
			ra8875_font_code = RA_FONT_24;
			a_flag = SeachStr_a(_ptr);	/* �����ַ������Ƿ���'\a' */
			if (a_flag == 0)	/**/
			{
				ra8875_use = 1;	/* ��ʾ��RA8875�ֿ� */
			}
			else
			{
				ra8875_use = 0;	/* ��ʾ����RA8875�ֿ� */
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
				ra8875_use = 1;	/* ��ʾ��RA8875�ֿ� */
			}
			else
			{
				ra8875_use = 0;	/* ��ʾ����RA8875�ֿ� */
				font_height = 32;
				font_width = 32;
				asc_bytes = 4;
				hz_bytes = 4;
			}
			break;
	}
	
	str_width = LCD_GetStrWidth(_ptr, _tFont);	/* �����ַ���ʵ�ʿ��(RA8875�ڲ�ASCII������Ϊ�䳤 */
	offset = 0;
	if (_Width > str_width)
	{
		if (_Align == ALIGN_RIGHT)	/* �Ҷ��� */
		{
			offset = _Width - str_width;
		}
		else if (_Align == ALIGN_CENTER)	/* ����� */
		{
			offset = (_Width - str_width) / 2;
		}
		else	/* ����� ALIGN_LEFT */
		{
			;
		}
	}

	/* ������ɫ, �м������ұ߶���  */
	if (offset > 0)
	{
		if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
		{
			LCD_Fill_Rect(_usX, _usY, LCD_GetFontHeight(_tFont), offset,  _tFont->BackColor);	
		}
		_usX += offset;
		
	}
	
	/* �Ҳ����ɫ */
	if (_Width > str_width)
	{
		if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
		{
			LCD_Fill_Rect(_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset,  _tFont->BackColor);
		}
	}
	
	/* ʹ��CPU�ڲ��ֿ�. ������Ϣ��CPU��ȡ */
	{
		/* ��ʼѭ�������ַ� */
		while (*_ptr != 0)
		{
			code1 = *_ptr;	/* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
				
			if (code1 < 0x80)
			{
				if (a_flag == 0)
				{
					RA8875_flag = 0;
					/* ��ascii�ַ������Ƶ�buf */
					_LCD_ReadAsciiDot(code1, _tFont->FontCode, buf);	/* ��ȡASCII�ַ����� */
					
					//����������⴦��,�����ȹ���
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
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
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
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
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
				/* ��1�����ֵĵ��� */
				_LCD_ReadHZDot(code1, code2, _tFont->FontCode, buf);
				width = font_width;
				line_bytes = hz_bytes;
			}
			
			y = _usY;
			if (RA8875_flag == 0)
			{
				/* ��ʼˢLCD */
				for (m = 0; m < font_height; m++)	/* �ַ��߶� */
				{
					x = _usX;
					for (i = 0; i < width; i++)	/* �ַ���� */
					{
						if ((buf[m * line_bytes + i / 8] & (0x80 >> (i % 8 ))) != 0x00)
						{
							LCD_PutPixel(x, y, _tFont->FrontColor);	/* ����������ɫΪ����ɫ */
						}
						else
						{
							if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
							{
								LCD_PutPixel(x, y, _tFont->BackColor);	/* ����������ɫΪ���ֱ���ɫ */
							}
						}
		
						x++;
					}
					
					for (i = 0; i < _tFont->Space; i++)	/* �ַ���� */
					{
						if (_tFont->BackColor != CL_MASK)	/* ͸��ɫ */
						{						
							/* ������ֵ�ɫ��_tFont->usBackColor�������ּ����ڵ���Ŀ�ȣ���ô��Ҫ������֮�����(��ʱδʵ��) */
							LCD_PutPixel(x + i, y, _tFont->BackColor);	/* ����������ɫΪ���ֱ���ɫ */
						}
					}
					y++;					
				}
			}
			else
			{
				if (_tFont->BackColor == CL_MASK)	/* ͸��ɫ */
				{
//					RA8875_SetTextTransp(1);
				}
//				RA8875_SetFrontColor(_tFont->FrontColor);			/* ��������ǰ��ɫ */
//				RA8875_SetBackColor(_tFont->BackColor);				/* �������屳��ɫ */
//				RA8875_SetFont(ra8875_font_code, 0, _tFont->Space);	/* ������룬�м�࣬�ּ�� */
//				RA8875_DispStr(_usX, _usY, (char *)&code1);
				if (_tFont->BackColor == CL_MASK)	/* ͸��ɫ */
				{
//					RA8875_SetTextTransp(0);
				}	
			}
	
			_usX += width + _tFont->Space;	/* �е�ַ���� */
			_ptr++;			/* ָ����һ���ַ� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SeachStr_a
*	����˵��: ����һ���ַ����Ƿ��С�\a��
*	��    ��:
*		_ptr  : �ַ���ָ��
*	�� �� ֵ: ��
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
*	�� �� ��: LCD_GetStrWidth
*	����˵��: �����ַ������(���ص�λ)
*	��    ��:
*		_ptr  : �ַ���ָ��
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ��
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
		code1 = *p;	/* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
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
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
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
	//							  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
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
					//����������⴦��
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
					//����������⴦��
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
		else	/* ���� */
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
*	�� �� ��: LCD_Fill_Rect
*	����˵��: ��һ����ɫֵ���һ�����Ρ���emWin ����ͬ������ LCD_FillRect����˼����»������֡�
*	��    ��:
*			_usX,_usY: �������Ͻǵ�����
*			_usHeight : ���εĸ߶�
*			_usWidth  : ���εĿ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void LCD_Fill_Rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	//g_tLCD.FillRect(_usX, _usY, _usHeight, _usWidth, _usColor);
	LCD_Fill(_usX,_usY,_usX+_usWidth,_usY+_usHeight,(uint32_t)_usColor);
}

/*
*********************************************************************************************************
*	�� �� ��: _LCD_ReadAsciiDot
*	����˵��: ��ȡ1��ASCII�ַ��ĵ�������
*	��    ��:
*		_code : ASCII�ַ��ı��룬1�ֽڡ�1-128
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ���ֿ��
*********************************************************************************************************
*/
static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1��ʾ��������Ƕȫ����ASCII�ַ��� */

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* ȱʡ��16���� */
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

	if (fAllHz == 1)	/* ��Ƕȫ��ASCII�ַ����� */
	{
		/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));		
	}
	else	/* ��Ƕ�����ַ�����ģ�������ֽ���ASCII�� */
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
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* ��ȫ�ֿ� */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* ʹ��CPU��Ƕ��16�����ַ� */
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

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* ���� Flashȫ�ֿ� */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* �ֿ�оƬ��12�����16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}
/*
*********************************************************************************************************
*	�� �� ��: LCD_PutPixel
*	����˵��: ��1������
*	��    ��:
*			_usX,_usY : ��������
*			_usColor  : ������ɫ
*	�� �� ֵ: ��
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
*	�� �� ��: LCD_GetFontWidth
*	����˵��: ��ȡ����Ŀ�ȣ����ص�λ)
*	��    ��:
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ����Ŀ�ȣ����ص�λ)
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
*	�� �� ��: LCD_GetFontHeight
*	����˵��: ��ȡ����ĸ߶ȣ����ص�λ)
*	��    ��:
*		_tFont : ����ṹ�壬������ɫ������ɫ(֧��͸��)��������롢���ּ��Ȳ���
*	�� �� ֵ: ����Ŀ�ȣ����ص�λ)
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
*	�� �� ��: _LCD_ReadHZDot
*	����˵��: ��ȡ1�����ֵĵ�������
*	��    ��:
*		_code1, _cod2 : ��������. GB2312����
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2,  uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	uint8_t *pDot;
	uint8_t font_bytes = 0;
	uint32_t address;
	uint16_t m;

	pDot = 0;	/* �������ڱ���澯 */
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
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
			/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			memset(_pBuf, 0xFF, font_bytes);
			break;
		}
	}
#else	/* ��ȫ�ֿ� */
	uint32_t offset = 0;
	uint8_t font_bytes = 0;
		
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
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

	/* �˴���Ҫ�����ֿ��ļ����λ�ý����޸� 
		GB2312��Χ�� 0xA1A1 - 0xFEFE
		���к��ַ�Χ : 0xB0A1 - 0xF7FE
	
		GBK ��Χ�� 0x8140 - 0xFEFE 
	
		�������Զ��庺�ֱ����GBK��GB2312����ռ䣺 0x8000 - 0x813F ��319����		
	*/
	if (_code1 >=0xA1 && _code1 <= 0xA9 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
	}
	else if (_code1 >=0xB0 && _code1 <= 0xF7 && _code2 >=0xA1)
	{
		offset += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
	}
	else	/* 2018-03-13 �����Զ��庺�ֱ��룬����ʵ������ͼ����� */
	{
		uint16_t code16;
		uint8_t *pDot;
		uint32_t address;
		uint16_t m;		
		
		code16 = _code1 * 256 + _code2;
		if (code16 >= 0x8000 && code16 <= 0x813F)	/* �Զ��庺�ֵ��󣬹̶�ʹ��CPUƬ�ڲ�С�ֿ� */
		{
			pDot = 0;	/* �������ڱ���澯 */
			switch (_fontcode)
			{
				case FC_ST_12:		/* 12���� */
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
					/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
					memset(_pBuf, 0xFF, font_bytes);
					break;
				}
			}	
			return;
		}
	}

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)offset, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* NOR Flashȫ�ֿ� */
	sf_ReadBuffer(_pBuf, offset, font_bytes);
#endif
	
#endif
}


/*
*********************************************************************************************************
*	�� �� ��: _LCD_ReadSmallDot
*	����˵��: ��ȡ1��С�����ַ��ĵ�������
*	��    ��:
*		_code : ASCII�ַ��ı��룬1�ֽڡ�1-128
*		_fontcode ���������
*		_pBuf : ��Ŷ������ַ���������
*	�� �� ֵ: ���ֿ��
*********************************************************************************************************
*/
static void _LCD_ReadSmallDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT	/* ʹ��CPU �ڲ�Flash С�ֿ� */
	const uint8_t *pAscDot;
	uint32_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0;	/* 1��ʾ��������Ƕȫ����ASCII�ַ��� */
	
	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 24 / 2;
			pAscDot = g_Ascii12;	
			fAllHz = 1;
			break;
		
		case FC_ST_16:
			/* ȱʡ��16���� */
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

	if (fAllHz == 1)	/* ��Ƕȫ��ASCII�ַ����� */
	{
		/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
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
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
	}
	else	/* ��Ƕ�����ַ�����ģ�������ֽ���ASCII�� */
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
			  /* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			  memset(_pBuf, 0xFF, font_bytes);
			  break;
		   }	   
	   }
   }
#else	/* ��ȫ�ֿ� */
	uint32_t pAscDot;
	uint8_t font_bytes = 0;

	pAscDot = 0;
	switch (_fontcode)
	{
		case FC_ST_12:		/* 12���� */
			font_bytes = 12;
			#if 0
				pAscDot = ASC12_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii12[' ' * 12];	/* ʹ��CPU��Ƕ��16�����ַ� */
			#endif			
			break;
		
		case FC_ST_16:
			font_bytes = 16;
			#if 0
				pAscDot = ASC16_ADDR;	/* �ֿ�оƬ��16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
			#else
				pAscDot = (uint32_t)&g_Ascii16[' ' * 16];	/* ʹ��CPU��Ƕ��16�����ַ� */
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

#ifdef USE_NOR_FONT		/* NOR Flashȫ�ֿ� */
	/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
	memcpy(_pBuf, (char *)pAscDot, font_bytes);	
#endif

#ifdef USE_SPI_FONT		/* ���� Flashȫ�ֿ� */
	if (_fontcode == FC_ST_12 || _fontcode == FC_ST_16)
	{
		memcpy(_pBuf, (char *)pAscDot, font_bytes);	
	}
	else
	{
		/* �ֿ�оƬ��12�����16�����ַ����ÿ�,�ʻ�ϸ�ˣ������Ƿǵȿ����� */
		sf_ReadBuffer(_pBuf, pAscDot, font_bytes);
	}
#endif
	
#endif
}


