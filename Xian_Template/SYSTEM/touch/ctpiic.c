#include "ctpiic.h"
#include "delay.h"


//控制IIC速度的延时
void CT_Delay(void)
{
	delay_us(2);//IIC通讯的时许间隔要求
}

//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{
	GPIO_InitTypeDef	gpio_initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	//PB0,PF11
	gpio_initure.Pin = GPIO_PIN_0;
	gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
	gpio_initure.Pull = GPIO_PULLUP;//上拉
	gpio_initure.Speed = GPIO_SPEED_HIGH;//高速
	HAL_GPIO_Init(GPIOB,&gpio_initure);
	
	gpio_initure.Pin = GPIO_PIN_11;//PF11设置推挽输出
	HAL_GPIO_Init(GPIOF,&gpio_initure);
	
	CT_IIC_SDA = 1;//PF11输出为高电平
	CT_IIC_SCL = 1;//PB0输出为高电平
	
}
//产生IIC起始信号
//开始信号：SCL为高电平的时候，SDA由高电平向低电平跳变，开始传送数据
void CT_IIC_Start(void)
{
	CT_SDA_OUT();//sda线设置为输出方向
	CT_IIC_SDA = 1;//拉高数据线	SDA由高电平向低电平跳变	
	CT_IIC_SCL = 1;//拉高时钟线
	delay_us(30);//延时30us
	CT_IIC_SDA = 0;//拉低数据线
	CT_Delay();
	CT_IIC_SCL = 0;//只有时钟线的信号为低电平期间，数据线上的高低电平状态才允许变化
}
//产生IIC停止信号
//结束信号：SCL高电平的时候，SDA由低电平向高电平跳变，结束传送数据
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda线输出
	CT_IIC_SCL = 1;
	delay_us(30);
	CT_IIC_SDA = 0;//SDA由低电平向高电平跳变
	CT_Delay();
	CT_IIC_SDA = 1;
}
//主机等待应答信号到来
//返回值：1，接收非应答；
//		  0，接收应答成功；
u8 CT_IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	CT_SDA_IN();//SDA设置为输入，用来接收应答信号，主机释放对SDA数据线的控制，由从机控制SDA线
	CT_IIC_SDA = 1;
	CT_IIC_SCL = 1;//主机scl信号产生高电平，代表此时sda数据线有效
	CT_Delay();
	while(CT_READ_SDA)//判断数据线读取是否为高电平
	{
		ucErrTime++;//如果读取为高电平，则记录一次错误次数
		if(ucErrTime > 250)//如果次数超过250次高电平
		{
			CT_IIC_Stop();//主机发送停止信号
			return 1;//代表着接收到是非应答信号
		}
		CT_Delay();
	}
	CT_IIC_SCL = 0;//scl低电平信号代表数据线信号无效
	return 0;//数据线读取的不是高电平，而是低电平，代表着接收到的是应答信号
}
//主机产生应答信号
//表示想要继续接收数据
//此处主机的应答信号和从机的不同
void CT_IIC_Ack(void)
{
	CT_IIC_SCL = 0;//scl输出低电平，sda数据线信号无效
	CT_SDA_OUT();//数据线配置为输出模式，释放sda控制权
	CT_Delay();
	CT_IIC_SDA = 0;//sda先输出低电平，但此时的sda信号是无效的
	CT_Delay();
	CT_IIC_SCL = 1;//scl输出高电平，sda数据线信号有效，至此产生了一个有效的应答信号
	CT_Delay();
	CT_IIC_SCL = 0;//scl输出低电平，sda数据线信号无效
}

//主机不产生应答信号--ACK应答
void CT_IIC_NAck(void)
{
	CT_IIC_SCL= 0;//scl输出低电平，sda数据线信号无效
	CT_SDA_OUT();//数据线配置为输出模式，释放sda控制权
	CT_Delay();
	CT_IIC_SDA = 1;//sda输出高电平，但此时的sda信号是无效的
	CT_Delay();
	CT_IIC_SCL = 1;//scl输出高电平，sda信号有效，电平为高电平，代表非应答信号
	CT_Delay();
	CT_IIC_SCL = 0;//scl输出低电平，sda数据线信号无效
}

//IIC发送一个字节
//只发送一个字节，可以不用接收应答/非应答信号
void CT_IIC_Send_Byte(u8 txd)
{
	u8 t;
	CT_SDA_OUT();//sda数据线配置为输出模式
	CT_IIC_SCL = 0;//拉低时钟线，sda数据线无效，进行高低电平(数据)的切换
	CT_Delay();
	for(t = 0;t < 8;t++)
	{
		CT_IIC_SDA = (txd & 0x80) >> 7;//取出最高位，此时为1000 0000，接着右移7位，只剩下1(数据示例)
		txd <<= 1;//数据左移去掉最高位
		CT_IIC_SCL = 1;//scl时钟线拉高，上面的数据变为有效数据
		CT_Delay();
		CT_IIC_SCL = 0;//拉低时钟线，sda数据线无效，准备进行下一次的高低电平(数据)的切换
		CT_Delay();
	}
}
//读一个字节，ack=1时，发送ack，ack= 0，发送nack
u8 CT_IIC_Read_Byte(u8 ack)
{
	u8 i,receive = 0;
	CT_SDA_IN();//SDA数据线设置为输入
	delay_us(30);
	for(i = 0;i < 8;i++)
	{
		CT_IIC_SCL = 0;//scl时钟线拉低，此时sda数据线信号无效，进行高低电平(数据)的切换
		CT_Delay();
		CT_IIC_SCL = 1;//scl时钟线拉高，此时sda数据线信号有效
		receive <<= 1;//IIC接收数据时字节的高位在前
		if(CT_READ_SDA)//如果sda数据线读取为高电平，读取的数据自加
			receive++;//如果sda数据线读取为低电平，只进行左移增0，不进行自加
	}
	if(!ack)
		CT_IIC_NAck();//发送nack
	else
		CT_IIC_Ack();//发送ack
	return receive;
}

