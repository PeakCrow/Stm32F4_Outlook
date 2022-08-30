#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f4xx.h"
#include "stdio.h"
/*
	ʹ���ڲ�debug(��Ϊʹ�õ��ǿ�ģʽ)��ӡʱ��������32k��sram�ռ䣬���Ե��Ե�ʱ�����ʹ�ã�release�汾Ҫȥ�������
	ȥ�������DEBUG_SWITCH_EN ���Ϊ0��������Manage Run-Time Environment->Compiler->Event Recorder&&STDOUT��ѡȡ��
				������Options for target->Target��Ϊ��������:
						  Start			  Size
			��	IRAM1---0x20000000		 0x20000
				IRAM2---0x10000000		 0x10000
	��������DEBUG_SWITCH_EN ���Ϊ1��������Manage Run-Time Environment->Compiler->Event Recorder&&STDOUT(����ѡΪEVR)��ѡʹ�ܣ�
				������Options for target->Target��Ϊ��������:
						  Start			  Size
			��	IRAM1---0x20000000		 0x10000
				IRAM2---0x20010000		 0x10000	��
				�����Ҽ�Project����->Compiler�Ҽ�->Event Recorder->Memory->IRAM2 [0x20010000-0x2001FFFF]
*/
#define DEBUG_SWITCH_EN 0		//1ʹ���ڲ�DEBUG��ӡ��0ʹ�ô���������д�ӡ
#define UART1_FIFO_EN 	1
#define UART2_FIFO_EN	0
#define UART3_FIFO_EN	0
/* ����˿ں� */
typedef enum
{
	COM1 = 0,
	COM2 = 1,	/* USART2 */
	COM3 = 2,	/* USART3 */
}COM_PORT_E;

/* ���崮�ڲ����ʺ�FIFO�������Ĵ�С����Ϊ���ͻ������ͽ��ջ�������֧��ȫ˫�� */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD 			115200
	#define UART1_TX_BUF_SIZE	1*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif
#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	10
	#define UART2_RX_BUF_SIZE	2*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif
/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;	/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;		/* ���ͻ����� */
	uint8_t *pRxBuf;		/* ���ջ����� */
	uint16_t usTxBufSize;	/* ���ͻ�������С */
	uint16_t usRxBufSize;	/* ���ջ�������С */
	__IO uint16_t usTxWrite;/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;	/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;/* �ȴ����͵����ݸ��� */
	
	__IO uint16_t usRxWrite;/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;	/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;/* ��δ��ȡ�������ݸ��� */
	
	void (*SendBefor)(void);/* ��ʼ����֮ǰ�Ļص�ָ�뺯��(��Ҫ����RS485�л�������ģʽ) */
	void (*SendOver)(void);	/* ������ϵĻص�����(��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ) */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
	uint8_t Sending;		/* ���ڷ����� */
}UART_T;

void bsp_InitUart(void);
void bsp_SetUartParam(USART_TypeDef *Instance,uint32_t BaudRate,uint32_t Parity,uint32_t Mode);
void comSendBuf(COM_PORT_E _ucPort,uint8_t *_ucaBuf,uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort,uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort,uint8_t *_pByte);

#endif

