#ifndef	_BSP_KEY_H_
#define	_BSP_KEY_H_

#include "sys.h"


///* 根据应用程序的功能重命名按键宏 */
//#define KEY_DOWM_K0		KEY_0_DOWN
//#define KEY_UP_K0		KEY_0_UP
//#define KEY_LONG_K0		KEY_0_LONG

//#define KEY_DOWN_Kup	KEY_up_DOWN
//#define	KEY_UP_Kup		KEY_up_UP
//#define	KEY_LONG_Kup	KEY_up_LONG

/* 按键ID */
typedef enum{
	KID_K0 = 0,
	KID_Kup
}KEY_ID_E;

/* 按键滤波时间50ms，单位10ms
	只有连续检测到50ms状态不变才认为有效
*/
#define KEY_FILTER_TIME	5
#define KEY_LONG_TIME	100	/* 单位10ms，持续1秒，认为长按 */
/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	10
/* 每个按键对应1个全局的结构体变量 */
typedef struct
{
//	uint8_t (*IsKeyDownFunc)(void);/* 按键按下的判断函数，1表示按下 */
	uint8_t 	Count;		/* 滤波器计数器 */
	uint16_t 	LongCount;	/* 长按计数器 */
	uint16_t 	LongTime;	/* 按键按下时间，0表示不检测长按 */
	uint8_t 	State;		/* 按键当前状态，按下，弹起 */
	uint8_t		RepeatSpeed;/* 连续按键周期 */
	uint8_t 	RepeatCount;/* 连续按键计数器 */
}KEY_T;

typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];	/* 键值缓冲区 */
	uint8_t Read;				/* 缓冲区读指针1 */
	uint8_t Write;				/* 缓冲区写指针 */
	uint8_t Read2;				/* 缓冲区读指针2 */
}KEY_FIFO_T;

/* 定义键值代码 */
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_0_DOWN,				/* 1键按下 */
	KEY_0_UP,				/* 1键弹起 */
	KEY_0_LONG,				/* 1键长按 */

	KEY_UP_DOWN,			/* 2键按下 */
	KEY_UP_UP,				/* 2键弹起 */
	KEY_UP_LONG,			/* 2键长按 */

	/* 组合键 */
	KEY_MULTI_DOWM,
	KEY_MULTI_UP,
	KEY_MULTI_LONG,
	
}KEY_ENUM;	



/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_Key_Scan10ms(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);



#endif


