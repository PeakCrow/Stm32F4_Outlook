#ifndef	_BSP_KEY_H_
#define	_BSP_KEY_H_

#include "sys.h"


///* ����Ӧ�ó���Ĺ��������������� */
//#define KEY_DOWM_K0		KEY_0_DOWN
//#define KEY_UP_K0		KEY_0_UP
//#define KEY_LONG_K0		KEY_0_LONG

//#define KEY_DOWN_Kup	KEY_up_DOWN
//#define	KEY_UP_Kup		KEY_up_UP
//#define	KEY_LONG_Kup	KEY_up_LONG

/* ����ID */
typedef enum{
	KID_K0 = 0,
	KID_Kup
}KEY_ID_E;

/* �����˲�ʱ��50ms����λ10ms
	ֻ��������⵽50ms״̬�������Ϊ��Ч
*/
#define KEY_FILTER_TIME	5
#define KEY_LONG_TIME	100	/* ��λ10ms������1�룬��Ϊ���� */
/* ����FIFO�õ����� */
#define KEY_FIFO_SIZE	10
/* ÿ��������Ӧ1��ȫ�ֵĽṹ����� */
typedef struct
{
//	uint8_t (*IsKeyDownFunc)(void);/* �������µ��жϺ�����1��ʾ���� */
	uint8_t 	Count;		/* �˲��������� */
	uint16_t 	LongCount;	/* ���������� */
	uint16_t 	LongTime;	/* ��������ʱ�䣬0��ʾ����ⳤ�� */
	uint8_t 	State;		/* ������ǰ״̬�����£����� */
	uint8_t		RepeatSpeed;/* ������������ */
	uint8_t 	RepeatCount;/* �������������� */
}KEY_T;

typedef struct
{
	uint8_t Buf[KEY_FIFO_SIZE];	/* ��ֵ������ */
	uint8_t Read;				/* ��������ָ��1 */
	uint8_t Write;				/* ������дָ�� */
	uint8_t Read2;				/* ��������ָ��2 */
}KEY_FIFO_T;

/* �����ֵ���� */
typedef enum
{
	KEY_NONE = 0,			/* 0 ��ʾ�����¼� */

	KEY_0_DOWN,				/* 1������ */
	KEY_0_UP,				/* 1������ */
	KEY_0_LONG,				/* 1������ */

	KEY_UP_DOWN,			/* 2������ */
	KEY_UP_UP,				/* 2������ */
	KEY_UP_LONG,			/* 2������ */

	/* ��ϼ� */
	KEY_MULTI_DOWM,
	KEY_MULTI_UP,
	KEY_MULTI_LONG,
	
}KEY_ENUM;	



/* ���ⲿ���õĺ������� */
void bsp_InitKey(void);
void bsp_Key_Scan10ms(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);



#endif


