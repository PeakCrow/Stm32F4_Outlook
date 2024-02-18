#include "bsp_key.h"
/*
	�����ӿڷ���??
	K0����	PE4	�͵�ƽ��ʾ��??
	Kup����	PA0	�ߵ�ƽ��ʾ��??
*/

#define HARD_KEY_NUM	2	/* ʵ�尴������ */
#define KEY_COUNT		3	/* ������������ + 1����ϼ� */

/* ʹ��GPIOʱ�� */
#define ALL_KEY_GPIO_CLK_ENABLE()	{ \
	__HAL_RCC_GPIOA_CLK_ENABLE();	\
	__HAL_RCC_GPIOE_CLK_ENABLE();	\
};

/* ���ζ���IO���Žṹ��ָ�룬IO���źţ������??*/
typedef struct
{
	GPIO_TypeDef* gpio;
	uint16_t pin;
	uint8_t ActiveLevel;
}X_GPIO_T;

/* ����ṹ���??*/
static const X_GPIO_T s_gpio_list[HARD_KEY_NUM] = {
	{GPIOE, GPIO_PIN_4, 1},
	{GPIOA,	GPIO_PIN_0, 1},
	
};

/*
����һ���꺯���򻯺�����??
�ж�GPIO�����Ƿ���Ч����
*/
static KEY_T s_tBtn[KEY_COUNT] = {0};
static KEY_FIFO_T s_tKey;	/* ????FIFO?????????? */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);
static void bsp_SetKeyParam(uint8_t _ucKeyID,uint16_t _LongTime,uint8_t _RepeatSpeed);
/*
*	?????? KeyPinActive
*	����˵��: �жϰ����Ƿ���(Ӳ��??
*	??   ?? _id��������
*	?????? 1����ʾ���£�0��ʾ�ͷ�
*	ʱ��??022????9??5??8??
*/
static uint8_t KeyPinActive(uint8_t _id)
{
	uint8_t level;
	/* IDR�Ĵ���������ȡIO���ŵ����룬16λ�ɶ���A...I */
	//�ߵ�ƽ��??
    //ֻ��ʹ�õ͵�ƽ��Ϊ�����źŵ�ʱ�򲻱ؽ������������㣬��Ϊλ�������??
    //�ߵ�ƽ�������ֻ�е�һ�������ǿ��õģ������ʱ��PA0
    //����
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
*	?????? IsKeyDownFunc
*	����˵��: �жϰ����Ƿ��£���������ϼ���??���??
*	??   ?? _id:����??
*	?????? 1����ʾ�������£�0����ʾ������??
*	ʱ��??022????9??2??8??
*/
static uint8_t IsKeyDownFunc(uint8_t _id)
{
	/* ʵ�嵥�� */
	if(_id < HARD_KEY_NUM)
	{
		uint8_t i;
		uint8_t count = 0;
		uint8_t save = 255;
		/* �ж��м��������� */
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
			return 1;/* ��������������Ч */
		}
		return 0;
	}
	
	/* ????K0Kup */
	if(_id == HARD_KEY_NUM + 0) // 0 - 1 - 2(????)
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
*	?????? bsp_InitKey
*	����˵��: ����������ʼ??
*	??   ?? none
*	?????? none
*	ʱ��??022????9??2??7??
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();	/* ��ʼ��������??*/
	bsp_InitKeyHard();	/* ��ʼ������Ӳ??*/
}
/*
*	?????? bsp_InitKeyHard
*	����˵��: ��ʼ������Ӳ??
*	??   ?? none
*	?????? none
*	ʱ��??022????9??2??7??
*/
static void bsp_InitKeyHard(void)
{
	GPIO_InitTypeDef gpio_init;
	uint8_t i;
	/* ��GPIOʱ�� */
	ALL_KEY_GPIO_CLK_ENABLE();
	
	/* �������еİ���GPIOΪ��������ģ??ʵ��MCU��λ���������״?? */
	gpio_init.Mode = GPIO_MODE_INPUT;			/* ��������ģʽ */
	gpio_init.Pull = GPIO_PULLDOWN;				/* ��������ʹ�� */
	gpio_init.Speed= GPIO_SPEED_FREQ_VERY_HIGH;	/* GPIO�ٶȵȼ� */
	for (i = 0; i < HARD_KEY_NUM; i++)
	{
		gpio_init.Pin = s_gpio_list[i].pin;
		HAL_GPIO_Init(s_gpio_list[i].gpio, &gpio_init);	
	}	
}

static void bsp_InitKeyVar(void)
{
	uint8_t i;
	
	/* �԰���FIFO��дָ��?? */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;
	
	/* ÿ�������ṹ���Ա������һ��ȱʡ??*/
	for(i = 0;i < KEY_COUNT;i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;		/* ???????0???????????? */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;	/* ????????????????????? */
		s_tBtn[i].State = 0;					/* ��???? */
		s_tBtn[i].RepeatSpeed = 5;				/* ??????????????0????????????,5????50ms????????? */
		s_tBtn[i].RepeatCount = 0;				/* ?????????? */
	}
	//????????????????????????????????????????????
	bsp_SetKeyParam(KID_Kup,100,6);//???�Lkey up???????60ms???????
}

/*
*	??????bsp_SetKeyParam 
*	����˵��: �޸İ����Ĳ�??
*	??   ?? _ucKeyID������ID��_LongTime������ʱ�䣻_RepeatSpeed�������ٶ�
*	?????? none
*	ʱ��??022????0??9??6??
*/
void bsp_SetKeyParam(uint8_t _ucKeyID,uint16_t _LongTime,uint8_t _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;		/* ???????0??????????????? */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;/* ??????????????0???????????? */
	s_tBtn[_ucKeyID].RepeatCount = 0;			/* ?????????? */
}

/*
*	?????? bsp_PutKey
*	����˵��: ??����ֵѹ��FIFO�������У�������ģ��һ����??
*	??   ?? _KeyCode��������??
*	?????? none
*	ʱ��??022????0??9??0??
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;
	if(++s_tKey.Write >= KEY_FIFO_SIZE)
		s_tKey.Write = 0;		/* FIFO�ռ�д����Write�ᱻ���¸�ֵΪ0 */
}
/*
*	?????? bsp_GetKey
*	����˵��: �Ӱ���FIFO�������ж�ȡһ����??
*	??   ?? none
*	?????? ��������
*	ʱ��??022????0??9??6??
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;
	if(s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;	/* write��readֵ��ȣ�����û�а�������
								�����Ѿ���FIFO��ȡ����ȫ������*/
	}else
	{
		ret = s_tKey.Buf[s_tKey.Read];		/* ��ȡ��ǰRead�е�??*/
		if(++s_tKey.Read >= KEY_FIFO_SIZE)	/* ���Read��ֵ���ڰ��������� */
		{
			s_tKey.Read = 0;				/* ��Readֵ���¸�ֵΪ0 */
		}
		return ret;
	}
}
/*
*	?????? bsp_DetectKey
*	����˵��: ���һ��������������״̬�����뱻�����Եĵ���
*	??   ?? i���������ŵ���Ŀ
*	?????? none
*	ʱ��??022????0??1??9??
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;
	
	pBtn = &s_tBtn[i];
	if(IsKeyDownFunc(i))//ִ�а������µĴ�??
	{
		if(pBtn->Count < KEY_FILTER_TIME)	//count��Ϊ�˲���������ʼֵΪ25ms
		{
			pBtn->Count = KEY_FILTER_TIME;	//??????????��????��??50ms??????�P??50ms?????????
		}else if(pBtn->Count < 2 * KEY_FILTER_TIME)//???????��??100ms
		{
			pBtn->Count++;					//�˲���������??
		}else
		{
			if(pBtn->State == 0)			//0��ʾ���������ڽ���394�к󣬰���״̬��ʧ��������������ʱ��С��25ms
			{
				pBtn->State = 1;			//��ʱ��??��ʾ��������
				
				/* ���Ͱ�ť���µ���Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 1));//��һ��KEY_ID�ṹ�尴�����£���Ӧ��״??
												 //??*0 + 1 = 1���������KEY_ENUMö���е�1--KEY_0_DOWN
			}
			
			if(pBtn->LongTime > 0)
			{
				if(pBtn->LongCount < pBtn->LongTime)//����������µ�ʱ��С�ڰ������õĳ���ʱ��
				{
					/* ���Ͱ�ť�������µ���Ϣ */
					if(++pBtn->LongCount == pBtn->LongTime)//���������µ�ʱ����ڰ������õĳ���ʱ??
					{
						/* ��ֵ���밴��FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));	//�������KEY_ENUMö���е�3--KEY_0_LONG
					}
				}else
				{
					if(pBtn->RepeatSpeed > 0)
					{
						if(++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* ��������ÿ??0ms����һ����??*/
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else//ִ�а������ֵĴ�������ִ�а���û�а��µĴ���
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
				
				/* ���Ͱ����������Ϣ */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}
		
		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}
/*
*	?????? bsp_KeyScan10ms
*	����˵��: ɨ�����а���������������sustick�ж������Եĵ���??0msһ??
*	??   ?? none
*	?????? none
*	ʱ��??022????0??1??3??
*/
void bsp_Key_Scan10ms(void)
{
	uint8_t i;
	
	for(i = 0;i < KEY_COUNT;i++)
	{
		bsp_DetectKey(i);
	}
}

