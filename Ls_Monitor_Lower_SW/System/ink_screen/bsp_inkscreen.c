#include "bsp_inkscreen.h"


static void gpio_ink_init(void);
static void inkscreen_writebyte(uint8_t _value);
static void inkscreen_sendcommand(uint8_t _reg);
static void inkscreen_senddata(uint8_t _data);
static uint8_t inkscreen_readbusy(void);

/*******************************************************************************
  * @FunctionName: GPIO_INK_Init
  * @Author:       trx
  * @DateTime:     2022��5��10�� 18:59:21 
  * @Purpose:      ��ȥspiͨѶ����Ŀ������ŵĳ�ʼ��
  * @param:        void
  * @return:       none
*******************************************************************************/
static void gpio_ink_init(void)
{
	GPIO_InitTypeDef gpio_initstruct = {0};

	/* ʹ������ʱ�� */
	INK_BUSY_CLK_ENABLE();
	INK_RST_CLK_ENABLE();
	INK_DC_CLK_ENABLE();
	INK_CS_CLK_ENABLE();

	/* �������ŵ�����ȼ� */
	//HAL_GPIO_WritePin(INK_BUSY_GPIO_PORT,INK_BUSY_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_RST_GPIO_PORT,INK_RST_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_DC_GPIO_PORT,INK_DC_GPIO_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(INK_CS_GPIO_PORT,INK_CS_GPIO_PIN,GPIO_PIN_RESET);
	

	/* ��������ģʽ */
	/* RST PIN */
	gpio_initstruct.Pin		 = INK_RST_GPIO_PIN;
	gpio_initstruct.Mode	 = GPIO_MODE_OUTPUT_PP;
	gpio_initstruct.Pull	 = GPIO_NOPULL;
	gpio_initstruct.Speed	 = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(INK_RST_GPIO_PORT,&gpio_initstruct);

	/* DC PIN */
	gpio_initstruct.Pin		 = INK_DC_GPIO_PIN;
	HAL_GPIO_Init(INK_DC_GPIO_PORT,&gpio_initstruct);

	/* CS PIN */
	gpio_initstruct.Pin		 = INK_CS_GPIO_PIN;
	HAL_GPIO_Init(INK_CS_GPIO_PORT,&gpio_initstruct);

	/* BUSY PIN */
	gpio_initstruct.Pin		 = INK_BUSY_GPIO_PIN;
	gpio_initstruct.Mode	 = GPIO_MODE_INPUT;
	HAL_GPIO_Init(INK_BUSY_GPIO_PORT,&gpio_initstruct);
	
}
/*******************************************************************************
  * @FunctionName: bsp_Inkscreen_write_byte
  * @Author:       trx
  * @DateTime:     2023��4��22��22��44�� 
  * @Purpose:      ��īˮ��spiдһ���ֽ�
  * @param:        void
  * @return:       none
*******************************************************************************/
static void inkscreen_writebyte(uint8_t _value)
{
	bsp_spi2Transfer(_value);
}
static void inkscreen_sendcommand(uint8_t _reg)
{
	PBout(12) = 0;
	PCout(0) = 0;
	inkscreen_writebyte(_reg);
	PCout(0) = 1;
}
static void inkscreen_senddata(uint8_t _data)
{
	PBout(12) = 1;
	PCout(0) = 0;
	inkscreen_writebyte(_data);
	PCout(0) = 1;
}
static uint8_t inkscreen_readbusy(void)
{
	App_Printf("ink screen is busying \r\n");
	uint8_t busy;
	do 
	{
		inkscreen_sendcommand(0x71);
		busy = PAin(1);
		busy = !(busy & 0x01);
	}
	while(busy);
	App_Printf("ink screen is release \r\n");
	inkscreen_delay_ms(200);
	return busy;
}
/*******************************************************************************
  * @FunctionName: bsp_InksreenExit
  * @Author:       trx
  * @DateTime:     2023��4��22��22��44�� 
  * @Purpose:      īˮ���˳�����Ҫ������5v����
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenExit(void)
{
	PBout(12) = 0;
	PCout(0) = 0;
	PAout(7) = 0;		/* close 5v */
}

/*******************************************************************************
  * @FunctionName: bsp_InksreenInit
  * @Author:       trx
  * @DateTime:     2023��4��22��22��44�� 
  * @Purpose:      īˮ����ʼ��-->Ӧ�ý�дͼ������Ҳ�Ź���
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenInit(void)
{
	gpio_ink_init();
	PBout(12) = 0;
	PCout(0) = 0;
	PAout(7) = 1;

    bsp_InkscreenReset();

	/* �򿪵�Դ */	
    inkscreen_sendcommand(0x04);  
    inkscreen_readbusy();
	/* panel setting �Ǳ����� */
    inkscreen_sendcommand(0x00);
    /* 0x0f 0x89 ȫ����0x00�Ĵ�����Ĭ������ */
    inkscreen_senddata(0x0f);
    inkscreen_senddata(0x89);
	/* resolution setting �ֱ������� */
    inkscreen_sendcommand(0x61);
    /* ˮƽ����128 -   1 = 127 */
    inkscreen_senddata (0x80);
    /* ��ֱ����296 -     1 = 295����Ҫ1���ֽڼ�1λ����ʾ */
    inkscreen_senddata (0x01);
    inkscreen_senddata (0x28);
	/* vcom and data interval setting vcom�����ݼ������ */
    inkscreen_sendcommand(0X50);
    /* ������Ļ��ɫ���ݰ����к�ɫ����ɫ�Ͱ�ɫ */
    inkscreen_senddata(0x77);

	bsp_InkscreeenClear();
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreenReset
  * @Author:       trx
  * @DateTime:     2023��4��22��23��01��
  * @Purpose:      īˮ����λ
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenReset(void)
{
	PAout(7) = 1;
	inkscreen_delay_ms(200);
	PAout(7) = 0;
	inkscreen_delay_ms(5);
	PAout(1) = 1;
	inkscreen_delay_ms(200);
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreeenClear
  * @Author:       trx
  * @DateTime:     2023��4��23��22��39��
  * @Purpose:      īˮ������
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreeenClear(void)
{
	uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
	uint16_t height = INKSCREEN_HEIGHT;

	/* ���ͺ�/��ɫ���� */
	inkscreen_sendcommand(INKSCREEN_BW);
	for(uint8_t i = 0;i < height;i++)
	{
		for(uint8_t j = 0;j < width;j++)
		{
			inkscreen_senddata(0xff);
		}
	}
	/* ���ͺ�ɫ���� */
	inkscreen_sendcommand(INKSCREEN_RED);
	for(uint8_t i = 0;i < height;i++)
	{
		for(uint8_t j = 0;j < width;j++)
		{
			inkscreen_senddata(0xff);
		}
	}
    /* ��ʾˢ��ָ�� */
	inkscreen_sendcommand(INKSCREEN_DRF);
	inkscreen_readbusy();
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreeenSleep
  * @Author:       trx
  * @DateTime:     2023��4��23��22��47��
  * @Purpose:      īˮ��˯��
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreeenSleep(void)
{
	/* ���� */
	inkscreen_sendcommand(INKSCREEN_POF);
	inkscreen_readbusy();
	/* ���˯�� */
	inkscreen_sendcommand(INKSCREEN_DSLP);
	inkscreen_senddata(INKSCREEN_CHC);
}
/*******************************************************************************
  * @FunctionName: bsp_InkscreenDisplay
  * @Author:       trx
  * @DateTime:     2023��4��23��22��47��
  * @Purpose:      īˮ����ʾ
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InkscreenDisplay(const uint8_t *_blackimage,const uint8_t * _ryimage)
{
	uint8_t width = (INKSCREEN_WIDTH % 8 == 0) ? (INKSCREEN_WIDTH / 8) : (INKSCREEN_WIDTH / 8 + 1);
	uint16_t height = INKSCREEN_HEIGHT;

	/* ���ͺ�ɫ���� */
	inkscreen_sendcommand(INKSCREEN_BW);
	for(uint8_t i = 0;i < height;i++)
	{
		for(uint8_t j = 0;i < width;j++)
		{
			inkscreen_senddata(_blackimage[i + j * width]);
		}
	}
	/* partial out �ų�Ӱ�죿 */
	inkscreen_sendcommand(INKSCREEN_PTOUT);

	/* ���ͺ�ɫ���� */
	inkscreen_sendcommand(INKSCREEN_RED);
	for(uint8_t i = 0;i < height;i++)
	{
		for(uint8_t j = 0;i < width;j++)
		{
			inkscreen_senddata(_ryimage[i + j * width]);
		}
	}
	/* partial out �ų�Ӱ�죿 */
	inkscreen_sendcommand(INKSCREEN_PTOUT);

	/* ��ʾˢ�� */
	inkscreen_sendcommand(INKSCREEN_DRF);
	inkscreen_readbusy();
}

























