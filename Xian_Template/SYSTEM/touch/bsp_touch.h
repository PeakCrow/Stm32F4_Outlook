#ifndef _BSP_TOUCH_H_
#define _BSP_TOUCH_H_


#define TP_PRES_DOWN 0X80	//屏幕被按下，取最高位
#define TP_CATH_PRES 0X40   //有按键按下，取次高位
#define CT_MAX_TOUCH 5		//电容屏支持的点数，固定为5点
//触摸屏控制器
typedef struct
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
}_m_tp_dev;

extern _m_tp_dev tp_dev;
//电阻屏芯片连接引脚	   
#define PEN  		PBin(1)  	//T_PEN
#define DOUT 		PBin(2)   	//T_MISO
#define TDIN 		PFout(11)  	//T_MOSI
#define TCLK 		PBout(0)  	//T_SCK
#define TCS  		PCout(13)  	//T_CS  片选引脚
///* 中断引脚 */
//#define TP_INT_EXTI_IRQ                 EXTI1_IRQn
///* 中断服务函数 */
//#define TP_IRQHandler                   EXTI1_IRQHandler
//电阻屏/电容屏 共用函数
uint8_t TP_Scan(uint8_t tp);								//扫描
uint8_t TP_Init(void);								//初始化
void bsp_InitLcdTouch(void);


/************************************************IIC通讯****************************************/


//IO方向设置	将寄存器中的第(2*11)位和下一位置0		寄存器的第(2*11)位置0
#define CT_SDA_IN() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}//PF11输入模式
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;}//PF11输出模式
//IO操作函数
#define CT_IIC_SCL	PBout(0)	//SCL-PB0引脚输出高低电平
#define CT_IIC_SDA	PFout(11)	//SDA-PF11引脚输出高低电平
#define CT_READ_SDA	PFin(11)	//输入SDA-PF11输入高低电平

//IIC所有操作函数
void CT_IIC_Init(void);//初始化IIC的IO口
void CT_IIC_Start(void);//发送IIC开始信号
void CT_IIC_Stop(void);//发送IIC停止信号
void CT_IIC_Send_Byte(uint8_t txd);//IIC发送一个字节
uint8_t CT_IIC_Read_Byte(uint8_t ack);//IIC读取一个字节
uint8_t CT_IIC_Wait_Ack(void);//IIC等待ACK信号
void CT_IIC_Ack(void);//IIC发送ACK信号
void CT_IIC_NAck(void);//IIC不发送ACK信号


/************************************************GT9147驱动****************************************/

//IO操作函数
#define GT_RST	PCout(13)	//GT9147复位引脚
#define GT_INT	PBin(1)		//GT9147中断引脚

//I2C读写命令
#define	GT_CMD_WR	0X28	//写命令
#define GT_CMD_RD	0X29	//读命令

//GT9147部分寄存器定义
#define GT_CTRL_REG		0X8040	//GT9147控制寄存器
#define GT_CFGS_REG		0X8047	//GT9147配置起始地址寄存器
#define GT_CHECK_REG	0X80FF	//GT9147校验和寄存器
#define GT_PID_REG		0X8140	//GT9147产品ID寄存器

#define GT_GSTID_REG	0X814E	//GT9147当前检测到的触摸情况
#define GT_TP1_REG		0X8150	//第一个触摸点数据地址
#define GT_TP2_REG		0X8158	//第二个触摸点数据地址
#define GT_TP3_REG		0X8160	//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168	//第四个触摸点数据地址
#define GT_TP5_REG		0X8170	//第五个触摸点数据地址
#define GT_TOUCH_AREA_L   0X8154
#define GT_TOUCH_AREA_H   0X8155//第一个触摸点的触摸面积

uint8_t GT9147_Send_Cfg(uint8_t mode);
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
void GT947_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
uint8_t GT9147_Init(void);
uint8_t GT9147_Scan(uint8_t mode);


/*******************************电容屏画板功能测试*************************************************/
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color);
void gui_fill_circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color);
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color);
void Load_Drow_Dialog(void);
void ctp_test(void);

#endif

