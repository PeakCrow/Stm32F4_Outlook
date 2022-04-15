#include "sys.h"
#include "ticktim.h"
#include "EventRecorder.h"
#include "stdio.h"
#include "bsp_usart_fifo.h"

void func1(void)
{
	printf("重复1.5s调用\r\n");
}
void func2(void)
{
	printf("重复2.0s调用\r\n");
}

uint32_t i = 0;
int main(void)
{	
	
    HAL_Init();                    	 			/* 初始化HAL库 */   
    Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */
	bsp_InitTimer();							/* 初始化滴答定时器 */
	bsp_InitUart();								/* 初始化串口1外设 */
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
	bsp_StartAutoTimer(0,1500,func1);
	bsp_StartAutoTimer(1,2000,func2);
	
	
	while(1)
	{	

	}
}

void bsp_RunPer50ms()
{
	printf("%d\r\n",i++);
}

