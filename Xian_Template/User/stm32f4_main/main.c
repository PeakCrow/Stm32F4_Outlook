#include "sys.h"
#include "delay.h"
#include "EventRecorder.h"
#include "stdio.h"
/************************************************
 ALIENTEK 探索者STM32F407开发板实验0-1
 Template工程模板-新建工程章节使用-HAL库版本
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

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
	printf("单词4.0s调用\r\n");
}
int main(void)
{	
    HAL_Init();                    	 			//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);   				//设置时钟,168Mhz
	bsp_InitTimer();
	/* 初始化 EventRecorder 并开启 */
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
	bsp_StartAutoTimer(0,1500,func1);
	bsp_StartAutoTimer(1,2000,func2);
	bsp_StartOnceTimer(2,4000,func3);
	while(1)
	{		
		printf("K2按键按下\r\n");
		bsp_DelayMs(3000);
		printf("K1按键按下\r\n");	
		bsp_DelayMs(3000);
	}
}


