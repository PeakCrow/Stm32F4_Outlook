#include "sys.h"
#include "ticktim.h"
#include "EventRecorder.h"
#include "stdio.h"

void func1(void)
{
	printf("重复1.5s调用\r\n");
}
void func2(void)
{
	printf("重复2.0s调用\r\n");
}
void func3(void)
{
	printf("单次4.0s调用\r\n");
}
int main(void)
{	
    HAL_Init();                    	 			//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);   				//设置时钟,168Mhz
	bsp_InitTimer();
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
	EventRecorderStart();
	bsp_StartAutoTimer(0,1500,func1);
	bsp_StartAutoTimer(1,2000,func2);
	bsp_StartOnceTimer(2,4000,func3);
	while(1)
	{	
		EventStartA(0);
		printf("K2按键按下\r\n");
		EventStopA(0);
		
		EventStartA(1);
		bsp_DelayMs(3000);
		EventStopA(1);
		
		EventStartA(2);
		printf("K1按键按下\r\n");	
		bsp_DelayMs(3000);
		EventStopA(2);
	}
}


