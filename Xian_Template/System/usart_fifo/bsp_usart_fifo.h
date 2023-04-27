#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "sys.h"
/*
	使用内部debug(因为使用的是狂暴模式)打印时，会增加32k的sram空间，所以调试的时候可以使用，release版本要去掉此组件
	去除组件：DEBUG_SWITCH_EN 宏改为0，接着在Manage Run-Time Environment->Compiler->Event Recorder&&STDOUT勾选取消
				接着在Options for target->Target改为如下配置:
						  Start			  Size
			√	IRAM1---0x20000000		 0x20000
				IRAM2---0x10000000		 0x10000
	添加组件：DEBUG_SWITCH_EN 宏改为1，接着在Manage Run-Time Environment->Compiler->Event Recorder&&STDOUT(功能选为EVR)勾选使能，
				接着在Options for target->Target改为如下配置:
						  Start			  Size
			√	IRAM1---0x20000000		 0x10000
				IRAM2---0x20010000		 0x10000	√
				接着右键Project串口->Compiler右键->Event Recorder->Memory->IRAM2 [0x20010000-0x2001FFFF]
*/
#define DEBUG_SWITCH_EN 	0//1使用内部DEBUG打印，0使用串口外设进行打印
#define UART1_FIFO_EN 		1
#define UART2_FIFO_EN		1
#define UART3_FIFO_EN		1
/* 定义端口号 */
typedef enum
{
	COM1 = 0,
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
}COM_PORT_E;

/* 定义串口波特率和FIFO缓冲区的大小，分为发送缓冲区和接收缓冲区，支持全双工 */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD 			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif
#if UART2_FIFO_EN == 1
	#define UART2_BAUD			115200
	#define UART2_TX_BUF_SIZE	1*1024
	#define UART2_RX_BUF_SIZE	1*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			115200
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif
/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;	/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;		/* 发送缓冲区 */
	uint8_t *pRxBuf;		/* 接收缓冲区 */
	uint16_t usTxBufSize;	/* 发送缓冲区大小 */
	uint16_t usRxBufSize;	/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;	/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;/* 等待发送的数据个数 */
	
	__IO uint16_t usRxWrite;/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;	/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;/* 还未读取的新数据个数 */
	
	void (*SendBefor)(void);/* 开始发送之前的回调指针函数(主要用于RS485切换到发送模式) */
	void (*SendOver)(void);	/* 发送完毕的回调函数(主要用于RS485将发送模式切换为接收模式) */
	void (*ReciveNew)(uint8_t _byte);	/* 串口收到数据的回调函数指针 */
	uint8_t Sending;		/* 正在发送中 */
}UART_T;

void bsp_InitUart(void);
void bsp_SetUartParam(USART_TypeDef *Instance,uint32_t BaudRate,uint32_t Parity,uint32_t Mode);
void comSendBuf(COM_PORT_E _ucPort,uint8_t *_ucaBuf,uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort,uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort,uint8_t *_pByte);

#endif

