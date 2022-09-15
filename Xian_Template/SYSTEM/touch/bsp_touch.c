#include "sys.h"

_m_tp_dev tp_dev = 
{
	TP_Init,
	TP_Scan,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
};

//默认为touchtype=0的数据.
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;

//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
void TP_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  //num为要写入的数据，TDIN为T_SCK引脚 写1
		else TDIN=0;   //写0
		num<<=1;    
		TCLK=0; //时钟引脚进行上升沿变化
		bsp_DelayUS(1);
		TCLK=1;		//上升沿有效	        
	}		 			    
} 

//SPI读数据 
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
uint16_t TP_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK=0;		//先拉低时钟 	 
	TDIN=0; 	//拉低数据线
	TCS=0; 		//选中触摸屏IC
	TP_Write_Byte(CMD);//发送命令字，想要读取数据，需要先发送想要读取数据的相关命令
	bsp_DelayUS(6);//ADS7846的转换时间最长为6us
	TCLK=0; //L52中的函数会将TCLK引脚拉高
	bsp_DelayUS(1);    	   
	TCLK=1;		//给1个时钟，清除BUSY
	bsp_DelayUS(1);    
	TCLK=0; //下降沿	     	    
	for(count=0;count<16;count++)//读出16位数据,只有高12位有效 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//下降沿有效  	    	   
		bsp_DelayUS(1);    
 		TCLK=1;
 		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//只有高12位有效.去掉无效的低四位
	TCS=1;		//释放片选	 
	return(Num);   
}
//触摸屏初始化  		    
//返回值:0,没有进行校准
//       1,进行过校准
uint8_t TP_Init(void)
{	
	if(lcddev.id==0X5510)		//电容触摸屏
	{
		if(GT9147_Init()==0)	//是GT9147
		{ 
			tp_dev.scan=GT9147_Scan;	//扫描函数指向GT9147触摸屏扫描
		}
		tp_dev.touchtype|=0X80;	//电容屏 最高位为1代表电容屏
		tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏 0为竖屏
		return 0;
	}
	return 1; 									 
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值 
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];//局部数组变量
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
		buf[i]=TP_Read_AD(xy);//读取5次数据放到数组中		 		    
	for(i=0;i<READ_TIMES-1; i++)//排序，丢弃掉(仅仅是)最后一个值
	{
		for(j=i+1;j<READ_TIMES;j++)//丢弃掉第一个值
		{
			if(buf[i]>buf[j])//升序排列
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)
		sum+=buf[i];//求和
	temp=sum/(READ_TIMES-2*LOST_VAL);//取平均
	return temp;   
} 
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);//读取x坐标
	ytemp=TP_Read_XOY(CMD_RDY);//读取y坐标									   
	//if(xtemp<100||ytemp<100)return 0;//读数失败
	*x=xtemp;
	*y=ytemp;
	return 1;//读数成功
}

//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 50 //误差范围 
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;  
	
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)//读取失败，退出函数
		return(0);
    flag=TP_Read_XY(&x2,&y2);	  
	
    if(flag==0)//读取失败，退出函数
		return(0);   
	
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else 
		return 0;	  
}  



//////////////////////////////////////////////////////////////////////////////////		  
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t TP_Scan(uint8_t tp)
{			   
	if(PEN==0)//PEN == PB1引脚如果产生低电平代表有触摸按下
	{
		if(tp)
			TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//读取物理坐标
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//读取屏幕坐标
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//将结果转换为屏幕坐标
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//之前没有被按下
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下  
			tp_dev.x[4]=tp_dev.x[0];//记录第一次按下时的坐标
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);//标记按键松开	
		}else//之前就没有被按下
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}


void bsp_InitLcdTouch()
{
	tp_dev.init();
}

/*********************************IIC通讯******************************************/
//控制IIC速度的延时
void CT_Delay(void)
{
	bsp_DelayUS(2);//IIC通讯的时许间隔要求
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
	bsp_DelayUS(30);//延时30us
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
	bsp_DelayUS(30);
	CT_IIC_SDA = 0;//SDA由低电平向高电平跳变
	CT_Delay();
	CT_IIC_SDA = 1;
}
//主机等待应答信号到来
//返回值：1，接收非应答；
//		  0，接收应答成功；
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
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
void CT_IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
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
uint8_t CT_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive = 0;
	CT_SDA_IN();//SDA数据线设置为输入
	bsp_DelayUS(30);
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

/*********************************************GT9147驱动***************************************/

//GT9147配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
//flash原有版本号,才会更新配置.
const uint8_t GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};  
//发送GT9147配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
uint8_t GT9147_Send_Cfg(uint8_t mode)
{
	uint8_t buf[2];
	uint8_t i=0;
	buf[0]=0;
	buf[1]=mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)buf[0]+=GT9147_CFG_TBL[i];//计算校验和
    buf[0]=(~buf[0])+1;
	GT9147_WR_Reg(GT_CFGS_REG,(uint8_t*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//发送寄存器配置
	GT9147_WR_Reg(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
	return 0;
} 
//向GT9147写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t ret=0;
	CT_IIC_Start();	//使用IIC总线发送开始信号
 	CT_IIC_Send_Byte(GT_CMD_WR);   	//发送写命令 	 
	CT_IIC_Wait_Ack();//等待应答
	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//发数据
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//产生一个停止条件	    
	return ret; 
}
//从GT9147读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void GT9147_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(GT_CMD_WR);   //发送写命令 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(GT_CMD_RD);   //发送读命令		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据	  
	} 
    CT_IIC_Stop();//产生一个停止条件    
} 
//初始化GT9147触摸屏
//返回值:0,初始化成功;1,初始化失败 
uint8_t GT9147_Init(void)
{
	uint8_t temp[5]; 
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	
    //PB1
    GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1设置为上拉输入
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化
            
    //PC13
    GPIO_Initure.Pin=GPIO_PIN_13;          	//PC13设置为推挽输出
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化
	
	CT_IIC_Init();      	//初始化电容屏的I2C总线  
	GT_RST=0;				//复位
	bsp_DelayMS(10);
 	GT_RST=1;				//释放复位		    
	bsp_DelayMS(10); 
		
	GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1设置为上拉输入
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //输出模式
    GPIO_Initure.Pull=GPIO_NOPULL;          //无上下拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化

//	/* 配置 EXTI 中断源 、配置中断优先级*/
//	HAL_NVIC_SetPriority(TP_INT_EXTI_IRQ , 1, 1);
//    /* 使能中断 */
//	HAL_NVIC_EnableIRQ(TP_INT_EXTI_IRQ );
  
	bsp_DelayMS(100);  
	GT9147_RD_Reg(GT_PID_REG,temp,4);//读取产品ID
	temp[4]=0;
	printf("CTP ID2:%s\r\n",temp);	//打印ID
	if(strcmp((char*)temp,"9147")==0)//ID==9147
	{
		temp[0]=0X02;			
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//软复位GT9147
		GT9147_RD_Reg(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
		if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
		{
			printf("Default Ver:%d\r\n",temp[0]);
			GT9147_Send_Cfg(1);//更新并保存配置
		}
		bsp_DelayMS(10);
		temp[0]=0X00;	 
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//结束复位   
		return 0;
	} 
	return 0;
}
const uint16_t GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t GT9147_Scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
	uint8_t tempsta;
	uint8_t temp_area1,temp_area2,temp_area3;
	static uint8_t t=0;//控制查询间隔,从而降低CPU占用率   
	t++;
	if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{
		GT9147_RD_Reg(GT_GSTID_REG,&mode,1);	//读取触摸点的状态  
		temp=0;
		GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//清标志 			
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta=tp_dev.sta;			//保存当前的tp_dev.sta值
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//保存触点0的数据
			tp_dev.y[4]=tp_dev.y[0];
					GT9147_RD_Reg(GT_TOUCH_AREA_L,&temp_area1,1);
					GT9147_RD_Reg(GT_TOUCH_AREA_H,&temp_area2,1);
					temp_area3 = (temp_area2 << 8) + temp_area1;//第一个触摸点的触摸面积
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//触摸有效?
				{
					GT9147_RD_Reg(GT9147_TPX_TBL[i],buf,4);	//读取XY坐标值

					if(tp_dev.touchtype&0X01)//横屏
					{
						tp_dev.y[i]=((uint16_t)buf[1]<<8)+buf[0];
						tp_dev.x[i]=800-(((uint16_t)buf[3]<<8)+buf[2]);
					}else
					{
						tp_dev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
						tp_dev.y[i]=((uint16_t)buf[3]<<8)+buf[2];
					}  
					//printf("x[%d]:%d,y[%d]:%d,area:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i],temp_area3);
					
				}			
			} 
			res=1;
			if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//非法数据(坐标超出了)
			{ 
				if((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}else					//非法数据,则忽略此次数据(还原原来的)  
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					mode=0X80;		
					tp_dev.sta=tempsta;	//恢复tp_dev.sta
				}
			}
			else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}
		tp_dev.x[0]=0xffff;
		tp_dev.y[0]=0xffff;
		tp_dev.sta&=0XE0;	//清除点有效标记		 
	} 	
	if(t>240)t=10;//重新从10开始计数
	return res;
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
//void TP_IRQHandler(void)
//{
//	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET) //确保是否产生了EXTI Line中断
//	{
//		//LED2_TOGGLE;
//		tp_dev.scan(0);
//		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);       //清除中断标志位
//	}  
//}




/*****************************************************画板例程测试************************************************************/
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color)
{
	if(len==0)
		return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径，这里的半径同样相当于线条的粗细程度
//color:颜色
void gui_fill_circle(uint16_t x0,uint16_t y0,uint16_t r,uint16_t color)
{											  
	uint32_t i;
	uint32_t imax = ((uint32_t)r*707)/1000+1;
	uint32_t sqmax = (uint32_t)r*(uint32_t)r+(uint32_t)r/2;
	uint32_t x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)//如果坐标值小于线条的大小，则退出函数，不进行画线
		return; 
	delta_x=x2-x1; //计算坐标增量 ，每次触摸的时候会连续读两次的坐标数值
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	
	if(delta_x>0)
		incx=1; //设置单步方向 正方向
	else if(delta_x==0)//x的坐标差值为0，代表是垂直线
		incx=0;//垂直线 
	else 
		{
			incx=-1;//负方向
			delta_x=-delta_x;//插值变为负值
		} 
		
	if(delta_y>0)//进行与x坐标值同样的操作
		incy=1; //正方向
	else if(delta_y==0)
		incy=0;//水平线 
	else
	{
		incy=-1;//负方向
		delta_y=-delta_y;
	} 
	
	
	if( delta_x>delta_y)
		distance=delta_x; //选取基本增量坐标轴 
	else 
		distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(TFT_WHITE);//清屏   
 	POINT_COLOR=TFT_BLUE;//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=TFT_RED;//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜色(电容触摸屏用)												 
const uint16_t POINT_COLOR_TBL[5]={TFT_RED,TFT_GREEN,TFT_BLUE,TFT_BROWN,TFT_GRED};  
//电容触摸屏测试函数
void ctp_test(void)
{
	uint8_t t=0;
	uint8_t i=0;	  	    
 	uint16_t lastpos[5][2];		//存放临时的坐标数据 
	while(1)//在while循环中不停的读取tp_dev.x 与 y的坐标，并且进行相应的操作
	{
		tp_dev.scan(0);
		for(t=0;t<5;t++)//这里t循环几次就代表可以同时画几条线,最多只能画5条
		{
			if((tp_dev.sta)&(1<<t))
			{
                //printf("X坐标:%d,Y坐标:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
                    
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else 
				lastpos[t][0]=0XFFFF;
		}
		
		tx_thread_sleep(10);i++;
		//if(i%20==0)LED0=!LED0;
	}	
}

 
