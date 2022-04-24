#include "sys.h"
#include "ticktim.h"
#include "EventRecorder.h"
#include <stdio.h>
#include "bsp_usart_fifo.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_key.h"


void func1(void)
{
	printf("ï¿½Ø¸ï¿½1.5sï¿½ï¿½ï¿½ï¿½\r\n");
}
void func2(void)
{
	printf("ï¿½Ø¸ï¿½2.0sï¿½ï¿½ï¿½ï¿½\r\n");
}
void func3(void)
{
	printf("ï¿½ï¿½ï¿½ï¿½4.0s\r\n");
}
int main(void)
{	
		vu8 ucKeyCode;	
		HAL_Init();                    	 			/* ³õÊ¼»¯HAL¿â */   
		Stm32_Clock_Init(336,8,2,7);   				/* ÉèÖÃÊ±ÖÓ,168Mhz */
		bsp_InitTimer();											/* ³õÊ¼»¯µÎ´ð¶¨Ê±Æ÷ */
		bsp_InitUart();												/* ³õÊ¼»¯´®¿Ú1ÍâÉè */
		bsp_InitKey();
		EventRecorderInitialize(EventRecordAll, 1U);/* ³õÊ¼»¯ EventRecorder ²¢¿ªÆô */
//	bsp_StartAutoTimer(0,1500,func1);
		bsp_StartAutoTimer(1,2000,func2);
//	bsp_StartOnceTimer(3,4000,func3);
		
	while(1)
	{	
		ucKeyCode = bsp_GetKey();
		if(ucKeyCode != KEY_NONE)
		{
			switch(ucKeyCode)
			{
				case KEY_0_DOWN:
						printf("K0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_0_UP:
						printf("K0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_UP_DOWN:
						printf("KUPï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_UP_UP:
						printf("KUPï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_0_LONG:
						printf("K0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_UP_LONG:
						printf("KUPï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_MULTI_DOWM:
						printf("MULTI_DOWMï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_MULTI_UP:
						printf("MULTI_UPï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;
				case KEY_MULTI_LONG:
						printf("MULTI_LONGï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½\r\n");
					break;				
			}
		}
	}
}
void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();
}
void bsp_RunPer50ms()
{
	uint8_t read;
	const char buf1[] = "ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½1\r\n";
	const char buf2[] = "ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½2\r\n";
	const char buf3[] = "ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½3\r\n";
	const char buf4[] = "ï¿½ï¿½ï¿½Õµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½4\r\n";

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

