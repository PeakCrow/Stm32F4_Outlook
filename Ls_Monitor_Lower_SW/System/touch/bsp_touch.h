#ifndef _BSP_TOUCH_H_
#define _BSP_TOUCH_H_

#include "sys.h"

#define TP_PRES_DOWN 0X80	//��Ļ�����£�ȡ���λ
#define TP_CATH_PRES 0X40   //�а������£�ȡ�θ�λ
#define CT_MAX_TOUCH 5		//������֧�ֵĵ������̶�Ϊ5��
//������������
typedef struct
{
	uint8_t (*init)(void);			//��ʼ��������������
	uint8_t (*scan)(uint8_t);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;
//������оƬ��������	   
#define PEN  		PBin(1)  	//T_PEN
#define DOUT 		PBin(2)   	//T_MISO
#define TDIN 		PFout(11)  	//T_MOSI
#define TCLK 		PBout(0)  	//T_SCK
#define TCS  		PCout(13)  	//T_CS  Ƭѡ����
///* �ж����� */
//#define TP_INT_EXTI_IRQ                 EXTI1_IRQn
///* �жϷ����� */
//#define TP_IRQHandler                   EXTI1_IRQHandler
//������/������ ���ú���
uint8_t TP_Scan(uint8_t tp);								//ɨ��
uint8_t TP_Init(void);								//��ʼ��
void bsp_InitLcdTouch(void);


/************************************************IICͨѶ****************************************/


//IO��������	���Ĵ����еĵ�(2*11)λ����һλ��0		�Ĵ����ĵ�(2*11)λ��0
#define CT_SDA_IN() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}//PF11����ģʽ
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;}//PF11���ģʽ
//IO��������
#define CT_IIC_SCL	PBout(0)	//SCL-PB0��������ߵ͵�ƽ
#define CT_IIC_SDA	PFout(11)	//SDA-PF11��������ߵ͵�ƽ
#define CT_READ_SDA	PFin(11)	//����SDA-PF11����ߵ͵�ƽ

//IIC���в�������
void CT_IIC_Init(void);//��ʼ��IIC��IO��
void CT_IIC_Start(void);//����IIC��ʼ�ź�
void CT_IIC_Stop(void);//����IICֹͣ�ź�
void CT_IIC_Send_Byte(uint8_t txd);//IIC����һ���ֽ�
uint8_t CT_IIC_Read_Byte(uint8_t ack);//IIC��ȡһ���ֽ�
uint8_t CT_IIC_Wait_Ack(void);//IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);//IIC����ACK�ź�
void CT_IIC_NAck(void);//IIC������ACK�ź�


/************************************************GT9147����****************************************/

//IO��������
#define GT_RST	PCout(13)	//GT9147��λ����
#define GT_INT	PBin(1)		//GT9147�ж�����

//I2C��д����
#define	GT_CMD_WR	0X28	//д����
#define GT_CMD_RD	0X29	//������

//GT9147���ּĴ�������
#define GT_CTRL_REG		0X8040	//GT9147���ƼĴ���
#define GT_CFGS_REG		0X8047	//GT9147������ʼ��ַ�Ĵ���
#define GT_CHECK_REG	0X80FF	//GT9147У��ͼĴ���
#define GT_PID_REG		0X8140	//GT9147��ƷID�Ĵ���

#define GT_GSTID_REG	0X814E	//GT9147��ǰ��⵽�Ĵ������
#define GT_TP1_REG		0X8150	//��һ�����������ݵ�ַ
#define GT_TP2_REG		0X8158	//�ڶ������������ݵ�ַ
#define GT_TP3_REG		0X8160	//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168	//���ĸ����������ݵ�ַ
#define GT_TP5_REG		0X8170	//��������������ݵ�ַ
#define GT_TOUCH_AREA_L   0X8154
#define GT_TOUCH_AREA_H   0X8155//��һ��������Ĵ������

uint8_t GT9147_Send_Cfg(uint8_t mode);
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
void GT947_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
uint8_t GT9147_Init(void);
uint8_t GT9147_Scan(uint8_t mode);


/*******************************���������幦�ܲ���*************************************************/
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color);
void gui_fill_circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color);
void Load_Drow_Dialog(void);
void ctp_test(void);

#endif

