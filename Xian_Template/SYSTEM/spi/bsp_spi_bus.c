#include "bsp_spi_bus.h"


/* 
ʱ�ӣ����ţ�DMA���жϵȺ궨��
APB2 ����ʱ�����Ƶ��Ϊ84MHz
*/
#define SPIx					SPI1
#define SPIx_CLK_ENABLE()		__HAL_RCC_SPI1_CLK_ENABLE()

#define DMAx_CLK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()

#define SPIx_FORCE_RESET()		__HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()	__HAL_RCC_SPI1_RELEASE_RESET()
/* PB3 */
#define	SPIx_SCK_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define	SPIx_SCK_GPIO			GPIOB
#define	SPIx_SCK_PIN			GPIO_PIN_3
#define SPIx_SCK_AF				GPIO_AF5_SPI1

/* PB4 */
#define SPIx_MISO_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO			GPIOB
#define SPIx_MISO_PIN			GPIO_PIN_4
#define SPIx_MISO_AF			GPIO_AF5_SPI1

/* PB5 */
#define SPIx_MOSI_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO			GPIOB
#define SPIx_MOSI_PIN			GPIO_PIN_5
#define SPIx_MOSI_AF			GPIO_AF5_SPI1

/* DMA2 */
#define SPIx_TX_DMA_CHANNEL		DMA_CHANNEL_3
#define SPIx_TX_DMA_STREAM		DMA2_Stream5
#define SPIx_RX_DMA_CHANNEL		DMA_CHANNEL_3
#define SPIx_RX_DMA_STREAM		DMA2_Stream0

/* �ж� */
#define SPIx_IRQn				SPI1_IRQn
#define SPIx_IRQHandler			SPI1_IRQHandler
#define SPIx_DMA_TX_IRQn		DMA2_Stream5_IRQn
#define SPIx_DMA_RX_IRQn		DMA2_Stream0_IRQn
#define SPIx_DMA_TX_IRQHandler	DMA2_Stream5_IRQHandler
#define SPIx_DMA_RX_IRQHandler	DMA2_Stream0_IRQHandler

/* ʱ�ӡ����ź궨�� */
#define SPIx_INK					SPI2
#define	SPIx_INK_CLK_ENBALE()		__HAL_RCC_SPI2_CLK_ENABLE()

/* PC2 */
#define	SPIx_INK_MISO_CLK_ENBALE()	__HAL_RCC_GPIOC_CLK_ENABLE()
#define	SPIx_INK_MISO_GPIO			GPIOC
#define	SPIx_INK_MISO_PIN			GPIO_PIN_2
#define	SPIx_INK_MISO_AF			GPIO_AF5_SPI2

/* PC3 */
#define SPIx_INK_MOSI_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE()
#define	SPIx_INK_MOSI_GPIO			GPIOC
#define	SPIx_INK_MOSI_PIN			GPIO_PIN_3
#define	SPIx_INK_MOSI_AF			GPIO_AF5_SPI2

/* PB13 */
#define	SPIx_INK_SCK_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
#define	SPIx_INK_SCK_GPIO			GPIOB
#define	SPIx_INK_SCK_PIN			GPIO_PIN_13
#define	SPIx_INK_SCK_AF				GPIO_AF5_SPI2




enum{
	TRANSFER_WAIT = 0,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

static SPI_HandleTypeDef hspi = {0};
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
static uint32_t s_BaudRatePrescaler;
static uint32_t s_CLKPhase;
static uint32_t s_CLKPolarity;
uint32_t g_spiLen;
uint8_t g_spi_busy;		/* SPIæ״̬��0��ʾ��æ��1��ʾæ */
__IO uint32_t wTransferState = TRANSFER_WAIT;

static SPI_HandleTypeDef ink_spi = {0};

uint8_t g_spiTxBuf[SPI_BUFFER_SIZE] = {0};
uint8_t g_spiRxBuf[SPI_BUFFER_SIZE] = {0};

void bsp_InitSPI1Bus(void)
{
	g_spi_busy = 0;
	/*
		ʱ����λ��CPHA = 1���ڴ���ͬ��ʱ�ӵĵڶ���������(�������½�)���ݱ�����
		ʱ�Ӽ��ԣ�CPOL = 1���ڴ���ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	*/
	bsp_InitSPI1Param(SPI_BAUDRATEPRESCALER_328_125K,SPI_PHASE_2EDGE,SPI_POLARITY_HIGH);
}
/*
*	�� �� ��: bsp_InitSPIBusParam
*	����˵��: ����SPI���߲�����ʱ�ӷ�Ƶ��ʱ����λ��ʱ�Ӽ���
*	��    ��: _BaudRatePrescaler  SPI����ʱ�ӷ�Ƶ���ã�֧�ֵĲ������£�
*                                 SPI_BAUDRATEPRESCALER_2    2��Ƶ
*                                 SPI_BAUDRATEPRESCALER_4    4��Ƶ
*                                 SPI_BAUDRATEPRESCALER_8    8��Ƶ
*                                 SPI_BAUDRATEPRESCALER_16   16��Ƶ
*                                 SPI_BAUDRATEPRESCALER_32   32��Ƶ
*                                 SPI_BAUDRATEPRESCALER_64   64��Ƶ
*                                 SPI_BAUDRATEPRESCALER_128  128��Ƶ
*                                 SPI_BAUDRATEPRESCALER_256  256��Ƶ
*                                                        
*             _CLKPhase           ʱ����λ��֧�ֵĲ������£�
*                                 SPI_PHASE_1EDGE     SCK���ŵĵ�1�����ز�����ĵ�1������
*                                 SPI_PHASE_2EDGE     SCK���ŵĵ�2�����ز�����ĵ�1������
*                                 
*             _CLKPolarity        ʱ�Ӽ��ԣ�֧�ֵĲ������£�
*                                 SPI_POLARITY_LOW    SCK�����ڿ���״̬���ڵ͵�ƽ
*                                 SPI_POLARITY_HIGH   SCK�����ڿ���״̬���ڸߵ�ƽ
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��17��53��
*/
void bsp_InitSPI1Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity)
{
	/* ���ִ��Ч�ʣ�ֻ����SPIӲ�����������仯ʱ����ִ��HAL_Init */
	if(s_BaudRatePrescaler == _BaudRatePrescaler && s_CLKPhase == _CLKPhase && s_CLKPolarity == _CLKPolarity)
	{
		return ;
	}
	
	s_BaudRatePrescaler = _BaudRatePrescaler;
	s_CLKPhase = _CLKPhase;
	s_CLKPolarity = _CLKPolarity;
	
	/* ����SPI���� */
	hspi.Instance				= SPIx;						/* ָ��SPI */
	hspi.Init.BaudRatePrescaler = _BaudRatePrescaler;		/* ���ò����� */
	hspi.Init.Direction 		= SPI_DIRECTION_2LINES;		/* ȫ˫�� */
	hspi.Init.CLKPhase 			= _CLKPhase;				/* ����ʱ����λ */
	hspi.Init.CLKPolarity 		= _CLKPolarity;				/* ����ʱ�Ӽ��� */
	hspi.Init.DataSize			= SPI_DATASIZE_8BIT;		/* �������ݿ�� */
	hspi.Init.FirstBit 			= SPI_FIRSTBIT_MSB;			/* ���ݸ�λ��ǰ */
	hspi.Init.TIMode			= SPI_TIMODE_DISABLE;		/* ��ֹTIģʽ */
	hspi.Init.CRCCalculation	= SPI_CRCCALCULATION_DISABLE;/* ��ֹCRCУ�� */
	hspi.Init.CRCPolynomial		= 7;						/* ��ֹCRC�󣬴�Ϊ��Ч */
	hspi.Init.NSS				= SPI_NSS_SOFT;				/* ʹ�������ʽ����Ƭѡ���� */
	hspi.Init.Mode 				= SPI_MODE_MASTER;			/* SPI1����������ģʽ */
	
		
	/* ��λSPI */
	if(HAL_SPI_DeInit(&hspi) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	
	/* ��ʼ��SPI */
	if(HAL_SPI_Init(&hspi) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	
	__HAL_SPI_ENABLE(&hspi);
}

/*
*	�� �� ��: HAL_SPI_MspInit
*	����˵��: ��ʼ��SPI�������Լ�DMA��IT,�˺�����HAL_SPI_DeInit()�����ص�
*	��    ��: _hspi��SPI����ṹ��
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��11��
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
//	printf("���ų�ʼ��");
	/* ����SPI����GPIO:SCK MOSI MISO */
	/* ���⻹��Ƭѡ�ź� */
	if(_hspi->Instance == SPIx)
		{
			GPIO_InitTypeDef	gpio_initstruct;
			
			/* SPI1 �� GPIOʱ�� */
			SPIx_SCK_CLK_ENABLE();
			SPIx_MISO_CLK_ENABLE();
			SPIx_MOSI_CLK_ENABLE();
			SPIx_CLK_ENABLE();
			
			/* SPI SCK */
			gpio_initstruct.Pin 		= SPIx_SCK_PIN;
			gpio_initstruct.Mode		= GPIO_MODE_AF_PP;
			gpio_initstruct.Pull		= GPIO_PULLUP;
			gpio_initstruct.Speed		= GPIO_SPEED_FREQ_HIGH;
			gpio_initstruct.Alternate	= SPIx_SCK_AF;
			HAL_GPIO_Init(SPIx_SCK_GPIO,&gpio_initstruct);
			
			/* SPI MISO */
			gpio_initstruct.Pin			= SPIx_MISO_PIN;
			gpio_initstruct.Alternate	= SPIx_MISO_AF;
			HAL_GPIO_Init(SPIx_MISO_GPIO,&gpio_initstruct);
			
			/* SPI MOSI */
			gpio_initstruct.Pin			= SPIx_MOSI_PIN;
			gpio_initstruct.Alternate	= SPIx_MOSI_AF;
			HAL_GPIO_Init(SPIx_MOSI_GPIO,&gpio_initstruct);
		}
	else if(_hspi->Instance == SPIx_INK)
		{
			GPIO_InitTypeDef gpio_initstruct;

			/* SPI2 �� GPIOʱ�� */
			SPIx_INK_SCK_CLK_ENABLE();
			SPIx_INK_MISO_CLK_ENBALE();
			SPIx_INK_MOSI_CLK_ENABLE();
			SPIx_INK_CLK_ENBALE();

			/* SPI SCK */
			gpio_initstruct.Pin			= SPIx_INK_SCK_PIN;
			gpio_initstruct.Mode		= GPIO_MODE_AF_PP;
			gpio_initstruct.Pull		= GPIO_PULLUP;
			gpio_initstruct.Speed		= GPIO_SPEED_FAST;
			gpio_initstruct.Alternate	= SPIx_INK_SCK_AF;
			HAL_GPIO_Init(SPIx_INK_SCK_GPIO,&gpio_initstruct);

			/* SPI MISO */
			gpio_initstruct.Pin			= SPIx_INK_MISO_PIN;
			gpio_initstruct.Alternate	= SPIx_INK_MISO_AF;
			HAL_GPIO_Init(SPIx_INK_MISO_GPIO,&gpio_initstruct);
			
			/* SPI MOSI */
			gpio_initstruct.Pin			= SPIx_INK_MOSI_PIN;
			gpio_initstruct.Alternate	= SPIx_INK_MOSI_AF;
			HAL_GPIO_Init(SPIx_INK_MOSI_GPIO,&gpio_initstruct);
		}
	
	/* ����DMA��NVIC */
	#ifdef	USE_SPI_DMA
	{
		/* ʹ��DMAʱ�� */
		DMAx_CLK_ENABLE();
		
		/* DMA�����Դ洢��SPI FLASHΪ׼ */
		/* SPI DMA�������� */
		hdma_tx.Instance					= SPIx_TX_DMA_STREAM;		/* ��ʼ������������ */
		hdma_tx.Init.Channel				= SPIx_TX_DMA_CHANNEL;		/* DMAͨ������ */
		hdma_tx.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;		/* ��ֹFIFO */
		hdma_tx.Init.FIFOThreshold			= DMA_FIFO_THRESHOLD_FULL;	/* ��ֹFIFO��λ�������ã�����������ֵ */
		hdma_tx.Init.MemBurst				= DMA_MBURST_SINGLE;		/* ��ֹFIFO��λ�������ã����ڴ洢��ͻ�� */
		hdma_tx.Init.PeriphBurst			= DMA_PBURST_SINGLE;		/* ��ֹFIFO��λ�������ã���������ͻ�� */
		hdma_tx.Init.Direction				= DMA_MEMORY_TO_PERIPH;		/* ���䷽���ǴӴ洢�������� */
		hdma_tx.Init.PeriphInc				= DMA_PINC_DISABLE;			/* �����ַ������ֹ */
		hdma_tx.Init.MemInc					= DMA_MINC_ENABLE;			/* �洢����ַ����ʹ�� */
		hdma_tx.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;		/* �����ַ����λ��ѡ���ֽڣ���8bit */
		hdma_tx.Init.MemDataAlignment		= DMA_NORMAL;				/* ����ģʽ */
		hdma_tx.Init.Priority				= DMA_PRIORITY_LOW;			/* ���ȼ��� */
		
		/* ��λDMA */
		if(HAL_DMA_DeInit(&hdma_tx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* ��ʼ��DMA */
		if(HAL_DMA_Init(&hdma_tx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* ����DMA�����SPI */
		__HAL_LINKDMA(_hspi,hdmatx,hdma_tx);
		
		
		/* SPI DMA�������� */
		hdma_rx.Instance 				= SPIx_RX_DMA_STREAM;		/* ��ʼ������������ */
		hdma_rx.Init.Channel			= SPIx_RX_DMA_CHANNEL;		/* DMAͨ������ */
		hdma_rx.Init.FIFOMode			= DMA_FIFOMODE_DISABLE;		/* ��ֹFIFO */
		hdma_rx.Init.FIFOThreshold		= DMA_FIFO_THRESHOLD_FULL;	/* ��ֹFIFO��λ��Ч������������ֵ */
		hdma_rx.Init.MemBurst			= DMA_MBURST_SINGLE;		/* ��ֹFIFO��λ��Ч�����ڴ洢��ͻ�� */
		hdma_rx.Init.Direction			= DMA_PERIPH_TO_MEMORY;		/* ���䷽������赽�洢�� */
		hdma_rx.Init.PeriphInc			= DMA_PINC_DISABLE;			/* �����ַ������ֹ */
		hdma_rx.Init.MemInc				= DMA_MINC_ENABLE;			/* �洢����ַ����ʹ�� */
		hdma_rx.Init.PeriphDataAlignment= DMA_PDATAALIGN_BYTE;		/* �������ݴ���λ��ѡ���ֽڣ���8bit */
		hdma_rx.Init.MemDataAlignment	= DMA_MDATAALIGN_BYTE;		/* �洢�����ݴ���λ��ѡ���ֽڣ���8bit */
		hdma_rx.Init.Mode				= DMA_NORMAL;				/* ����ģʽ */
		hdma_rx.Init.Priority			= DMA_PRIORITY_HIGH;		/* ���ȼ��� */
		
		/* ��λDMA */
		if(HAL_DMA_DeInit(&hdma_rx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* ��ʼ��DMA */
		if(HAL_DMA_Init(&hdma_rx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* ����DMA�����SPI */
		__HAL_LINKDMA(_hspi,hdmarx,hdma_rx);
		
		/* ����DMA�����ж� */
		HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
		
		/* ����DMA�����ж� */
		HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
		
		/* ����SPI�ж� */
		HAL_NVIC_SetPriority(SPIx_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_IRQn);
	}
	
	#endif
	
	#ifdef	USE_SPI_INT
		{
			/* ����SPI�ж� */
			HAL_NVIC_SetPriority(SPIx_IRQn,1,0);
			HAL_NVIC_EnableIRQ(SPIx_IRQn);
		}
	#endif
}
/*
*	�� �� ��: bsp_spiTransfer
*	����˵��: spi���ݴ���
*	��    ��: none
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��11��
*/
void bsp_spi1Transfer(void)
{
	if(g_spiLen > SPI_BUFFER_SIZE)
	{
		return ;
	}
	
	/* DMA��ʽ���� */
#ifdef	USE_SPI_DMA
	wTransferState = TRANSFER_WAIT;
	if(HAL_SPI_TransmitReceive_DMA(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	while(wTransferState  == TRANSFER_WAIT)
	{
		;
	}
	
#endif
	
	/* �жϷ�ʽ���� */
#ifdef	USE_SPI_INT
	wTransferState = TRANSFER_WAIT;
	if(HAL_SPI_TransmitReceive_IT(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	while(wTransferState == TRANSFER_WAIT)
	{
		;
	}
#endif
	
	/* ��ѯ��ʽ���� */
#ifdef USE_SPI_POLL
		if(HAL_SPI_TransmitReceive(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen,1000) != HAL_OK)
			{
				printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
			}
#endif
}

/******** ���������ص�������HAL_SPI_TransmitReceive_DMA�����б��ص� *******/

/*
*	�� �� ��: HAL_SPI_TxRxCpltCallback
*	����˵��: SPI���ݴ�����ɻص�����
*	��    ��: hspi��SPI��ʼ�����ָ��
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��14��
*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_COMPLETE;
}

/*
*	�� �� ��: HAL_SPI_ErrorCallback
*	����˵��: SPI���ݴ������ص�����
*	��    ��: hspi��SPI��ʼ�����ָ��
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��14��
*/
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_ERROR;
}

/*
*	�� �� ��: bsp_SpiBusEnter
*	����˵��: ռ��SPI����
*	��    ��: none
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��27��
*/
void bsp_Spi1BusEnter(void)
{
	g_spi_busy = 1;
}

/*
*	�� �� ��: bsp_SpiBusExit
*	����˵��: �ͷű�ռ�õ�SPI����
*	��    ��: none
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��28��
*/
void bsp_Spi1BusExit(void)
{
	g_spi_busy = 0;
}
/*
*	�� �� ��: bsp_SpiBusBusy
*	����˵��: �ж�SPI�����Ƿ�æ�������Ǽ������SPIоƬ��Ƭѡ�ź��Ƿ�Ϊ1
*	��    ��: none
*	�� �� ֵ: 0:��ʾ��æ��1����ʾ��æ
*	ʱ�䣺2022��4��22��22��30��
*/
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}

/*
*	�� �� ��: SPIx_IRQHandler
*	����˵��: �жϷ������
*	��    ��: none
*	�� �� ֵ: none
*	ʱ�䣺2022��4��22��22��32��
*/
#ifdef USE_SPI_INT
void SPIx_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi);
}
#endif

#ifdef USE_SPI_DMA
void SPIx_DMA_RX_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hspi.hdmarx);
}
void SPIx_DMA_TX_IRQHandler(void)
{
	HAL_DMA_IRQHandler(hspi.hdmatx);
}
void SPIx_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&hspi);
}

#endif

/*******************************************************************************
  * @FunctionName: bsp_InitSPI2Bus
  * @Author:       trx
  * @DateTime:     2022��5��10�� 18:01:58 
  * @Purpose:      ��ʼ��SPI2����Ϊīˮ����ͨѶ
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InitSPI2Bus(void)
{
	/*
		ʱ����λ��CPHA = 0���ڴ���ͬ��ʱ�ӵĵڶ���������(�������½�)���ݱ�����
		ʱ�Ӽ��ԣ�CPOL = 0���ڴ���ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
	*/
	bsp_InitSPI2Param(SPI_APB1_BAUDRATEPRESCALER_164_0625K,SPI_PHASE_1EDGE,SPI_POLARITY_LOW);
}

/*******************************************************************************
  * @FunctionName: bsp_InitSPI2Param
  * @Author:       trx
  * @DateTime:     2022��5��10�� 18:03:31 
  * @Purpose:      spi2ͨѶ���ߵ���������
  * @param:        _BaudRatePrescaler����Ƶ
  * @param:        _CLKPhase         ��ʱ�Ӽ���
  * @param:        _CLKPolarity      ��ʱ����λ
  * @return:       none
*******************************************************************************/
void bsp_InitSPI2Param(uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity)
{
	/* ����SPI���� */
	ink_spi.Instance					= SPIx_INK;						/* ָ��SPI */
	ink_spi.Init.BaudRatePrescaler		= _BaudRatePrescaler;			/* �����ٶ� */
	ink_spi.Init.Direction				= SPI_DIRECTION_2LINES;			/* ȫ˫�� */
	ink_spi.Init.CLKPhase				= _CLKPhase;					/* ����ʱ����λ */
	ink_spi.Init.CLKPolarity			= _CLKPolarity;					/* ����ʱ�Ӽ��� */
	ink_spi.Init.DataSize				= SPI_DATASIZE_8BIT;			/* �������ݿ�� */
	ink_spi.Init.FirstBit				= SPI_FIRSTBIT_MSB;				/* �������ݸ�λ��ǰ */
	ink_spi.Init.TIMode					= SPI_TIMODE_DISABLE;			/* ����TIģʽ */
	ink_spi.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLE;	/* ��ֹCRCУ�� */
	ink_spi.Init.CRCPolynomial			= 10;							/* ��ֹCRC�󣬴�������Ч */
	ink_spi.Init.NSS					= SPI_NSS_SOFT;					/* ����ʹ�������ʽ����Ƭѡ���� */
	ink_spi.Init.Mode					= SPI_MODE_MASTER;				/* ����spi����������ģʽ */

	/* ��λSPI */
	if (HAL_SPI_DeInit(&ink_spi) != HAL_OK)
		{
			printf("Wrong parameters value:file %s on line %d\r\n", __FILE__,__LINE__);
		}

	/* ��ʼ��SPI */
	if (HAL_SPI_Init(&ink_spi) != HAL_OK)
		{
			printf("Wrong parameters value:file %s on line %d\r\n", __FILE__,__LINE__);
			
		}

	__HAL_SPI_ENABLE(&ink_spi);
}


/*******************************************************************************
  * @FunctionName: bsp_spi2Transfer
  * @Author:       trx
  * @DateTime:     2022��5��10�� 18:02:31 
  * @Purpose:      spi2���ݷ��ͣ�īˮ��ֻ�������ݣ�����������
  * @param:        _value��׼�����͵��ֽ�����
  * @return:       none
*******************************************************************************/
void bsp_spi2Transfer(uint8_t _value)
{
	if (HAL_SPI_Transmit(&ink_spi,&_value,1,1000) != HAL_OK)
		{
			printf("Wrong parameters valude: file %s on line %d\r\n",__FILE__,__LINE__);
		}
	//printf("%d\r\n",HAL_SPI_Transmit(&ink_spi,&_value,1,1000));
	//printf("%d\r\n",_value);
}



