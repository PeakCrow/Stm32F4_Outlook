#include "bsp_ws2812b.h"

                //GRB
static uint16_t g_Ws_2812b_lamp_buf[RESET_WORD+24*LED_NUM]={0};

/*******************************************************************************
  * @FunctionName: bsp_InitWs2812b
  * @Author:       trx
  * @DateTime:     2022��6��12�� 18:45:41 
  * @Purpose:      �ɵ���ws2812b��ʼ��
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
  * @DateTime:     2022��6��12�� 16:45:37 
  * @Purpose:      ���ɵ�led�Ƶĸ�λ��������ȥ
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
  * @DateTime:     2022��6��12�� 17:17:09 
  * @Purpose:      �����еĿɵ��ƹر�
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
  * @DateTime:     2022��6��12�� 16:47:24 
  * @Purpose:      �����˸�λ����������еƹ�����ݶ�����Ϊĳ����ɫ
  * @param:        _R����ԭɫ�еĺ�ɫ
  * @param:        _G����ԭɫ�е���ɫ
  * @param:        _B����ԭɫ�е���ɫ
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
  * @DateTime:     2022��6��12�� 17:18:52 
  * @Purpose:      ������_index���ƣ���ɫ����ָ��
  * @param:        _index��Ҫ�����ĵƵ����
  * @param:        _R����ԭɫ�еĺ�ɫ
  * @param:        _G����ԭɫ�е���ɫ
  * @param:        _B����ԭɫ�е���ɫ
  * @return:       0���ɵ��Ƶ������ų��ޣ�1��δ����
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
  * @DateTime:     2022��6��12�� 17:31:51 
  * @Purpose:      ����һ�εƣ����ҿ���ָ����ɫ
  * @param:        _start_index��һ�εƵ���ʼ���
  * @param:        _end_index��һ�εƵ�ĩβ���
  * @param:        _R����ԭɫ�еĺ�ɫ
  * @param:        _G����ԭɫ�е���ɫ
  * @param:        _B����ԭɫ�е���ɫ
  * @return:       0���ɵ��Ƶ������ų��ޣ�1��δ����
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
  * @DateTime:     2022��6��12�� 16:49:00 
  * @Purpose:      ����ɫЧ��
  * @param:        _color0������ɫ���̵���ʼ��ɫ
  * @param:        _color1������ɫ���̵Ľ�����ɫ
  * @param:        _speed������ɫ�����е�ÿ����ɫ���л�ʱ��
  * @return:       none
*******************************************************************************/
void Ws2812b_Gradient_Lamp(unsigned long _color0, unsigned long _color1,uint16_t _speed)
{
    unsigned char Red0, Green0, Blue0;  // ��ʼ��ԭɫ
    uint8_t       Red1, Green1, Blue1;  // �����ԭɫ
    int           RedMinus, GreenMinus, BlueMinus;  // ��ɫ�color1 - color0��
    unsigned char NStep;                            // ��Ҫ����
    float         RedStep, GreenStep, BlueStep;     // ��ɫ����ֵ
    unsigned char i;

    Ws2812b_Set_resetdata();
    // �� �� �� ��ԭɫ�ֽ�
    Red0   = _color0>>8;
    Green0 = _color0>>16;
    Blue0  = _color0;
    
    Red1   = _color1>>8;
    Green1 = _color1>>16;
    Blue1  = _color1;
    
    // ������Ҫ���ٲ���ȡ��ֵ�����ֵ��
    RedMinus   = Red1 - Red0; 
    GreenMinus = Green1 - Green0; 
    BlueMinus  = Blue1 - Blue0;
    
    //�ж���������ֵ��С����a>bΪ�棬��=a������=b
    NStep = ( abs(RedMinus) > abs(GreenMinus) ) ? abs(RedMinus):abs(GreenMinus); 
    NStep = ( NStep > abs(BlueMinus) ) ? NStep:abs(BlueMinus);
    
    // �������ɫ����ֵ
    RedStep   = (float)RedMinus   / NStep;
    GreenStep = (float)GreenMinus / NStep;
    BlueStep  = (float)BlueMinus  / NStep;
    
    // ���俪ʼ
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
  * @DateTime:     2022��6��12�� 18:08:01 
  * @Purpose:      ��ˮ�Ƶ�Ч�����ε��������ǵ������Ļ�Ϩ��
  * @param:        _R����ԭɫ�еĺ�ɫ
  * @param:        _G����ԭɫ�е���ɫ
  * @param:        _B����ԭɫ�е���ɫ
  * @param:        _speed�����ε�����ʱ����
  * @param:        _flag����ˮ�Ƶ���Ч��ѡ��
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



