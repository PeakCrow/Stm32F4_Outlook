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

//Ĭ��Ϊtouchtype=0������.
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;

//SPIд����
//������ICд��1byte����    
//num:Ҫд�������
void TP_Write_Byte(uint8_t num)    
{  
	uint8_t count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  //numΪҪд������ݣ�TDINΪT_SCK���� д1
		else TDIN=0;   //д0
		num<<=1;    
		TCLK=0; //ʱ�����Ž��������ر仯
		bsp_DelayUS(1);
		TCLK=1;		//��������Ч	        
	}		 			    
} 

//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
uint16_t TP_Read_AD(uint8_t CMD)	  
{ 	 
	uint8_t count=0; 	  
	uint16_t Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//���������֣���Ҫ��ȡ���ݣ���Ҫ�ȷ�����Ҫ��ȡ���ݵ��������
	bsp_DelayUS(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; //L52�еĺ����ὫTCLK��������
	bsp_DelayUS(1);    	   
	TCLK=1;		//��1��ʱ�ӣ����BUSY
	bsp_DelayUS(1);    
	TCLK=0; //�½���	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//�½�����Ч  	    	   
		bsp_DelayUS(1);    
 		TCLK=1;
 		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.ȥ����Ч�ĵ���λ
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);   
}
//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
uint8_t TP_Init(void)
{	
	if(lcddev.id==0X5510)		//���ݴ�����
	{
		if(GT9147_Init()==0)	//��GT9147
		{ 
			tp_dev.scan=GT9147_Scan;	//ɨ�躯��ָ��GT9147������ɨ��
		}
		tp_dev.touchtype|=0X80;	//������ ���λΪ1���������
		tp_dev.touchtype|=lcddev.dir&0X01;//������������ 0Ϊ����
		return 0;
	}
	return 1; 									 
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
uint16_t TP_Read_XOY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];//�ֲ��������
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
		buf[i]=TP_Read_AD(xy);//��ȡ5�����ݷŵ�������		 		    
	for(i=0;i<READ_TIMES-1; i++)//���򣬶�����(������)���һ��ֵ
	{
		for(j=i+1;j<READ_TIMES;j++)//��������һ��ֵ
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)
		sum+=buf[i];//���
	temp=sum/(READ_TIMES-2*LOST_VAL);//ȡƽ��
	return temp;   
} 
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);//��ȡx����
	ytemp=TP_Read_XOY(CMD_RDY);//��ȡy����									   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}

//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y) 
{
	uint16_t x1,y1;
 	uint16_t x2,y2;
 	uint8_t flag;  
	
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)//��ȡʧ�ܣ��˳�����
		return(0);
    flag=TP_Read_XY(&x2,&y2);	  
	
    if(flag==0)//��ȡʧ�ܣ��˳�����
		return(0);   
	
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else 
		return 0;	  
}  



//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
uint8_t TP_Scan(uint8_t tp)
{			   
	if(PEN==0)//PEN == PB1������������͵�ƽ�����д�������
	{
		if(tp)
			TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}


void bsp_InitLcdTouch()
{
	tp_dev.init();
}

/*********************************IICͨѶ******************************************/
//����IIC�ٶȵ���ʱ
void CT_Delay(void)
{
	bsp_DelayUS(2);//IICͨѶ��ʱ����Ҫ��
}

//���ݴ���оƬIIC�ӿڳ�ʼ��
void CT_IIC_Init(void)
{
	GPIO_InitTypeDef	gpio_initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	
	//PB0,PF11
	gpio_initure.Pin = GPIO_PIN_0;
	gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;//�������
	gpio_initure.Pull = GPIO_PULLUP;//����
	gpio_initure.Speed = GPIO_SPEED_HIGH;//����
	HAL_GPIO_Init(GPIOB,&gpio_initure);
	
	gpio_initure.Pin = GPIO_PIN_11;//PF11�����������
	HAL_GPIO_Init(GPIOF,&gpio_initure);
	
	CT_IIC_SDA = 1;//PF11���Ϊ�ߵ�ƽ
	CT_IIC_SCL = 1;//PB0���Ϊ�ߵ�ƽ
	
}
//����IIC��ʼ�ź�
//��ʼ�źţ�SCLΪ�ߵ�ƽ��ʱ��SDA�ɸߵ�ƽ��͵�ƽ���䣬��ʼ��������
void CT_IIC_Start(void)
{
	CT_SDA_OUT();//sda������Ϊ�������
	CT_IIC_SDA = 1;//����������	SDA�ɸߵ�ƽ��͵�ƽ����	
	CT_IIC_SCL = 1;//����ʱ����
	bsp_DelayUS(30);//��ʱ30us
	CT_IIC_SDA = 0;//����������
	CT_Delay();
	CT_IIC_SCL = 0;//ֻ��ʱ���ߵ��ź�Ϊ�͵�ƽ�ڼ䣬�������ϵĸߵ͵�ƽ״̬������仯
}
//����IICֹͣ�ź�
//�����źţ�SCL�ߵ�ƽ��ʱ��SDA�ɵ͵�ƽ��ߵ�ƽ���䣬������������
void CT_IIC_Stop(void)
{
	CT_SDA_OUT();//sda�����
	CT_IIC_SCL = 1;
	bsp_DelayUS(30);
	CT_IIC_SDA = 0;//SDA�ɵ͵�ƽ��ߵ�ƽ����
	CT_Delay();
	CT_IIC_SDA = 1;
}
//�����ȴ�Ӧ���źŵ���
//����ֵ��1�����շ�Ӧ��
//		  0������Ӧ��ɹ���
uint8_t CT_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	CT_SDA_IN();//SDA����Ϊ���룬��������Ӧ���źţ������ͷŶ�SDA�����ߵĿ��ƣ��ɴӻ�����SDA��
	CT_IIC_SDA = 1;
	CT_IIC_SCL = 1;//����scl�źŲ����ߵ�ƽ�������ʱsda��������Ч
	CT_Delay();
	while(CT_READ_SDA)//�ж������߶�ȡ�Ƿ�Ϊ�ߵ�ƽ
	{
		ucErrTime++;//�����ȡΪ�ߵ�ƽ�����¼һ�δ������
		if(ucErrTime > 250)//�����������250�θߵ�ƽ
		{
			CT_IIC_Stop();//��������ֹͣ�ź�
			return 1;//�����Ž��յ��Ƿ�Ӧ���ź�
		}
		CT_Delay();
	}
	CT_IIC_SCL = 0;//scl�͵�ƽ�źŴ����������ź���Ч
	return 0;//�����߶�ȡ�Ĳ��Ǹߵ�ƽ�����ǵ͵�ƽ�������Ž��յ�����Ӧ���ź�
}
//��������Ӧ���ź�
//��ʾ��Ҫ������������
//�˴�������Ӧ���źźʹӻ��Ĳ�ͬ
void CT_IIC_Ack(void)
{
	CT_IIC_SCL = 0;//scl����͵�ƽ��sda�������ź���Ч
	CT_SDA_OUT();//����������Ϊ���ģʽ���ͷ�sda����Ȩ
	CT_Delay();
	CT_IIC_SDA = 0;//sda������͵�ƽ������ʱ��sda�ź�����Ч��
	CT_Delay();
	CT_IIC_SCL = 1;//scl����ߵ�ƽ��sda�������ź���Ч�����˲�����һ����Ч��Ӧ���ź�
	CT_Delay();
	CT_IIC_SCL = 0;//scl����͵�ƽ��sda�������ź���Ч
}

//����������Ӧ���ź�--ACKӦ��
void CT_IIC_NAck(void)
{
	CT_IIC_SCL= 0;//scl����͵�ƽ��sda�������ź���Ч
	CT_SDA_OUT();//����������Ϊ���ģʽ���ͷ�sda����Ȩ
	CT_Delay();
	CT_IIC_SDA = 1;//sda����ߵ�ƽ������ʱ��sda�ź�����Ч��
	CT_Delay();
	CT_IIC_SCL = 1;//scl����ߵ�ƽ��sda�ź���Ч����ƽΪ�ߵ�ƽ�������Ӧ���ź�
	CT_Delay();
	CT_IIC_SCL = 0;//scl����͵�ƽ��sda�������ź���Ч
}

//IIC����һ���ֽ�
//ֻ����һ���ֽڣ����Բ��ý���Ӧ��/��Ӧ���ź�
void CT_IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	CT_SDA_OUT();//sda����������Ϊ���ģʽ
	CT_IIC_SCL = 0;//����ʱ���ߣ�sda��������Ч�����иߵ͵�ƽ(����)���л�
	CT_Delay();
	for(t = 0;t < 8;t++)
	{
		CT_IIC_SDA = (txd & 0x80) >> 7;//ȡ�����λ����ʱΪ1000 0000����������7λ��ֻʣ��1(����ʾ��)
		txd <<= 1;//��������ȥ�����λ
		CT_IIC_SCL = 1;//sclʱ�������ߣ���������ݱ�Ϊ��Ч����
		CT_Delay();
		CT_IIC_SCL = 0;//����ʱ���ߣ�sda��������Ч��׼��������һ�εĸߵ͵�ƽ(����)���л�
		CT_Delay();
	}
}
//��һ���ֽڣ�ack=1ʱ������ack��ack= 0������nack
uint8_t CT_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive = 0;
	CT_SDA_IN();//SDA����������Ϊ����
	bsp_DelayUS(30);
	for(i = 0;i < 8;i++)
	{
		CT_IIC_SCL = 0;//sclʱ�������ͣ���ʱsda�������ź���Ч�����иߵ͵�ƽ(����)���л�
		CT_Delay();
		CT_IIC_SCL = 1;//sclʱ�������ߣ���ʱsda�������ź���Ч
		receive <<= 1;//IIC��������ʱ�ֽڵĸ�λ��ǰ
		if(CT_READ_SDA)//���sda�����߶�ȡΪ�ߵ�ƽ����ȡ�������Լ�
			receive++;//���sda�����߶�ȡΪ�͵�ƽ��ֻ����������0���������Լ�
	}
	if(!ack)
		CT_IIC_NAck();//����nack
	else
		CT_IIC_Ack();//����ack
	return receive;
}

/*********************************************GT9147����***************************************/

//GT9147���ò�����
//��һ���ֽ�Ϊ�汾��(0X60),���뱣֤�µİ汾�Ŵ��ڵ���GT9147�ڲ�
//flashԭ�а汾��,�Ż��������.
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
//����GT9147���ò���
//mode:0,���������浽flash
//     1,�������浽flash
uint8_t GT9147_Send_Cfg(uint8_t mode)
{
	uint8_t buf[2];
	uint8_t i=0;
	buf[0]=0;
	buf[1]=mode;	//�Ƿ�д�뵽GT9147 FLASH?  ���Ƿ���籣��
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)buf[0]+=GT9147_CFG_TBL[i];//����У���
    buf[0]=(~buf[0])+1;
	GT9147_WR_Reg(GT_CFGS_REG,(uint8_t*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//���ͼĴ�������
	GT9147_WR_Reg(GT_CHECK_REG,buf,2);//д��У���,�����ø��±��
	return 0;
} 
//��GT9147д��һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:д���ݳ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t ret=0;
	CT_IIC_Start();	//ʹ��IIC���߷��Ϳ�ʼ�ź�
 	CT_IIC_Send_Byte(GT_CMD_WR);   	//����д���� 	 
	CT_IIC_Wait_Ack();//�ȴ�Ӧ��
	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    	CT_IIC_Send_Byte(buf[i]);  	//������
		ret=CT_IIC_Wait_Ack();
		if(ret)break;  
	}
    CT_IIC_Stop();					//����һ��ֹͣ����	    
	return ret; 
}
//��GT9147����һ������
//reg:��ʼ�Ĵ�����ַ
//buf:���ݻ�������
//len:�����ݳ���			  
void GT9147_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i; 
 	CT_IIC_Start();	
 	CT_IIC_Send_Byte(GT_CMD_WR);   //����д���� 	 
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//���͸�8λ��ַ
	CT_IIC_Wait_Ack(); 	 										  		   
 	CT_IIC_Send_Byte(reg&0XFF);   	//���͵�8λ��ַ
	CT_IIC_Wait_Ack();  
 	CT_IIC_Start();  	 	   
	CT_IIC_Send_Byte(GT_CMD_RD);   //���Ͷ�����		   
	CT_IIC_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //������	  
	} 
    CT_IIC_Stop();//����һ��ֹͣ����    
} 
//��ʼ��GT9147������
//����ֵ:0,��ʼ���ɹ�;1,��ʼ��ʧ�� 
uint8_t GT9147_Init(void)
{
	uint8_t temp[5]; 
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//����GPIOBʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	
    //PB1
    //GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1����Ϊ��������
    //GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��
            
    //PC13
    GPIO_Initure.Pin=GPIO_PIN_13;          	//PC13����Ϊ�������
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //��ʼ��
	
	CT_IIC_Init();      	//��ʼ����������I2C����  
	GT_RST=0;				//��λ
	bsp_DelayMS(10);
 	GT_RST=1;				//�ͷŸ�λ		    
	bsp_DelayMS(10); 
		
	GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1����Ϊ��������
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;  //���ģʽ
    GPIO_Initure.Pull=GPIO_NOPULL;          //��������
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //����
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //��ʼ��

//	/* ���� EXTI �ж�Դ �������ж����ȼ�*/
//	HAL_NVIC_SetPriority(TP_INT_EXTI_IRQ , 1, 1);
//    /* ʹ���ж� */
//	HAL_NVIC_EnableIRQ(TP_INT_EXTI_IRQ );
  
	bsp_DelayMS(100);  
	GT9147_RD_Reg(GT_PID_REG,temp,4);//��ȡ��ƷID
	temp[4]=0;
	printf("CTP ID2:%s\r\n",temp);	//��ӡID
	if(strcmp((char*)temp,"9147")==0)//ID==9147
	{
		temp[0]=0X02;			
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//��λGT9147
		GT9147_RD_Reg(GT_CFGS_REG,temp,1);//��ȡGT_CFGS_REG�Ĵ���
		if(temp[0]<0X60)//Ĭ�ϰ汾�Ƚϵ�,��Ҫ����flash����
		{
			printf("Default Ver:%d\r\n",temp[0]);
			GT9147_Send_Cfg(1);//���²���������
		}
		bsp_DelayMS(10);
		temp[0]=0X00;	 
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//������λ   
		return 0;
	} 
	return 0;
}
const uint16_t GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
//ɨ�败����(���ò�ѯ��ʽ)
//mode:0,����ɨ��.
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
uint8_t GT9147_Scan(uint8_t mode)
{
	uint8_t buf[4];
	uint8_t i=0;
	uint8_t res=0;
	uint8_t temp;
	uint8_t tempsta;
	uint8_t temp_area1,temp_area2,temp_area3;
	static uint8_t t=0;//���Ʋ�ѯ���,�Ӷ�����CPUռ����   
	t++;
	if((t%10)==0||t<10)//����ʱ,ÿ����10��CTP_Scan�����ż��1��,�Ӷ���ʡCPUʹ����
	{
		GT9147_RD_Reg(GT_GSTID_REG,&mode,1);	//��ȡ�������״̬  
		temp=0;
		GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//���־ 			
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//����ĸ���ת��Ϊ1��λ��,ƥ��tp_dev.sta���� 
			tempsta=tp_dev.sta;			//���浱ǰ��tp_dev.staֵ
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//���津��0������
			tp_dev.y[4]=tp_dev.y[0];
					GT9147_RD_Reg(GT_TOUCH_AREA_L,&temp_area1,1);
					GT9147_RD_Reg(GT_TOUCH_AREA_H,&temp_area2,1);
					temp_area3 = (temp_area2 << 8) + temp_area1;//��һ��������Ĵ������
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//������Ч?
				{
					GT9147_RD_Reg(GT9147_TPX_TBL[i],buf,4);	//��ȡXY����ֵ

					if(tp_dev.touchtype&0X01)//����
					{
						tp_dev.y[i]=((uint16_t)buf[1]<<8)+buf[0];
						tp_dev.x[i]=800-(((uint16_t)buf[3]<<8)+buf[2]);
					}else
					{
						tp_dev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
						tp_dev.y[i]=((uint16_t)buf[3]<<8)+buf[2];
					}  
					printf("x[%d]:%d,y[%d]:%d,area:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i],temp_area3);
					
				}			
			} 
			res=1;
			if(tp_dev.x[0]>lcddev.width||tp_dev.y[0]>lcddev.height)//�Ƿ�����(���곬����)
			{ 
				if((mode&0XF)>1)		//��������������,�򸴵ڶ�����������ݵ���һ������.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//����һ��,��������������10��,�Ӷ����������
				}else					//�Ƿ�����,����Դ˴�����(��ԭԭ����)  
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					mode=0X80;		
					tp_dev.sta=tempsta;	//�ָ�tp_dev.sta
				}
			}
			else t=0;					//����һ��,��������������10��,�Ӷ����������
		}
	}
	if((mode&0X8F)==0X80)//�޴����㰴��
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);	//��ǰ����ɿ�
		}
		tp_dev.x[0]=0xffff;
		tp_dev.y[0]=0xffff;
		tp_dev.sta&=0XE0;	//�������Ч���		 
	} 	
	if(t>240)t=10;//���´�10��ʼ����
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
//	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET) //ȷ���Ƿ������EXTI Line�ж�
//	{
//		//LED2_TOGGLE;
//		tp_dev.scan(0);
//		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);       //����жϱ�־λ
//	}  
//}




/*****************************************************�������̲���************************************************************/
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(uint16_t x0,uint16_t y0,uint16_t len,uint16_t color)
{
	if(len==0)
		return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶������İ뾶ͬ���൱�������Ĵ�ϸ�̶�
//color:��ɫ
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
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t size,uint16_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)//�������ֵС�������Ĵ�С�����˳������������л���
		return; 
	delta_x=x2-x1; //������������ ��ÿ�δ�����ʱ������������ε�������ֵ
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	
	if(delta_x>0)
		incx=1; //���õ������� ������
	else if(delta_x==0)//x�������ֵΪ0�������Ǵ�ֱ��
		incx=0;//��ֱ�� 
	else 
		{
			incx=-1;//������
			delta_x=-delta_x;//��ֵ��Ϊ��ֵ
		} 
		
	if(delta_y>0)//������x����ֵͬ���Ĳ���
		incy=1; //������
	else if(delta_y==0)
		incy=0;//ˮƽ�� 
	else
	{
		incy=-1;//������
		delta_y=-delta_y;
	} 
	
	
	if( delta_x>delta_y)
		distance=delta_x; //ѡȡ�������������� 
	else 
		distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
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
//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(TFT_WHITE);//����   
 	POINT_COLOR=TFT_BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=TFT_RED;//���û�����ɫ 
}
////////////////////////////////////////////////////////////////////////////////
//5�����ص����ɫ(���ݴ�������)												 
const uint16_t POINT_COLOR_TBL[5]={TFT_RED,TFT_GREEN,TFT_BLUE,TFT_BROWN,TFT_GRED};  
//���ݴ��������Ժ���
void ctp_test(void)
{
	uint8_t t=0;
	uint8_t i=0;	  	    
 	uint16_t lastpos[5][2];		//�����ʱ���������� 
	while(1)//��whileѭ���в�ͣ�Ķ�ȡtp_dev.x �� y�����꣬���ҽ�����Ӧ�Ĳ���
	{
		tp_dev.scan(0);
		for(t=0;t<5;t++)//����tѭ�����ξʹ������ͬʱ��������,���ֻ�ܻ�5��
		{
			if((tp_dev.sta)&(1<<t))
			{
                //printf("X����:%d,Y����:%d\r\n",tp_dev.x[0],tp_dev.y[0]);
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
                    
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//����
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<20)
					{
						Load_Drow_Dialog();//���
					}
				}
			}else 
				lastpos[t][0]=0XFFFF;
		}
		
		tx_thread_sleep(10);i++;
		//if(i%20==0)LED0=!LED0;
	}	
}

 
