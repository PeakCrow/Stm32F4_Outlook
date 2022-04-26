#include "bsp_spi_bus.h"


//#define USR_SPI_DMA		/* DMA方式 */
//#define USE_SPI_INT		/* 中断方式 */
#define USR_SPI_POLL	/* 查询方式 */


/* 
时钟，引脚，DMA，中断等宏定义
APB2 高速时钟最大频率为84MHz
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
#define SPIx_TX_DMA_STREAM		DMA2_Stream3
#define SPIx_RX_DMA_CHANNEL		DMA_CHANNEL_3
#define SPIx_RX_DMA_STREAM		DMA2_Stream0

/* 中断 */
#define SPIx_IRQn				SPI1_IRQn
#define SPIx_IRQHandler			SPI1_IRQHandler
#define SPIx_DMA_TX_IRQn		DMA2_Stream3_IRQn
#define SPIx_DMA_RX_IRQn		DMA2_Stream0_IRQn
#define SPIx_DMA_TX_IRQHandler	DMA2_Stream3_IRQHandler
#define SPIx_DMA_RX_IRQHandler	DMA2_Stream0_IRQHandler

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
uint8_t g_spi_busy;		/* SPI忙状态，0表示不忙，1表示忙 */
__IO uint32_t wTransferState = TRANSFER_WAIT;

uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];

void bsp_InitSPIBus(void)
{
	g_spi_busy = 0;
	bsp_InitSPIParam(SPI_BAUDRATEPRESCALER_256,SPI_PHASE_2EDGE,SPI_POLARITY_HIGH);
}
/*
*	函 数 名: bsp_InitSPIBusParam
*	功能说明: 配置SPI总线参数，时钟分频，时钟相位和时钟极性
*	形    参: _BaudRatePrescaler  SPI总线时钟分频设置，支持的参数如下：
*                                 SPI_BAUDRATEPRESCALER_2    2分频
*                                 SPI_BAUDRATEPRESCALER_4    4分频
*                                 SPI_BAUDRATEPRESCALER_8    8分频
*                                 SPI_BAUDRATEPRESCALER_16   16分频
*                                 SPI_BAUDRATEPRESCALER_32   32分频
*                                 SPI_BAUDRATEPRESCALER_64   64分频
*                                 SPI_BAUDRATEPRESCALER_128  128分频
*                                 SPI_BAUDRATEPRESCALER_256  256分频
*                                                        
*             _CLKPhase           时钟相位，支持的参数如下：
*                                 SPI_PHASE_1EDGE     SCK引脚的第1个边沿捕获传输的第1个数据
*                                 SPI_PHASE_2EDGE     SCK引脚的第2个边沿捕获传输的第1个数据
*                                 
*             _CLKPolarity        时钟极性，支持的参数如下：
*                                 SPI_POLARITY_LOW    SCK引脚在空闲状态处于低电平
*                                 SPI_POLARITY_HIGH   SCK引脚在空闲状态处于高电平
*	返 回 值: none
*	时间：2022年4月22日17点53分
*/
void bsp_InitSPIParam(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity)
{
	/* 提高执行效率，只有在SPI硬件参数发生变化时，才执行HAL_Init */
	if(s_BaudRatePrescaler == _BaudRatePrescaler && s_CLKPhase == _CLKPhase && s_CLKPolarity == _CLKPolarity)
	{
		return ;
	}
	
	s_BaudRatePrescaler = _BaudRatePrescaler;
	s_CLKPhase = _CLKPhase;
	s_CLKPolarity = _CLKPolarity;
	
	/* 设置SPI参数 */
	hspi.Instance				= SPIx;						/* 指定SPI */
	hspi.Init.BaudRatePrescaler = _BaudRatePrescaler;		/* 设置波特率 */
	hspi.Init.Direction 		= SPI_DIRECTION_2LINES;		/* 全双工 */
	hspi.Init.CLKPhase 			= _CLKPhase;				/* 配置时钟相位 */
	hspi.Init.CLKPolarity 		= _CLKPolarity;				/* 配置时钟极性 */
	hspi.Init.DataSize			= SPI_DATASIZE_8BIT;		/* 设置数据宽度 */
	hspi.Init.FirstBit 			= SPI_FIRSTBIT_MSB;			/* 数据高位在前 */
	hspi.Init.TIMode			= SPI_TIMODE_DISABLE;		/* 禁止TI模式 */
	hspi.Init.CRCCalculation	= SPI_CRCCALCULATION_DISABLE;/* 禁止CRC校验 */
	hspi.Init.CRCPolynomial		= 7;						/* 禁止CRC后，此为无效 */
	hspi.Init.NSS				= SPI_NSS_SOFT;				/* 使用软件方式管理片选引脚 */
	hspi.Init.Mode 				= SPI_MODE_MASTER;			/* SPI1工作在主控模式 */
	
	/* 复位SPI */
	if(HAL_SPI_DeInit(&hspi) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	
	/* 初始化SPI */
	if(HAL_SPI_Init(&hspi) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
}

/*
*	函 数 名: HAL_SPI_MspInit
*	功能说明: 初始化SPI的引脚以及DMA和IT
*	形    参: _hspi：SPI句柄结构体
*	返 回 值: none
*	时间：2022年4月22日22点11分
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
//	printf("引脚初始化");
	/* 配置SPI总线GPIO:SCK MOSI MISO */
	/* 此外还有片选信号 */
	{
		GPIO_InitTypeDef	gpio_initstruct;
		
		/* SPI1 和 GPIO时钟 */
		SPIx_SCK_CLK_ENABLE();
		SPIx_MISO_CLK_ENABLE();
		SPIx_MOSI_CLK_ENABLE();
		SPIx_CLK_ENABLE();
		
		/* SPI SCK */
		gpio_initstruct.Pin 		= SPIx_SCK_PIN;
		gpio_initstruct.Mode		= GPIO_MODE_AF_PP;
		gpio_initstruct.Pull		= GPIO_NOPULL;
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
	
	/* 配置DMA和NVIC */
	#ifdef	USE_SPI_DMA
	{
		/* 使能DMA时钟 */
		DMAx_CLK_ENABLE();
		
		/* DMA方向以存储器SPI FLASH为准 */
		/* SPI DMA发送配置 */
		hdma_tx.Instance					= SPIx_TX_DMA_STREAM;		/* 初始化发送数据流 */
		hdma_tx.Init.Channel				= SPIx_TX_DMA_CHANNEL;		/* DMA通道配置 */
		hdma_tx.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;		/* 禁止FIFO */
		hdma_tx.Init.FIFOThreshold			= DMA_FIFO_THRESHOLD_FULL;	/* 禁止FIFO此位不起作用，用于设置阈值 */
		hdma_tx.Init.MemBurst				= DMA_MBURST_SINGLE;		/* 禁止FIFO此位不起作用，用于存储器突发 */
		hdma_tx.Init.PeriphBurst			= DMA_PBURST_SINGLE;		/* 禁止FIFO此位不起作用，用于外设突发 */
		hdma_tx.Init.Direction				= DMA_MEMORY_TO_PERIPH;		/* 传输方向是从存储器到外设 */
		hdma_tx.Init.PeriphInc				= DMA_PINC_DISABLE;			/* 外设地址自增禁止 */
		hdma_tx.Init.MenInc					= DMA_MINC_ENABLE;			/* 存储器地址自增使能 */
		hdma_tx.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;		/* 外设地址传输位宽选择字节，即8bit */
		hdma_tx.Init.MemDataAlignment		= DMA_NORMAL;				/* 正常模式 */
		hdma_tx.Init.Priority				= DMA_PRIORITY_LOW;			/* 优先级低 */
		
		/* 复位DMA */
		if(HAL_DMA_DeInit(&hdma_tx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* 初始化DMA */
		if(HAL_DMA_Init(&hdma_tx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* 关联DMA句柄到SPI */
		__HAL_LINKDMA(_hspi,hdmatx,hdma_tx);
		
		
		/* SPI DMA接收配置 */
		hdma_rx.Instance 				= SPIx_RX_DMA_STREAM;		/* 初始化接收数据流 */
		hdma_rx.Init.Channel			= SPIx_RX_DMA_CHANNEL;		/* DMA通道配置 */
		hdma_rx.Init.FIFOMode			= DMA_FIFOMODE_DISABLE;		/* 禁止FIFO */
		hdma_rx.Init.FIFOThreshold		= DMA_FIFO_THRESHOLD_FULL;	/* 禁止FIFO此位无效，用于设置阈值 */
		hdma_rx.Init.MemBurst			= DMA_MBURST_SINGLE;		/* 禁止FIFO此位无效，用于存储器突发 */
		hdma_rx.Init.Direction			= DMA_PERIPH_TO_MEMORY;		/* 传输方向从外设到存储器 */
		hdma_rx.Init.PeriphInc			= DMA_PINC_DISABLE;			/* 外设地址自增禁止 */
		hdma_rx.Init.MenInc				= DMA_MINC_ENABLE;			/* 存储器地址自增使能 */
		hdma_rx.Init.PeriphDataAlignment= DMA_PDATAALIGN_BYTE;		/* 外设数据传输位宽选择字节，即8bit */
		hdma_rx.Init.MemDataAlignment	= DMA_MDATAALIGN_BYTE;		/* 存储器数据传输位宽选择字节，即8bit */
		hdma_rx.Init.Mode				= DMA_NORMAL;				/* 正常模式 */
		hdma_rx.Init.Priority			= DMA_PRIORITY_HIGH;		/* 优先级低 */
		
		/* 复位DMA */
		if(HAL_DMA_DeInit(&hdma_rx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* 初始化DMA */
		if(HAL_DMA_Init(&hdma_rx) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
		
		/* 关联DMA句柄到SPI */
		__HAL_LINKDMA(_hspi,hdmarx,hdma_rx);
		
		/* 配置DMA发送中断 */
		HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
		
		/* 配置DMA接收中断 */
		HAL_NVIC_SetPriority(SPIx_DMA_RX_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_DMA_RX_IRQn);
		
		/* 配置SPI中断 */
		HAL_NVIC_SetPriority(SPIx_IRQn,1,0);
		HAL_NVIC_EnableIRQ(SPIx_IRQn);
	}
	
	#endif
	
	#ifdef	USE_SPI_INT
		{
			/* 配置SPI中断 */
			HAL_NVIC_SetPriority(SPIx_IRQn,1,0);
			HAL_NVIC_EnableIRQ(SPIx_IRQn);
		}
	#endif
}
/*
*	函 数 名: bsp_spiTransfer
*	功能说明: spi数据传输
*	形    参: none
*	返 回 值: none
*	时间：2022年4月22日22点11分
*/
void bsp_spiTransfer(void)
{
	if(g_spiLen > SPI_BUFFER_SIZE)
	{
		return ;
	}
	
	/* DMA方式传输 */
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
	
	/* 中断方式传输 */
#ifdef	USE_SPI_INT
	wTransferState = TRANSFER_WAIT;
	if(HAL_SPI_TransmitReceive_IT(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	while(wTransferState = TRANSFER_WAIT)
	{
		;
	}
#endif
	
	/* 查询方式传输 */
#ifdef USE_SPI_POLL
		if(HAL_SPI_TransmitReceive(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
		{
			printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
		}
	
#endif
}

/******** 下面两个回调函数是在HAL_SPI_TransmitReceive_DMA函数中被调用的 *******/

/*
*	函 数 名: HAL_SPI_TxRxCpltCallback
*	功能说明: SPI数据传输完成回调函数
*	形    参: hspi：SPI初始化句柄指针
*	返 回 值: none
*	时间：2022年4月22日22点14分
*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_COMPLETE;
}

/*
*	函 数 名: HAL_SPI_ErrorCallback
*	功能说明: SPI数据传输错误回调函数
*	形    参: hspi：SPI初始化句柄指针
*	返 回 值: none
*	时间：2022年4月22日22点14分
*/
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_ERROR;
}

/*
*	函 数 名: bsp_SpiBusEnter
*	功能说明: 占用SPI总线
*	形    参: none
*	返 回 值: none
*	时间：2022年4月22日22点27分
*/
void bsp_SpiBusEnter(void)
{
	g_spi_busy = 1;
}

/*
*	函 数 名: bsp_SpiBusExit
*	功能说明: 释放被占用的SPI总线
*	形    参: none
*	返 回 值: none
*	时间：2022年4月22日22点28分
*/
void bsp_SpiBusExit(void)
{
	g_spi_busy = 0;
}
/*
*	函 数 名: bsp_SpiBusBusy
*	功能说明: 判断SPI总线是否忙，方法是检测其他SPI芯片的片选信号是否为1
*	形    参: none
*	返 回 值: 0:表示不忙；1：表示在忙
*	时间：2022年4月22日22点30分
*/
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}

/*
*	函 数 名: SPIx_IRQHandler
*	功能说明: 中断服务程序
*	形    参: none
*	返 回 值: none
*	时间：2022年4月22日22点32分
*/
#ifdef USE_SPI_INT
void SPIx_IRQHandler(void)
{
	HAL_SPI_IRQHandler(*hspi);
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
