#ifndef   _BSP_WS2812B_H_
#define   _BSP_WS2812B_H_

#include "sys.h"


#define RESET_WORD 	240
#define LED_NUM		12
#define CODE1 		70
#define CODE0 		30

typedef enum _run_lamp_state
{
	gradua_on = 0,	//��ε���
	signal_on = 1,	//���ε���
}run_lamp_state;



#define   White        0xFFFFFF  	// ��ɫ
#define   Black        0x000000  	// ��ɫ
#define   Blue         0x0000ff  	// ��ɫ
#define   Red          0x00FF00  	// ��ɫ
#define   Green        0xff0000  	// ��ɫ
#define	  Yellow	   0xffff00		// ��ɫ
void bsp_InitWs2812b(void);
void Ws2812b_Set_resetdata(void);
void Ws2812b_Set_Alloff(void);
void Ws2812b_Rgb_SetColor(uint8_t _R,uint8_t _G,uint8_t _B);
uint8_t Ws2812b_Rgb_SetIndexColor(uint8_t _index,uint8_t _R,uint8_t _G,uint8_t _B);
uint8_t Ws2812b_Rgb_SetIndexPartColor(uint8_t _start_index,uint8_t _end_index,uint8_t _R,uint8_t _G,uint8_t _B);
void Ws2812b_Gradient_Lamp(unsigned long _color0, unsigned long _color1,uint16_t _speed);
void Ws2812b_Run_Water_Lamp(uint8_t _R,uint8_t _G,uint8_t _B,uint16_t _speed,run_lamp_state _flag);
#endif

