#include "sys.h"

				//GRB
static uint16_t g_Ws_2812b_lamp_buf[RESET_WORD+24*LED_NUM]={0};

/*******************************************************************************
  * @FunctionName: bsp_InitWs2812b
  * @Author:       trx
  * @DateTime:     2022年6月12日 18:45:41 
  * @Purpose:      可调灯ws2812b初始化
  * @param:        void               
  * @return:       none
*******************************************************************************/
void bsp_InitWs2812b(void)
{	
	bsp_SetTIMOutPWM(GPIOA,GPIO_PIN_6,TIM3,1,800000,1000);
}

/*******************************************************************************
  * @FunctionName: Ws2812b_Set_resetdata
  * @Author:       trx
  * @DateTime:     2022年6月12日 16:45:37 
  * @Purpose:      将可调led灯的复位数据填充进去
  * @param:        void
  * @return:       none
*******************************************************************************/
void Ws2812b_Set_resetdata(void)
{
	for(int j=0;j<RESET_WORD;j++)
		{
			g_Ws_2812b_lamp_buf[j]=0;
		}
	for(int i = 0;i < LED_NUM*24;i++)
	{
		g_Ws_2812b_lamp_buf[RESET_WORD+i] = 30;
	}
}
/*******************************************************************************
  * @FunctionName: Ws2812b_Set_Alloff
  * @Author:       trx
  * @DateTime:     2022年6月12日 17:17:09 
  * @Purpose:      将所有的可调灯关闭
  * @param:        void               
  * @return:       none
*******************************************************************************/
void Ws2812b_Set_Alloff(void)
{	
	Ws2812b_Set_resetdata();
	HAL_TIM_PWM_Start_DMA(&g_TimHandle,TIM_CHANNEL_1,(uint32_t*)g_Ws_2812b_lamp_buf,sizeof(g_Ws_2812b_lamp_buf)/sizeof(g_Ws_2812b_lamp_buf[0]));
}
/*******************************************************************************
  * @FunctionName: Ws2812b_Rgb_SetColor
  * @Author:       trx
  * @DateTime:     2022年6月12日 16:47:24 
  * @Purpose:      将除了复位数据外的所有灯光的数据都配置为某个颜色
  * @param:        _R：三原色中的红色
  * @param:        _G：三原色中的绿色
  * @param:        _B：三原色中的蓝色
  * @return:       none
*******************************************************************************/
void Ws2812b_Rgb_SetColor(uint8_t _R,uint8_t _G,uint8_t _B)
{
	int i;
	for(int j = 0;j < LED_NUM;j++)
	{
		for(i=0;i<=7;i++)
			{
				g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_G & (1 << (7 -i)) )? CODE1 : CODE0 );
			}
		for(i=8;i<=15;i++)
			{
				g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_R & (1 << (15-i)) )? CODE1 : CODE0 );
			}
		for(i=16;i<=23;i++)
			{
				g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_B & (1 << (23-i)) )? CODE1 : CODE0 );
			}
	}
	HAL_TIM_PWM_Start_DMA(&g_TimHandle,TIM_CHANNEL_1,(uint32_t*)g_Ws_2812b_lamp_buf,sizeof(g_Ws_2812b_lamp_buf)/sizeof(g_Ws_2812b_lamp_buf[0]));
}
/*******************************************************************************
  * @FunctionName: Ws2812b_Rgb_SetIndexColor
  * @Author:       trx
  * @DateTime:     2022年6月12日 17:18:52 
  * @Purpose:      点亮第_index个灯，颜色可以指定
  * @param:        _index：要点亮的灯的序号
  * @param:        _R：三原色中的红色
  * @param:        _G：三原色中的绿色
  * @param:        _B：三原色中的蓝色
  * @return:       0：可调灯的索引号超限；1：未超限
*******************************************************************************/
uint8_t Ws2812b_Rgb_SetIndexColor(uint8_t _index,uint8_t _R,uint8_t _G,uint8_t _B)
{
	int i;
	Ws2812b_Set_resetdata();
	if(_index > LED_NUM)
		return 0;
	for(i=0;i<=7;i++)
		{
			g_Ws_2812b_lamp_buf[RESET_WORD+i + (_index-1)*24]= ( (_G & (1 << (7 -i)) )? CODE1 : CODE0 );
		}
	for(i=8;i<=15;i++)
		{
			g_Ws_2812b_lamp_buf[RESET_WORD+i + (_index-1)*24]= ( (_R & (1 << (15-i)) )? CODE1 : CODE0 );
		}
	for(i=16;i<=23;i++)
		{
			g_Ws_2812b_lamp_buf[RESET_WORD+i + (_index-1)*24]= ( (_B & (1 << (23-i)) )? CODE1 : CODE0 );
		}
	HAL_TIM_PWM_Start_DMA(&g_TimHandle,TIM_CHANNEL_1,(uint32_t*)g_Ws_2812b_lamp_buf,sizeof(g_Ws_2812b_lamp_buf)/sizeof(g_Ws_2812b_lamp_buf[0]));
	return 1;
}
/*******************************************************************************
  * @FunctionName: Ws2812b_Rgb_SetIndexPartColor
  * @Author:       trx
  * @DateTime:     2022年6月12日 17:31:51 
  * @Purpose:      点亮一段灯，并且可以指定颜色
  * @param:        _start_index：一段灯的起始序号
  * @param:        _end_index：一段灯的末尾序号
  * @param:        _R：三原色中的红色
  * @param:        _G：三原色中的绿色
  * @param:        _B：三原色中的蓝色
  * @return:       0：可调灯的索引号超限；1：未超限
*******************************************************************************/
uint8_t Ws2812b_Rgb_SetIndexPartColor(uint8_t _start_index,uint8_t _end_index,uint8_t _R,uint8_t _G,uint8_t _B)
{
	int i = 0;
	Ws2812b_Set_resetdata();
	if(_end_index > LED_NUM)
		return 0;
	for(int j = (_start_index-1);j < _end_index;j++)
	{
		for(i=0;i<=7;i++)
			{
				g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_G & (1 << (7 -i)) )? CODE1 : CODE0 );
			}
		for(i=8;i<=15;i++)
				{
					g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_R & (1 << (15-i)) )? CODE1 : CODE0 );
				}
		for(i=16;i<=23;i++)
			{
				g_Ws_2812b_lamp_buf[RESET_WORD+i + j*24]= ( (_B & (1 << (23-i)) )? CODE1 : CODE0 );
			}
	}	
	HAL_TIM_PWM_Start_DMA(&g_TimHandle,TIM_CHANNEL_1,(uint32_t*)g_Ws_2812b_lamp_buf,sizeof(g_Ws_2812b_lamp_buf)/sizeof(g_Ws_2812b_lamp_buf[0]));
	return 1;
}


/*******************************************************************************
  * @FunctionName: Ws2812b_Gradient_Lamp
  * @Author:       trx
  * @DateTime:     2022年6月12日 16:49:00 
  * @Purpose:      渐变色效果
  * @param:        _color0：渐变色过程的起始颜色
  * @param:        _color1：渐变色过程的结束颜色
  * @param:        _speed：渐变色过程中的每个颜色的切换时间
  * @return:       none
*******************************************************************************/
void Ws2812b_Gradient_Lamp(unsigned long _color0, unsigned long _color1,uint16_t _speed)
{
	unsigned char Red0, Green0, Blue0;  // 起始三原色
	uint8_t 	  Red1, Green1, Blue1;  // 结果三原色
	int			  RedMinus, GreenMinus, BlueMinus;	// 颜色差（color1 - color0）
	unsigned char NStep; 							// 需要几步
	float		  RedStep, GreenStep, BlueStep;		// 各色步进值
	unsigned char i;

	Ws2812b_Set_resetdata();
	// 绿 红 蓝 三原色分解
	Red0   = _color0>>8;
	Green0 = _color0>>16;
	Blue0  = _color0;
	
	Red1   = _color1>>8;
	Green1 = _color1>>16;
	Blue1  = _color1;
	
	// 计算需要多少步（取差值的最大值）
	RedMinus   = Red1 - Red0; 
	GreenMinus = Green1 - Green0; 
	BlueMinus  = Blue1 - Blue0;
	
	//判断两个绝对值大小。。a>b为真，则=a；否则=b
	NStep = ( abs(RedMinus) > abs(GreenMinus) ) ? abs(RedMinus):abs(GreenMinus); 
	NStep = ( NStep > abs(BlueMinus) ) ? NStep:abs(BlueMinus);
	
	// 计算出各色步进值
	RedStep   = (float)RedMinus   / NStep;
	GreenStep = (float)GreenMinus / NStep;
	BlueStep  = (float)BlueMinus  / NStep;
	
	// 渐变开始
	for(i=0; i<NStep; i++)
	{
		Red1   = Red0   + (int)(RedStep   * i);
		Green1 = Green0 + (int)(GreenStep * i);
		Blue1  = Blue0  + (int)(BlueStep  * i);
		
		Ws2812b_Rgb_SetColor(Red1,Green1,Blue1);
		tx_thread_sleep(_speed);
	}
}


/*******************************************************************************
  * @FunctionName: Ws2812b_Run_Water_Lamp
  * @Author:       trx
  * @DateTime:     2022年6月12日 18:08:01 
  * @Purpose:      流水灯灯效，依次点亮，但是点亮过的会熄灭
  * @param:        _R：三原色中的红色
  * @param:        _G：三原色中的绿色
  * @param:        _B：三原色中的蓝色
  * @param:        _speed：依次点亮的时间间隔
  * @param:        _flag：流水灯点亮效果选择
  * @return:       none
*******************************************************************************/
void Ws2812b_Run_Water_Lamp(uint8_t _R,uint8_t _G,uint8_t _B,uint16_t _speed,run_lamp_state _flag)
{
	for(int i = 1;i <= LED_NUM;i++)
		{
			if(_flag == 0)
				Ws2812b_Rgb_SetIndexPartColor(1,i,_R,_G,_B);
			else
				Ws2812b_Rgb_SetIndexColor(i,_R,_G,_B);
			tx_thread_sleep(_speed);
		}
	Ws2812b_Set_Alloff();
	
}



