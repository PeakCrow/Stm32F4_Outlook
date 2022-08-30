#include "bsp_spi_bus.h"


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
#define SPIx_TX_DMA_STREAM		DMA2_Stream5
#define SPIx_RX_DMA_CHANNEL		DMA_CHANNEL_3
#define SPIx_RX_DMA_STREAM		DMA2_Stream0

/* 中断 */
#define SPIx_IRQn				SPI1_IRQn
#define SPIx_IRQHandler			SPI1_IRQHandler
#define SPIx_DMA_TX_IRQn		DMA2_Stream5_IRQn
#define SPIx_DMA_RX_IRQn		DMA2_Stream0_IRQn
#define SPIx_DMA_TX_IRQHandler	DMA2_Stream5_IRQHandler
#define SPIx_DMA_RX_IRQHandler	DMA2_Stream0_IRQHandler

/* 时钟、引脚宏定义 */
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
uint8_t g_spi_busy;		/* SPI忙状态，0表示不忙，1表示忙 */
__IO uint32_t wTransferState = TRANSFER_WAIT;

static SPI_HandleTypeDef ink_spi = {0};

uint8_t g_spiTxBuf[SPI_BUFFER_SIZE] = {0};
uint8_t g_spiRxBuf[SPI_BUFFER_SIZE] = {0};

void bsp_InitSPI1Bus(void)
{
	g_spi_busy = 0;
	/*
		时钟相位：CPHA = 1，在串行同步时钟的第二个跳变沿(上升或下降)数据被采样
		时钟极性：CPOL = 1，在串行同步时钟的空闲状态为高电平
	*/
	bsp_InitSPI1Param(SPI_BAUDRATEPRESCALER_328_125K,SPI_PHASE_2EDGE,SPI_POLARITY_HIGH);
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
void bsp_InitSPI1Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity)
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
	
	__HAL_SPI_ENABLE(&hspi);
}

/*
*	函 数 名: HAL_SPI_MspInit
*	功能说明: 初始化SPI的引脚以及DMA和IT,此函数由HAL_SPI_DeInit()函数回调
*	形    参: _hspi：SPI句柄结构体
*	返 回 值: none
*	时间：2022年4月22日22点11分
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
//	printf("引脚初始化");
	/* 配置SPI总线GPIO:SCK MOSI MISO */
	/* 此外还有片选信号 */
	if(_hspi->Instance == SPIx)
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

			/* SPI2 和 GPIO时钟 */
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
		hdma_tx.Init.MemInc					= DMA_MINC_ENABLE;			/* 存储器地址自增使能 */
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
		hdma_rx.Init.MemInc				= DMA_MINC_ENABLE;			/* 存储器地址自增使能 */
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
void bsp_spi1Transfer(void)
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
	while(wTransferState == TRANSFER_WAIT)
	{
		;
	}
#endif
	
	/* 查询方式传输 */
#ifdef USE_SPI_POLL
		if(HAL_SPI_TransmitReceive(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen,1000) != HAL_OK)
			{
				printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
			}
#endif
}

/******** 下面两个回调函数在HAL_SPI_TransmitReceive_DMA函数中被回调 *******/

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
void bsp_Spi1BusEnter(void)
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
void bsp_Spi1BusExit(void)
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
  * @DateTime:     2022年5月10日 18:01:58 
  * @Purpose:      初始化SPI2，作为墨水屏的通讯
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InitSPI2Bus(void)
{
	/*
		时钟相位：CPHA = 0，在串行同步时钟的第二个跳变沿(上升或下降)数据被采样
		时钟极性：CPOL = 0，在串行同步时钟的空闲状态为低电平
	*/
	bsp_InitSPI2Param(SPI_APB1_BAUDRATEPRESCALER_164_0625K,SPI_PHASE_1EDGE,SPI_POLARITY_LOW);
}

/*******************************************************************************
  * @FunctionName: bsp_InitSPI2Param
  * @Author:       trx
  * @DateTime:     2022年5月10日 18:03:31 
  * @Purpose:      spi2通讯总线的外设配置
  * @param:        _BaudRatePrescaler：分频
  * @param:        _CLKPhase         ：时钟极性
  * @param:        _CLKPolarity      ：时钟相位
  * @return:       none
*******************************************************************************/
void bsp_InitSPI2Param(uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity)
{
	/* 设置SPI参数 */
	ink_spi.Instance					= SPIx_INK;						/* 指定SPI */
	ink_spi.Init.BaudRatePrescaler		= _BaudRatePrescaler;			/* 配置速度 */
	ink_spi.Init.Direction				= SPI_DIRECTION_2LINES;			/* 全双工 */
	ink_spi.Init.CLKPhase				= _CLKPhase;					/* 配置时钟相位 */
	ink_spi.Init.CLKPolarity			= _CLKPolarity;					/* 配置时钟极性 */
	ink_spi.Init.DataSize				= SPI_DATASIZE_8BIT;			/* 配置数据宽度 */
	ink_spi.Init.FirstBit				= SPI_FIRSTBIT_MSB;				/* 配置数据高位在前 */
	ink_spi.Init.TIMode					= SPI_TIMODE_DISABLE;			/* 精致TI模式 */
	ink_spi.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLE;	/* 禁止CRC校验 */
	ink_spi.Init.CRCPolynomial			= 10;							/* 禁止CRC后，此配置无效 */
	ink_spi.Init.NSS					= SPI_NSS_SOFT;					/* 配置使用软件方式管理片选引脚 */
	ink_spi.Init.Mode					= SPI_MODE_MASTER;				/* 配置spi工作在主控模式 */

	/* 复位SPI */
	if (HAL_SPI_DeInit(&ink_spi) != HAL_OK)
		{
			printf("Wrong parameters value:file %s on line %d\r\n", __FILE__,__LINE__);
		}

	/* 初始化SPI */
	if (HAL_SPI_Init(&ink_spi) != HAL_OK)
		{
			printf("Wrong parameters value:file %s on line %d\r\n", __FILE__,__LINE__);
			
		}

	__HAL_SPI_ENABLE(&ink_spi);
}


/*******************************************************************************
  * @FunctionName: bsp_spi2Transfer
  * @Author:       trx
  * @DateTime:     2022年5月10日 18:02:31 
  * @Purpose:      spi2数据发送，墨水屏只接收数据，不反馈数据
  * @param:        _value；准备发送的字节数据
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



