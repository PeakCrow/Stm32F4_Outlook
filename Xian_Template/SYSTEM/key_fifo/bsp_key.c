#include "bsp_key.h"


/*
	按键接口分配：
	K0键：	PE4	低电平表示按下
	Kup键：	PA0	高电平表示按下

*/

#define HARD_KEY_NUM	2	/* 实体按键个数 */
#define KEY_COUNT		3	/* 两个独立按键 + 1个组合键 */

/* 使能GPIO时钟 */
#define ALL_KEY_GPIO_CLK_ENABLE()	{ \
	__HAL_RCC_GPIOA_CLK_ENABLE();	\
	__HAL_RCC_GPIOE_CLK_ENABLE();	\
};

/* 依次定义IO引脚结构体指针，IO引脚号，激活电平 */
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	uint8_t ActiveLevel;
}X_GPIO_T;

/* 定义结构体变量 */
static const X_GPIO_T s_gpio_list[HARD_KEY_NUM] = {
	{GPIOE, GPIO_PIN_4, 1},
	{GPIOA,	GPIO_PIN_0, 1},
	
};

/*
定义一个宏函数简化后续代码
判断GPIO引脚是否有效按下
*/
static KEY_T s_tBtn[KEY_COUNT] = {0};
static KEY_FIFO_T s_tKey;	/* 按键FIFO变量，结构体 */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

/*
*	函 数 名: KeyPinActive
*	功能说明: 判断按键是否按下(硬件层)
*	形    参: _id：按键号
*	返 回 值: 1：表示按下；0表示释放
*	时间：2022年4月19日22点17分
*/
static uint8_t KeyPinActive(uint8_t _id)
{
	uint8_t level;
	/* IDR寄存器用来读取IO引脚的输入，16位可读，A...I */
	//高电平按下
	if((s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == 1
		|| (s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin) == 16)
	{
		level = 1;
	}
	else 
	{
		level = 0;
	}
	//printf("%d %d\r\n",(s_gpio_list[_id].gpio->IDR & s_gpio_list[_id].pin),level);
	if(level == s_gpio_list[_id].ActiveLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
*	函 数 名: IsKeyDownFunc
*	功能说明: 判断按键是否按下，单键和组合键区分(软件层)
*	形    参: _id:按键号
*	返 回 值: 1：表示按键按下；0：表示按键释放
*	时间：2022年4月19日22点28分
*/
static uint8_t IsKeyDownFunc(uint8_t _id)
{
	/* 实体单键 */
	if(_id < HARD_KEY_NUM)
	{
		uint8_t i;
		uint8_t count = 0;
		uint8_t save = 255;
		/* 判断有几个键按下 */
		for(i = 0;i < HARD_KEY_NUM;i++)
		{
			if(KeyPinActive(i))
			{
				count++;
				save = i;
			}
		}
		//printf("%d %d %d\r\n",count,save,_id);
		if(count == 1 && save == _id)
		{
			return 1;/* 单个按键按下有效 */
		}
		return 0;
	}
	
	/* 组合键K0Kup */
	if(_id == HARD_KEY_NUM + 0) // 0 - 1 - 2(组合键)
	{
		if(KeyPinActive(KID_K0) && KeyPinActive(KID_Kup))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
/*
*	函 数 名: bsp_InitKey
*	功能说明: 按键驱动初始化
*	形    参: none
*	返 回 值: none
*	时间：2022年4月19日22点37分
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();	/* 初始化按键变量 */
	bsp_InitKeyHard();	/* 初始化按键硬件 */
}
/*
*	函 数 名: bsp_InitKeyHard
*	功能说明: 初始化按键硬件
*	形    参: none
*	返 回 值: none
*	时间：2022年4月19日22点37分
*/
static void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef gpio_init;
	uint8_t i;
	/* 打开GPIO时钟 */
	ALL_KEY_GPIO_CLK_ENABLE();
	
	/* 配置所有的按键GPIO为浮动输入模式(实际MCU复位后就是输入状态) */
	gpio_init.Mode = GPIO_MODE_INPUT;	/* 设置输入模式 */
	gpio_init.Pull = GPIO_PULLDOWN;		/* 下拉电阻使能 */
	gpio_init.Speed= GPIO_SPEED_FREQ_VERY_HIGH;/* GPIO速度等级 */
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		gpio_init.Pin = s_gpio_list[i].pin;
		HAL_GPIO_Init(s_gpio_list[i].gpio, &gpio_init);	
	}	
}

static void bsp_InitKeyVar(void)
{
	uint8_t i;
	
	/* 对按键FIFO读写指针清0 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;
	
	/* 每个按键结构体成员变量赋一组缺省值 */
	for(i = 0;i < KEY_COUNT;i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;	/* 长按时间0，表示不检测长按 */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;	/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State = 0;					/* 未按下 */
		s_tBtn[i].RepeatSpeed = 0;				/* 按键连发的速度，0表示不支持连按 */
		s_tBtn[i].RepeatCount = 0;				/* 连发计数器 */
	}
	//如果需要单独更改某个按键的参数，可以在此单独重新赋值
	//bsp_SetKeyParam(KID_K0,100,6);
}

/*
*	函 数 名:bsp_SetKeyParam 
*	功能说明: 修改按键的参数
*	形    参: _ucKeyID：按键ID；_LongTime：长按时间；_RepeatSpeed：长按速度
*	返 回 值: none
*	时间：2022年4月20日09点26分
*/
void bsp_SetKeyParam(uint8_t _ucKeyID,uint16_t _LongTime,uint8_t _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;		/* 长按时间，0表示不检测长按事件 */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;/* 按键连发的速度，0表示不检测连发 */
	s_tBtn[_ucKeyID].RepeatCount = 0;			/* 连发计数器 */
}

/*
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入FIFO缓冲区中，可用于模拟一个按键
*	形    参: _KeyCode：按键代码
*	返 回 值: none
*	时间：2022年4月20日09点30分
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;
	if(++s_tKey.Write >= KEY_FIFO_SIZE)
		s_tKey.Write = 0;		/* FIFO空间写满，Write会被重新赋值为0 */
}
/*
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区中读取一个键值
*	形    参: none
*	返 回 值: 按键代码
*	时间：2022年4月20日09点36分
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;
	if(s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;	/* write和read值相等，代表没有按键按下
								或者已经从FIFO中取走了全部按键*/
	}else
	{
		ret = s_tKey.Buf[s_tKey.Read];		/* 读取当前Read中的值 */
		if(++s_tKey.Read >= KEY_FIFO_SIZE)	/* 如果Read的值大于按键的数量 */
		{
			s_tKey.Read = 0;				/* 将Read值重新赋值为0 */
		}
		return ret;
	}
}
/*
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键，非阻塞状态，必须被周期性的调用
*	形    参: i：按键引脚的数目
*	返 回 值: none
*	时间：2022年4月20日11点19分
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;
	
	pBtn = &s_tBtn[i];
	if(IsKeyDownFunc(i))//执行按键按下的处理
	{
		if(pBtn->Count < KEY_FILTER_TIME)	//count作为滤波计数器初始值为25ms
		{
			pBtn->Count = KEY_FILTER_TIME;	//如果按键按下的时间小于50ms，就将其赋值为50ms，代表按下
		}else if(pBtn->Count < 2 * KEY_FILTER_TIME)//按下时间小于100ms
		{
			pBtn->Count++;					//滤波计数器自增
		}else
		{
			if(pBtn->State == 0)			//0表示按键弹起，在进入394行后，按下状态消失，表明按键按下时间小于25ms
			{
				pBtn->State = 1;			//此时赋值1表示按键按下
				
				/* 发送按钮按下的消息 */
				bsp_PutKey((uint8_t)(3 * i + 1));//第一个KEY_ID结构体按键按下，对应的状态
												 //为3*0 + 1 = 1，代表的是KEY_ENUM枚举中的1--KEY_0_DOWN
			}
			
			if(pBtn->LongTime > 0)
			{
				if(pBtn->LongCount < pBtn->LongTime)//如果按键按下的时间小于按键配置的长按时间
				{
					/* 发送按钮持续按下的消息 */
					if(++pBtn->LongCount == pBtn->LongTime)//当按键按下的时间等于按键配置的长按时间
					{
						/* 键值放入按键FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));	//代表的是KEY_ENUM枚举中的3--KEY_0_LONG
					}
				}else
				{
					if(pBtn->RepeatSpeed > 0)
					{
						if(++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* 长按键后，每隔10ms发送一个按键 */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else//执行按键松手的处理，或者执行按键没有按下的处理
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if(pBtn->State == 1)
			{
				pBtn->State = 0;
				
				/* 发送按键弹起的消息 */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}
		
		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}
/*
*	函 数 名: bsp_KeyScan10ms
*	功能说明: 扫描所有按键，非阻塞，被sustick中断周期性的调用，10ms一次
*	形    参: none
*	返 回 值: none
*	时间：2022年4月20日11点23分
*/
void bsp_Key_Scan10ms(void)
{
	uint8_t i;
	
	for(i = 0;i < KEY_COUNT;i++)
	{
		bsp_DetectKey(i);
	}
}

