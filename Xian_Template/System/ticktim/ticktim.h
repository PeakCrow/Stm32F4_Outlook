#ifndef __TICK_TIM_H
#define __TICK_TIM_H

#include "sys.h"
/*
	�ڴ˶������ɸ������ʱ��ȫ�ֱ���
	ע�⣬��������__IO �� volatile����Ϊ����������жϺ���������ͬʱ�����ʣ��п�����ɱ����������Ż���
*/
#define TIM_TASK_COUNT 4  //�����ʱ���ĸ�����(��ʱ��ID��Χ0-3)

/*��ʱ���ṹ�壬��Ա����������volatile,����C�������Ż�ʱ����������*/
typedef enum{
	TIM_ONCE_MODE = 0,	/*һ�ι���ģʽ*/
	TIM_MULTI_MODE = 1	/*�ظ���ʱ����ģʽ*/
}TIM_MODE_E;
/*��ʱ���ṹ�壬��Ա����������volatile,����C�������Ż�ʱ����������*/
typedef struct{
	volatile uint8_t State;			//0-һ�ι���ģʽ��1-�ظ�����
	volatile uint8_t Mode;			//������ģʽ��һ����
	volatile uint8_t Flag;			//��ʱ�����־
	volatile uint32_t Count;		//������
	volatile uint32_t PreLoad;		//��������װ��ֵ
	void (*callfunc) (void);		//���ܺ���
}SOFT_TIM;

/*�ṩ������C�ļ����õĺ���*/
/*�ඨʱ�����ú���*/
void bsp_InitTimer(void);
void bsp_StartOnceTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void));
void bsp_StartAutoTimer(uint8_t _id,uint32_t _period,void (*callfunc) (void));
void bsp_StopTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);

/*���ڵ��ú��������������while(1)��*/
void bsp_RunPer1ms(void);
void bsp_RunPer10ms(void);
void bsp_RunPer50ms(void);
/*��ʱ�������ʺϷ���while(1)��*/
void bsp_DelayUs(uint32_t n);
void bsp_DelayMs(uint32_t n);
#endif

