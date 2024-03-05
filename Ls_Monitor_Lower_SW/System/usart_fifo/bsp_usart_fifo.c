#include "bsp_usart_fifo.h"



/* 串口1的GPIO PA9 PA10 接口 */
#define USART1_CLK_ENABLE()			    __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT			    GPIOA
#define USART1_TX_PIN				    GPIO_PIN_9
#define USART1_TX_AF				    GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT			     GPIOA
#define USART1_RX_PIN				     GPIO_PIN_10
#define USART1_RX_AF				     GPIO_AF7_USART1
/* 串口2的GPIO --- PA2 PA3  GPS (只用RX??TX被以太网占用??*/
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()

#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_AF                     GPIO_AF7_USART2

#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_AF                     GPIO_AF7_USART2

/* 串口3的GPIO --- PB10 PB11  RS485 */
#define USART3_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()

#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_PORT              GPIOB
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_TX_AF                     GPIO_AF7_USART3

#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_RX_GPIO_PORT              GPIOB
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_RX_AF                     GPIO_AF7_USART3
/* 定义每个串口结构体变??*/
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuff1[UART1_TX_BUF_SIZE];	/* 发送缓冲区 */
	static uint8_t g_RxBuff1[UART1_RX_BUF_SIZE];	/* 接收缓冲??*/
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* 接收缓冲??*/
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* 发送缓冲区 */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* 接收缓冲??*/
#endif

static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart,uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);

/*
*	?????? bsp_InitUart
*	功能说明: 主函数中调用的串口初始化函数
*	??   ?? none
*	?????? none
*	时间??022????5??1??7??
*/
void bsp_InitUart()
{
	UartVarInit();
	InitHardUart();
}

/*
*	?????? UartVarInit
*	功能说明: 串口相关的变量初始化
*	??   ?? ??
*	?????? ??
*	时间??022????4??2??9??
*/
static void UartVarInit(void)
{
#if	UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32串口设备 */
	g_tUart1.pTxBuf = g_TxBuff1;				/* 串口发送缓冲区指针 */
	g_tUart1.pRxBuf = g_RxBuff1;				/* 串口接收缓冲区指??*/
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* 接收缓冲区大??*/
	g_tUart1.usTxWrite = 0;						/* 发送FIFO写索??*/
	g_tUart1.usTxRead = 0;						/* 发送FIFO读索??*/
	g_tUart1.usRxWrite = 0;						/* 接收FIFO写索??*/
	g_tUart1.usRxRead = 0;						/* 接收FIFO读索??*/
	g_tUart1.usRxCount = 0;						/* 接收到的新数据个??*/
	g_tUart1.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart1.SendBefor = 0;						/* 发送数据前的回调函??*/
	g_tUart1.SendOver = 0;						/* 发送完毕后的回调函??*/
	g_tUart1.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart1.Sending  = 0;						/* 正在发送中标志 */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 串口设备 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* 发送缓冲区指针 */
	g_tUart2.pRxBuf = g_RxBuf2;					/* 接收缓冲区指??*/
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* 接收缓冲区大??*/
	g_tUart2.usTxWrite = 0;						/* 发送FIFO写索??*/
	g_tUart2.usTxRead = 0;						/* 发送FIFO读索??*/
	g_tUart2.usRxWrite = 0;						/* 接收FIFO写索??*/
	g_tUart2.usRxRead = 0;						/* 接收FIFO读索??*/
	g_tUart2.usRxCount = 0;						/* 接收到的新数据个??*/
	g_tUart2.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart2.SendBefor = 0;						/* 发送数据前的回调函??*/
	g_tUart2.SendOver = 0;						/* 发送完毕后的回调函??*/
	g_tUart2.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart2.Sending = 0;						/* 正在发送中标志 */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 串口设备 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* 发送缓冲区指针 */
	g_tUart3.pRxBuf = g_RxBuf3;					/* 接收缓冲区指??*/
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* 发送缓冲区大小 */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* 接收缓冲区大??*/
	g_tUart3.usTxWrite = 0;						/* 发送FIFO写索??*/
	g_tUart3.usTxRead = 0;						/* 发送FIFO读索??*/
	g_tUart3.usRxWrite = 0;						/* 接收FIFO写索??*/
	g_tUart3.usRxRead = 0;						/* 接收FIFO读索??*/
	g_tUart3.usRxCount = 0;						/* 接收到的新数据个??*/
	g_tUart3.usTxCount = 0;						/* 待发送的数据个数 */
	g_tUart3.SendBefor = 0;						/* 发送数据前的回调函??*/
	g_tUart3.SendOver = 0;						/* 发送完毕后的回调函??*/
	g_tUart3.ReciveNew = 0;						/* 接收到新数据后的回调函数 */
	g_tUart3.Sending = 0;						/* 正在发送中标志 */
#endif
}

/*
*	?????? InitHardUart
*	功能说明: 串口外设的初始化
*	??   ?? none
*	?????? none
*	时间??022????5??1??8??
*/
void InitHardUart(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
#if UART1_FIFO_EN == 1	/* 串口1 */
	/* 使能GPIO TX/RX时钟 */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* 使能USARTx时钟 */
	USART1_CLK_ENABLE();
	
	/* 配置TX引脚 */
	GPIO_InitStruct.Pin			= USART1_TX_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_PULLUP;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= USART1_TX_AF;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT,&GPIO_InitStruct);
	
	/* 配置RX引脚 */
	GPIO_InitStruct.Pin 		= USART1_RX_PIN;
	GPIO_InitStruct.Alternate 	= USART1_RX_AF;
	HAL_GPIO_Init(USART1_RX_GPIO_PORT,&GPIO_InitStruct);
	
	/* 配置NVIC the NVIC for UART */
	HAL_NVIC_SetPriority(USART1_IRQn,0,1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	
	/* 配置波特率、奇偶校验位 */
	bsp_SetUartParam(USART1,UART1_BAUD,UART_PARITY_NONE,UART_MODE_TX_RX);
	
	/* 清除TC发送完成标??*/
	CLEAR_BIT(USART1->SR,USART_SR_TC);
	/* 清除RXEN接收标志 */
	CLEAR_BIT(USART1->SR,USART_SR_RXNE);
	/* 使能PE，RX接收中断 */
	SET_BIT(USART1->CR1,USART_CR1_RXNEIE);
#endif

#if UART2_FIFO_EN == 1		/* 串口2 */
	/* 使能 GPIO TX/RX 时钟 */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART2_CLK_ENABLE();	

	/* 配置TX引脚 */
	GPIO_InitStruct.Pin       = USART2_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART2_TX_AF;
	HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* 配置RX引脚 */
	GPIO_InitStruct.Pin = USART2_RX_PIN;
	GPIO_InitStruct.Alternate = USART2_RX_AF;
	HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

	/* 配置NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
  
	/* 配置波特率、奇偶校??*/
	bsp_SetUartParam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);	// UART_MODE_TX_RX

	CLEAR_BIT(USART2->SR, USART_SR_TC);   /* 清除TC发送完成标??*/
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
#endif

#if UART3_FIFO_EN == 1			/* 串口3 */
	/* 使能 GPIO TX/RX 时钟 */
	USART3_TX_GPIO_CLK_ENABLE();
	USART3_RX_GPIO_CLK_ENABLE();
	
	/* 使能 USARTx 时钟 */
	USART3_CLK_ENABLE();	

	/* 配置TX引脚 */
	GPIO_InitStruct.Pin       = USART3_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART3_TX_AF;
	HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* 配置RX引脚 */
	GPIO_InitStruct.Pin = USART3_RX_PIN;
	GPIO_InitStruct.Alternate = USART3_RX_AF;
	HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

	/* 配置NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
  
	/* 配置波特率、奇偶校??*/
	bsp_SetUartParam(USART3,  UART3_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART3->SR, USART_SR_TC);   /* 清除TC发送完成标??*/
    CLEAR_BIT(USART3->SR, USART_SR_RXNE); /* 清除RXNE接收标志 */
	SET_BIT(USART3->CR1, USART_CR1_RXNEIE);	/* 使能PE. RX接受中断 */
#endif
}
/*
*********************************************************************************************************
*	?????? ComToUart
*	功能说明: 将COM端口号转换为UART指针
*	??   ?? _ucPort: 端口??COM1 - COM8)
*	?????? uart指针
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		return 0;
	}
}

/*
*	?????? bsp_SetUartParam
*	功能说明: 配置串口的硬件参??
*	??   ?? Instance	USART_TypeDef类型结构??
				BaudRate	波特??
				Partity		校验类型，奇校验或者偶校验
				Mode		发送和接收模式使能
*	?????? ??
*	时间??022????4??3??3??
*/
void bsp_SetUartParam(USART_TypeDef *Instance,uint32_t BaudRate,uint32_t Parity,uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;
	
/*	串口1硬件配置参数
	异步串口模式
	-字长	= 8??
	-停止??= 1个停止位
	-校验	= 参数Parity
	-波特??= 参数BaudRate
	-硬件流控制关??RTS and CTS signals) */
	UartHandle.Instance			= Instance;
	UartHandle.Init.BaudRate	= BaudRate;
	UartHandle.Init.WordLength	= UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits	= UART_STOPBITS_1;
	UartHandle.Init.Parity		= Parity;
	UartHandle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	UartHandle.Init.Mode 		= Mode;
	UartHandle.Init.OverSampling= UART_OVERSAMPLING_16;
	
	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__); 
	}
	
}

/*
*	?????? UartIRQ
*	功能说明: 串口发送接收中断的处理函数
*	??   ?? _pUart:串口外设自定义结构体的全局变量
*	?????? none
*	时间??022????5??1??0??
*/
static void UartIRQ(UART_T *_pUart)
{
	
	/* 获取相应寄存器的数据 */
	uint32_t isrflags	= READ_REG(_pUart->uart->SR);
	uint32_t cr1its		= READ_REG(_pUart->uart->CR1);
	uint32_t cr3its		= READ_REG(_pUart->uart->CR3);
	
	/* 处理接收中断 */
	if((isrflags & USART_SR_RXNE) != RESET)/* 进行 位与 判断是否准备好读取接收到的数??*/
	{
		/* 从串口接收数据寄存器读取数据存放到接收FIFO */
		uint8_t ch;
		/* 数据寄存器DR只有??位有??*/
		ch = READ_REG(_pUart->uart->DR);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		/* 接收数据读指针大于buf，则??，否则计数自??*/
		if(++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		/* 每接收一次数据，FIFO?? */
		if(_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}
		/* 回调函数，通知应用程序接收到新数据，一般是发送一个消息或者标志置1 */
		if(_pUart->ReciveNew)
		{
			_pUart->ReciveNew(ch);
		}
	}
	
	/* 处理发送缓冲区空中??*/
	//数据传输到移位寄存器						发送中断使??
	if(((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		if(_pUart->usTxCount == 0)
		{
			/* 发送缓冲区的数据已经取完时，禁止发送缓冲区中断(此时最后一个数据还未真正发送完?? */
			CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
			
			/* 使能数据发送完毕中??*/
			SET_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
		}else
		{
			/* 正在发送串口数??*/
			_pUart->Sending = 1;
			
			/* 从发送FIFO中取1个字节写入串口发送数据寄存器 */
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			/* 发送缓冲区读指针大于缓冲区大小 */
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
	/* 数据位全部发送完毕的中断 */
	if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		if(_pUart->usTxCount == 0)
		{
			/* 如果发送FIFO的数据全部发送完毕，禁止数据发送完毕中??*/
			CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
			/* 回调函数，一般用来处理RS485通信 */
			if(_pUart->SendOver)
			{
				_pUart->SendOver();
			}
			_pUart->Sending = 0;
		}else
		{
			/* 正常情况下，不会进入此分??*/
			/* 如果发送FIFO的数据还未完毕，则从发送FIFO??个数据写入发送数据寄存器 */
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

/*
*	?????? USART1_IRQHandler
*	功能说明: HAL库的中段处理函数
*	??   ?? none
*	?????? none
*	时间??022????5??1??1??
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#endif
#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
	UartIRQ(&g_tUart2);
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
	UartIRQ(&g_tUart3);
}
#endif
/*
*	?????? UartSend
*	功能说明: 使能串口发送中??
*	??   ?? _pUart：串口外设自定义结构体指针；_ucaBuf：要发送的buff；_usLen：发送的buff长度
*	?????? none
*	时间??022????5??1??8??
*/
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen)
{
	uint16_t i;
	
	for(i = 0;i < _usLen;i++)
	{
		/* 如果发送缓冲区已经满了，则等待缓冲区空 */
		while(1)
		{
			__IO uint16_t usCount;
			
			__set_PRIMASK(1);	/* 禁止全局中断 */
			usCount = _pUart->usTxCount;
			__set_PRIMASK(0);	/* 打开全局中断 */
			if(usCount < _pUart->usTxBufSize)
			{
				break;	
			}else if(usCount == _pUart->usTxBufSize)/* 数据已经填满缓冲??*/
			{
				/* 发送中断使??*/
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
				}
			}
		}
		
		/* 将数据填入发送缓冲区 */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];
		
		__set_PRIMASK(1);	/* 禁止全局中断 */
		if(++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		__set_PRIMASK(0);	/* 打开全局中断 */
	}
	/* 使能发送中??缓冲区空) */
	SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
}

/*
*	?????? comSenBuf
*	功能说明: 配置串口发送字符串
*	??   ?? _ucPort：准备发送的串口外设；_ucaBuf：要发送的数据buff；_usLen：要发送的buff长度
*	?????? none
*	时间??022????5??1??0??
*/
void comSendBuf(COM_PORT_E _ucPort,uint8_t *_ucaBuf,uint16_t _usLen)
{
	UART_T *pUart;
	
	pUart = ComToUart(_ucPort);
	if(pUart == 0)
	{
		return;
	}
	if(pUart->SendBefor != 0)
	{
		pUart->SendBefor();/* 如果是RS485通信，可以在这个函数中将RS485设置为发送模??*/
	}
	UartSend(pUart,_ucaBuf,_usLen);
}


/*
*	?????? comSendChar
*	功能说明: 串口发送一个字??
*	??   ?? _ucPort:串口外设枚举变量；_ucByte：发送的字节数据
*	?????? none
*	时间??022????5??1??6??
*/
void comSendChar(COM_PORT_E _ucPort,uint8_t _ucByte)
{
	comSendBuf(_ucPort,&_ucByte,1);
}

/*
*	?????? UartGetChar
*	功能说明: 从串口外设的接收缓冲区中读取一个字??
*	??   ?? _pUart：串口外设自定义结构体；_pByte：存放读取数据的指针
*	?????? 0：表示无数据读取??：表示读取到数据
*	时间??022????6??4??9??
*/
static uint8_t UartGetChar(UART_T *_pUart,uint8_t * _pByte)
{
	uint16_t usCount;
	
	/* usRxWrite 变量在中断函数中被改写，主程序读取该变量时，必须进行临界区保??*/
	__set_PRIMASK(1);	/* 禁止全局中断 */
	usCount = _pUart->usRxCount;
	__set_PRIMASK(0);	/* 开启全局中断 */

	
	/* 如果读和写索引相同，则返?? */
	if(usCount == 0)
	{
		return 0;
	}
	else
	{
		/* 从串口接收FIFO??个数??*/
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];
		
		/* 改写FIFO读索??*/
		__set_PRIMASK(1);	/* 禁止全局中断 */
		if(++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		__set_PRIMASK(0);	/* 开启全局中断 */
		return 1;
	}
}
/*
*	?????? comGetChar
*	功能说明: 从串口接收缓冲区中读取一个数??
*	??   ?? _usPort:串口外设号的枚举变量；_pByte：存放数据的指针
*	?????? 0：表示无数据??：表示读取到有效数据
*	时间??022????6??5??5??
*/
uint8_t comGetChar(COM_PORT_E _usPort,uint8_t *_pByte)
{
	UART_T *pUart;
	pUart = ComToUart(_usPort);
	if(pUart == 0)
	{
		return 0;
	}
	return UartGetChar(pUart,_pByte);
}
/*
*	?????? fputc
*	功能说明: 重定向C标准库的printf函数
*	??   ?? ...
*	?????? 返回写入的字符总数，否则返回一个负??
*	时间??022????5??1??1??
*/
#if DEBUG_SWITCH_EN == 0
int fputc(int ch,FILE *f)
{
#if 1
	comSendChar(COM1, ch);
	return ch;
#else
	USART1->DR = ch;
	
	while((USART1->SR & USART_SR_TC) == 0){}
		return ch;
#endif
}
#endif


/*
*	?????? fgetc
*	功能说明: 重定义getc函数，这样可以使用getchar函数从串??输入数据
*	??   ?? none
*	?????? none
*	时间??022????6??5??9??
*/
#if DEBUG_SWITCH_EN == 0
int fgetc(FILE *f)
{
#if 1
	/* 从串??接收FIFO中取1个数据，只有取到数据才返??*/
	uint8_t ucData;
	while(comGetChar(COM1,&ucData) == 0);
	
	return ucData;
#else
	while((USART1->SR & USART_SR_RXNE) == 0){}
	return (int)USART1->DR;
#endif
}
#endif



