#ifndef _BSP_LCD_H_
#define _BSP_LCD_H_
#include "sys.h"


typedef __IO uint16_t vuint16_t;

typedef struct
{
	uint16_t width;	//LCD���
	uint16_t height;	//LCD�߶�
	uint16_t id;		//LCD ID
	uint8_t dir;		//0,������1������
	uint16_t wramcmd;//��ʼдgramָ��
	uint16_t setxcmd;//����x����ָ��
	uint16_t setycmd;//����y����ָ��
}_lcd_dev;

extern _lcd_dev lcddev;		//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ
extern uint32_t POINT_COLOR;		//Ĭ�Ϻ�ɫ
extern uint32_t BACK_COLOR;		//������ɫ��Ĭ��Ϊ��ɫ

#define LCD_LED PBout(15) 	//LCD����	PB15

typedef struct 
{
	vuint16_t LCD_REG;
	vuint16_t LCD_RAM;
}LCD_TypeDef;

//ʹ��NOR/SRAM�� Bank1.sector4,��ַλHADDR[27,26]=11 A6��Ϊ�������������� 
//ע������ʱSTM32�ڲ�������һλ����! 111 1110=0X7E	
#define LCD_BASE	((uint32_t)(0x6C000000 | 0x0000007E))
#define LCD			((LCD_TypeDef *)LCD_BASE)


void bsp_Initlcd(void);
void LCD_WR_REG(vuint16_t);
void LCD_WR_DATA(vuint16_t);
uint16_t LCD_RD_DATA(void);
void LCD_WriteReg(uint16_t ,uint16_t);
uint16_t LCD_ReadReg(uint16_t);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t);
void LCD_Scan_Dir(uint8_t);
void LCD_Display_Dir(uint8_t);
void LCD_Clear(uint32_t);
void LCD_Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);
void LCD_DrawPoint(uint16_t x,uint16_t y);	
void LCD_Color_Fill(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t*);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);		   				//��䵥ɫ
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height);
	
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint32_t color);	
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);						//��ʾһ���ַ�
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);		//��ʾһ���ַ���,12/16����

//ɨ�跽����
#define L2R_U2D  0 		//������,���ϵ���
#define L2R_D2U  1 		//������,���µ���
#define R2L_U2D  2 		//���ҵ���,���ϵ���
#define R2L_D2U  3 		//���ҵ���,���µ���

#define U2D_L2R  4 		//���ϵ���,������
#define U2D_R2L  5 		//���ϵ���,���ҵ���
#define D2U_L2R  6 		//���µ���,������ ��
#define D2U_R2L  7		//���µ���,���ҵ���	 

#define DFT_SCAN_DIR  D2U_L2R  //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define TFT_WHITE         	 0xFFFF
#define TFT_BLACK         	 0x0000	  
#define TFT_BLUE         	 0x001F  
#define TFT_BRED             0XF81F
#define TFT_GRED 			 0XFFE0
#define TFT_GBLUE			 0X07FF
#define TFT_RED           	 0xF800
#define TFT_MAGENTA       	 0xF81F
#define TFT_GREEN         	 0x07E0
#define TFT_CYAN          	 0x7FFF
#define TFT_YELLOW        	 0xFFE0
#define TFT_BROWN 			 0XBC40 //��ɫ
#define TFT_BRRED 			 0XFC07 //�غ�ɫ
#define TFT_GRAY  			 0X8430 //��ɫ




#endif

