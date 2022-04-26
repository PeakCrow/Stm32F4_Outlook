/*******************************************************************************
  * @file        main.c
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      TRX
  * @date        2022-4-25
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION
  * ------------------------
  * - 2022-4-25  TRX Created
*******************************************************************************/
#include "sys.h"
#include "ticktim.h"
#include "EventRecorder.h"
#include <stdio.h>
#include "bsp_usart_fifo.h"
#include <stdlib.h>
#include <string.h>
#include "bsp_key.h"
#include "bsp_spi_flash_demo.h"

void func1(void)
{
	printf("1.5så‘¨æœŸè°ƒç”¨\r\n");
}
void func2(void)
{
	printf("2.0sÖÜÆÚµ÷ÓÃ\r\n");
}
void func3(void)
{
	printf("4.0så•æ¬¡è°ƒç”¨\r\n");
}

/*******************************************************************************
  * @FunctionName: main
  * @Author:       trx
  * @DateTime:     2022å¹?æœ?5æ—¥T0:45:22 
  * @Purpose:      
  * @param:        void               
  * @return:
*******************************************************************************/
int main(void)
{	
		vu8 ucKeyCode;	
		HAL_Init();                    	 			/* ³õÊ¼»¯HAL¿â */   
		Stm32_Clock_Init(336,8,2,7);   				/* ÉèÖÃÊ±ÖÓ,168Mhz */
		bsp_InitTimer();											/* ³õÊ¼»¯µÎ´ğ¶¨Ê±Æ÷ */
		bsp_InitUart();												/* ³õÊ¼»¯´®¿Ú1ÍâÉè */
		bsp_InitKey();
		bsp_InitSPIBus();	/* é…ç½®SPIæ€»çº¿ */		
		bsp_InitSFlash();	/* åˆå§‹åŒ–SPI ä¸²è¡ŒFlash */
		EventRecorderInitialize(EventRecordAll, 1U);/* ³õÊ¼»¯ EventRecorder ²¢¿ªÆô */
//	bsp_StartAutoTimer(0,1500,func1);
//		bsp_StartAutoTimer(1,2000,func2);
//	bsp_StartOnceTimer(3,4000,func3);
		DemoSpiFlash();
}
/*******************************************************************************
  * @FunctionName: bsp_RunPer10ms
  * @Author:       trx
  * @DateTime:     2022å¹?æœ?5æ—¥T0:52:20 
  * @Purpose:      
  * @param:                           
*******************************************************************************/
void bsp_RunPer10ms()
{
	bsp_Key_Scan10ms();
}
/*******************************************************************************
  * @FunctionName: bsp_RunPer50ms
  * @Author:       trx
  * @DateTime:     2022å¹?æœ?5æ—¥T0:48:14 
  * @Purpose:      
  * @param:                           
*******************************************************************************/
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

