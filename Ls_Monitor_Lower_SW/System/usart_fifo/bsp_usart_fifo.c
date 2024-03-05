#include "bsp_usart_fifo.h"



/* ����1��GPIO PA9 PA10 �ӿ� */
#define USART1_CLK_ENABLE()			    __HAL_RCC_USART1_CLK_ENABLE()

#define USART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_TX_GPIO_PORT			    GPIOA
#define USART1_TX_PIN				    GPIO_PIN_9
#define USART1_TX_AF				    GPIO_AF7_USART1

#define USART1_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART1_RX_GPIO_PORT			     GPIOA
#define USART1_RX_PIN				     GPIO_PIN_10
#define USART1_RX_AF				     GPIO_AF7_USART1
/* ����2��GPIO --- PA2 PA3  GPS (ֻ��RX??TX����̫��ռ��??*/
#define USART2_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()

#define USART2_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_TX_GPIO_PORT              GPIOA
#define USART2_TX_PIN                    GPIO_PIN_2
#define USART2_TX_AF                     GPIO_AF7_USART2

#define USART2_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USART2_RX_GPIO_PORT              GPIOA
#define USART2_RX_PIN                    GPIO_PIN_3
#define USART2_RX_AF                     GPIO_AF7_USART2

/* ����3��GPIO --- PB10 PB11  RS485 */
#define USART3_CLK_ENABLE()              __HAL_RCC_USART3_CLK_ENABLE()

#define USART3_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_TX_GPIO_PORT              GPIOB
#define USART3_TX_PIN                    GPIO_PIN_10
#define USART3_TX_AF                     GPIO_AF7_USART3

#define USART3_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define USART3_RX_GPIO_PORT              GPIOB
#define USART3_RX_PIN                    GPIO_PIN_11
#define USART3_RX_AF                     GPIO_AF7_USART3
/* ����ÿ�����ڽṹ���??*/
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuff1[UART1_TX_BUF_SIZE];	/* ���ͻ����� */
	static uint8_t g_RxBuff1[UART1_RX_BUF_SIZE];	/* ���ջ���??*/
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ���??*/
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ���??*/
#endif

static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart,uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);

/*
*	?????? bsp_InitUart
*	����˵��: �������е��õĴ��ڳ�ʼ������
*	??   ?? none
*	?????? none
*	ʱ��??022????5??1??7??
*/
void bsp_InitUart()
{
	UartVarInit();
	InitHardUart();
}

/*
*	?????? UartVarInit
*	����˵��: ������صı�����ʼ��
*	??   ?? ??
*	?????? ??
*	ʱ��??022????4??2??9??
*/
static void UartVarInit(void)
{
#if	UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32�����豸 */
	g_tUart1.pTxBuf = g_TxBuff1;				/* ���ڷ��ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuff1;				/* ���ڽ��ջ�����ָ??*/
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������??*/
	g_tUart1.usTxWrite = 0;						/* ����FIFOд��??*/
	g_tUart1.usTxRead = 0;						/* ����FIFO����??*/
	g_tUart1.usRxWrite = 0;						/* ����FIFOд��??*/
	g_tUart1.usRxRead = 0;						/* ����FIFO����??*/
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ�??*/
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص���??*/
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص���??*/
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart1.Sending  = 0;						/* ���ڷ����б�־ */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 �����豸 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ??*/
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������??*/
	g_tUart2.usTxWrite = 0;						/* ����FIFOд��??*/
	g_tUart2.usTxRead = 0;						/* ����FIFO����??*/
	g_tUart2.usRxWrite = 0;						/* ����FIFOд��??*/
	g_tUart2.usRxRead = 0;						/* ����FIFO����??*/
	g_tUart2.usRxCount = 0;						/* ���յ��������ݸ�??*/
	g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص���??*/
	g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص���??*/
	g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart2.Sending = 0;						/* ���ڷ����б�־ */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 �����豸 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* ���ͻ�����ָ�� */
	g_tUart3.pRxBuf = g_RxBuf3;					/* ���ջ�����ָ??*/
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* ���ջ�������??*/
	g_tUart3.usTxWrite = 0;						/* ����FIFOд��??*/
	g_tUart3.usTxRead = 0;						/* ����FIFO����??*/
	g_tUart3.usRxWrite = 0;						/* ����FIFOд��??*/
	g_tUart3.usRxRead = 0;						/* ����FIFO����??*/
	g_tUart3.usRxCount = 0;						/* ���յ��������ݸ�??*/
	g_tUart3.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart3.SendBefor = 0;						/* ��������ǰ�Ļص���??*/
	g_tUart3.SendOver = 0;						/* ������Ϻ�Ļص���??*/
	g_tUart3.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
	g_tUart3.Sending = 0;						/* ���ڷ����б�־ */
#endif
}

/*
*	?????? InitHardUart
*	����˵��: ��������ĳ�ʼ��
*	??   ?? none
*	?????? none
*	ʱ��??022????5??1??8??
*/
void InitHardUart(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
#if UART1_FIFO_EN == 1	/* ����1 */
	/* ʹ��GPIO TX/RXʱ�� */
	USART1_TX_GPIO_CLK_ENABLE();
	USART1_RX_GPIO_CLK_ENABLE();
	
	/* ʹ��USARTxʱ�� */
	USART1_CLK_ENABLE();
	
	/* ����TX���� */
	GPIO_InitStruct.Pin			= USART1_TX_PIN;
	GPIO_InitStruct.Mode		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull		= GPIO_PULLUP;
	GPIO_InitStruct.Speed		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= USART1_TX_AF;
	HAL_GPIO_Init(USART1_TX_GPIO_PORT,&GPIO_InitStruct);
	
	/* ����RX���� */
	GPIO_InitStruct.Pin 		= USART1_RX_PIN;
	GPIO_InitStruct.Alternate 	= USART1_RX_AF;
	HAL_GPIO_Init(USART1_RX_GPIO_PORT,&GPIO_InitStruct);
	
	/* ����NVIC the NVIC for UART */
	HAL_NVIC_SetPriority(USART1_IRQn,0,1);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	
	/* ���ò����ʡ���żУ��λ */
	bsp_SetUartParam(USART1,UART1_BAUD,UART_PARITY_NONE,UART_MODE_TX_RX);
	
	/* ���TC������ɱ�??*/
	CLEAR_BIT(USART1->SR,USART_SR_TC);
	/* ���RXEN���ձ�־ */
	CLEAR_BIT(USART1->SR,USART_SR_RXNE);
	/* ʹ��PE��RX�����ж� */
	SET_BIT(USART1->CR1,USART_CR1_RXNEIE);
#endif

#if UART2_FIFO_EN == 1		/* ����2 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART2_TX_GPIO_CLK_ENABLE();
	USART2_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART2_CLK_ENABLE();	

	/* ����TX���� */
	GPIO_InitStruct.Pin       = USART2_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART2_TX_AF;
	HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* ����RX���� */
	GPIO_InitStruct.Pin = USART2_RX_PIN;
	GPIO_InitStruct.Alternate = USART2_RX_AF;
	HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

	/* ����NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
  
	/* ���ò����ʡ���żУ??*/
	bsp_SetUartParam(USART2,  UART2_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);	// UART_MODE_TX_RX

	CLEAR_BIT(USART2->SR, USART_SR_TC);   /* ���TC������ɱ�??*/
    CLEAR_BIT(USART2->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
	SET_BIT(USART2->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
#endif

#if UART3_FIFO_EN == 1			/* ����3 */
	/* ʹ�� GPIO TX/RX ʱ�� */
	USART3_TX_GPIO_CLK_ENABLE();
	USART3_RX_GPIO_CLK_ENABLE();
	
	/* ʹ�� USARTx ʱ�� */
	USART3_CLK_ENABLE();	

	/* ����TX���� */
	GPIO_InitStruct.Pin       = USART3_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USART3_TX_AF;
	HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);	
	
	/* ����RX���� */
	GPIO_InitStruct.Pin = USART3_RX_PIN;
	GPIO_InitStruct.Alternate = USART3_RX_AF;
	HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

	/* ����NVIC the NVIC for UART */   
	HAL_NVIC_SetPriority(USART3_IRQn, 0, 3);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
  
	/* ���ò����ʡ���żУ??*/
	bsp_SetUartParam(USART3,  UART3_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

	CLEAR_BIT(USART3->SR, USART_SR_TC);   /* ���TC������ɱ�??*/
    CLEAR_BIT(USART3->SR, USART_SR_RXNE); /* ���RXNE���ձ�־ */
	SET_BIT(USART3->CR1, USART_CR1_RXNEIE);	/* ʹ��PE. RX�����ж� */
#endif
}
/*
*********************************************************************************************************
*	?????? ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	??   ?? _ucPort: �˿�??COM1 - COM8)
*	?????? uartָ��
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
*	����˵��: ���ô��ڵ�Ӳ����??
*	??   ?? Instance	USART_TypeDef���ͽṹ??
				BaudRate	����??
				Partity		У�����ͣ���У�����żУ��
				Mode		���ͺͽ���ģʽʹ��
*	?????? ??
*	ʱ��??022????4??3??3??
*/
void bsp_SetUartParam(USART_TypeDef *Instance,uint32_t BaudRate,uint32_t Parity,uint32_t Mode)
{
	UART_HandleTypeDef UartHandle;
	
/*	����1Ӳ�����ò���
	�첽����ģʽ
	-�ֳ�	= 8??
	-ֹͣ??= 1��ֹͣλ
	-У��	= ����Parity
	-����??= ����BaudRate
	-Ӳ�������ƹ�??RTS and CTS signals) */
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
*	����˵��: ���ڷ��ͽ����жϵĴ�����
*	??   ?? _pUart:���������Զ���ṹ���ȫ�ֱ���
*	?????? none
*	ʱ��??022????5??1??0??
*/
static void UartIRQ(UART_T *_pUart)
{
	
	/* ��ȡ��Ӧ�Ĵ��������� */
	uint32_t isrflags	= READ_REG(_pUart->uart->SR);
	uint32_t cr1its		= READ_REG(_pUart->uart->CR1);
	uint32_t cr3its		= READ_REG(_pUart->uart->CR3);
	
	/* ��������ж� */
	if((isrflags & USART_SR_RXNE) != RESET)/* ���� λ�� �ж��Ƿ�׼���ö�ȡ���յ�����??*/
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;
		/* ���ݼĴ���DRֻ��??λ��??*/
		ch = READ_REG(_pUart->uart->DR);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		/* �������ݶ�ָ�����buf����??�����������??*/
		if(++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		/* ÿ����һ�����ݣ�FIFO?? */
		if(_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}
		/* �ص�������֪ͨӦ�ó�����յ������ݣ�һ���Ƿ���һ����Ϣ���߱�־��1 */
		if(_pUart->ReciveNew)
		{
			_pUart->ReciveNew(ch);
		}
	}
	
	/* �����ͻ���������??*/
	//���ݴ��䵽��λ�Ĵ���						�����ж�ʹ??
	if(((isrflags & USART_SR_TXE) != RESET) && (cr1its & USART_CR1_TXEIE) != RESET)
	{
		if(_pUart->usTxCount == 0)
		{
			/* ���ͻ������������Ѿ�ȡ��ʱ����ֹ���ͻ������ж�(��ʱ���һ�����ݻ�δ����������?? */
			CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
			
			/* ʹ�����ݷ��������??*/
			SET_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
		}else
		{
			/* ���ڷ��ʹ�����??*/
			_pUart->Sending = 1;
			
			/* �ӷ���FIFO��ȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			_pUart->uart->DR = _pUart->pTxBuf[_pUart->usTxRead];
			/* ���ͻ�������ָ����ڻ�������С */
			if(++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
	/* ����λȫ��������ϵ��ж� */
	if(((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
	{
		if(_pUart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ��������??*/
			CLEAR_BIT(_pUart->uart->CR1,USART_CR1_TCIE);
			/* �ص�������һ����������RS485ͨ�� */
			if(_pUart->SendOver)
			{
				_pUart->SendOver();
			}
			_pUart->Sending = 0;
		}else
		{
			/* ��������£��������˷�??*/
			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFO??������д�뷢�����ݼĴ��� */
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
*	����˵��: HAL����жδ�����
*	??   ?? none
*	?????? none
*	ʱ��??022????5??1??1??
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
*	����˵��: ʹ�ܴ��ڷ�����??
*	??   ?? _pUart�����������Զ���ṹ��ָ�룻_ucaBuf��Ҫ���͵�buff��_usLen�����͵�buff����
*	?????? none
*	ʱ��??022????5??1??8??
*/
static void UartSend(UART_T *_pUart,uint8_t *_ucaBuf,uint16_t _usLen)
{
	uint16_t i;
	
	for(i = 0;i < _usLen;i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
		while(1)
		{
			__IO uint16_t usCount;
			
			__set_PRIMASK(1);	/* ��ֹȫ���ж� */
			usCount = _pUart->usTxCount;
			__set_PRIMASK(0);	/* ��ȫ���ж� */
			if(usCount < _pUart->usTxBufSize)
			{
				break;	
			}else if(usCount == _pUart->usTxBufSize)/* �����Ѿ���������??*/
			{
				/* �����ж�ʹ??*/
				if((_pUart->uart->CR1 & USART_CR1_TXEIE) == 0)
				{
					SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
				}
			}
		}
		
		/* ���������뷢�ͻ����� */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];
		
		__set_PRIMASK(1);	/* ��ֹȫ���ж� */
		if(++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		__set_PRIMASK(0);	/* ��ȫ���ж� */
	}
	/* ʹ�ܷ�����??��������) */
	SET_BIT(_pUart->uart->CR1,USART_CR1_TXEIE);
}

/*
*	?????? comSenBuf
*	����˵��: ���ô��ڷ����ַ���
*	??   ?? _ucPort��׼�����͵Ĵ������裻_ucaBuf��Ҫ���͵�����buff��_usLen��Ҫ���͵�buff����
*	?????? none
*	ʱ��??022????5??1??0??
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
		pUart->SendBefor();/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģ??*/
	}
	UartSend(pUart,_ucaBuf,_usLen);
}


/*
*	?????? comSendChar
*	����˵��: ���ڷ���һ����??
*	??   ?? _ucPort:��������ö�ٱ�����_ucByte�����͵��ֽ�����
*	?????? none
*	ʱ��??022????5??1??6??
*/
void comSendChar(COM_PORT_E _ucPort,uint8_t _ucByte)
{
	comSendBuf(_ucPort,&_ucByte,1);
}

/*
*	?????? UartGetChar
*	����˵��: �Ӵ�������Ľ��ջ������ж�ȡһ����??
*	??   ?? _pUart�����������Զ���ṹ�壻_pByte����Ŷ�ȡ���ݵ�ָ��
*	?????? 0����ʾ�����ݶ�ȡ??����ʾ��ȡ������
*	ʱ��??022????6??4??9??
*/
static uint8_t UartGetChar(UART_T *_pUart,uint8_t * _pByte)
{
	uint16_t usCount;
	
	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ�����??*/
	__set_PRIMASK(1);	/* ��ֹȫ���ж� */
	usCount = _pUart->usRxCount;
	__set_PRIMASK(0);	/* ����ȫ���ж� */

	
	/* �������д������ͬ����?? */
	if(usCount == 0)
	{
		return 0;
	}
	else
	{
		/* �Ӵ��ڽ���FIFO??����??*/
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];
		
		/* ��дFIFO����??*/
		__set_PRIMASK(1);	/* ��ֹȫ���ж� */
		if(++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		__set_PRIMASK(0);	/* ����ȫ���ж� */
		return 1;
	}
}
/*
*	?????? comGetChar
*	����˵��: �Ӵ��ڽ��ջ������ж�ȡһ����??
*	??   ?? _usPort:��������ŵ�ö�ٱ�����_pByte��������ݵ�ָ��
*	?????? 0����ʾ������??����ʾ��ȡ����Ч����
*	ʱ��??022????6??5??5??
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
*	����˵��: �ض���C��׼���printf����
*	??   ?? ...
*	?????? ����д����ַ����������򷵻�һ����??
*	ʱ��??022????5??1??1??
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
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ�??��������
*	??   ?? none
*	?????? none
*	ʱ��??022????6??5??9??
*/
#if DEBUG_SWITCH_EN == 0
int fgetc(FILE *f)
{
#if 1
	/* �Ӵ�??����FIFO��ȡ1�����ݣ�ֻ��ȡ�����ݲŷ�??*/
	uint8_t ucData;
	while(comGetChar(COM1,&ucData) == 0);
	
	return ucData;
#else
	while((USART1->SR & USART_SR_RXNE) == 0){}
	return (int)USART1->DR;
#endif
}
#endif



