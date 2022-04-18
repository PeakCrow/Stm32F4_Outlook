#include "sys.h"
#include "ticktim.h"
#include "EventRecorder.h"
#include <stdio.h>
#include "bsp_usart_fifo.h"
#include <stdlib.h>
#include <string.h>

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
	printf("单次4.0s\r\n");
}
int main(void)
{	
	
    HAL_Init();                    	 			/* 初始化HAL库 */   
    Stm32_Clock_Init(336,8,2,7);   				/* 设置时钟,168Mhz */
	bsp_InitTimer();							/* 初始化滴答定时器 */
	bsp_InitUart();								/* 初始化串口1外设 */
	EventRecorderInitialize(EventRecordAll, 1U);/* 初始化 EventRecorder 并开启 */
	bsp_StartAutoTimer(0,1500,func1);
	bsp_StartAutoTimer(1,2000,func2);
	bsp_StartOnceTimer(3,4000,func3);
	
	while(1)
	{	
		
	}
}

void bsp_RunPer50ms()
{
	uint8_t read;
	const char buf1[] = "接收到串口命令1\r\n";
	const char buf2[] = "接收到串口命令2\r\n";
	const char buf3[] = "接收到串口命令3\r\n";
	const char buf4[] = "接收到串口命令4\r\n";

	if(comGetChar(COM2,&read))
	{
		switch(read)
		{
			case '1':
				comSendBuf(COM2,(uint8_t *)buf1,strlen(buf1));
				break;
			case '2':
				comSendBuf(COM2,(uint8_t *)buf2,strlen(buf2));
				break;
			case '3':
				comSendBuf(COM2,(uint8_t *)buf3,strlen(buf3));
				break;
			case '4':
				comSendBuf(COM2,(uint8_t *)buf4,strlen(buf4));
			default:
				break;
		}
	}
}

