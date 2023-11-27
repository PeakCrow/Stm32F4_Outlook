#include "bsp_spi_bus.h"


static SPI_HandleTypeDef hspi = {0};
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
static uint32_t s_BaudRatePrescaler;
static uint32_t s_CLKPhase;
static uint32_t s_CLKPolarity;
uint32_t g_spiLen;
uint8_t g_spi_busy;		/* SPI的繁忙状态，0表示不忙，1表示忙 */
__IO uint32_t wTransferState = TRANSFER_WAIT;

static SPI_HandleTypeDef ink_spi = {0};

uint8_t g_spiTxBuf[SPI_BUFFER_SIZE] = {0};
uint8_t g_spiRxBuf[SPI_BUFFER_SIZE] = {0};

void bsp_InitSPI1Bus(void)
{
    g_spi_busy = 0;
    /*
        spi速度为328.125k
        配置同步时钟的空闲状态为高电平,CPOL时钟极性 1
        配置同步时钟的第二个跳变沿(上升或下降)数据被采样,CPHA时钟相位 1
    */
    bsp_InitSPI1Param(SPI_BAUDRATEPRESCALER_328_125K,SPI_PHASE_2EDGE,SPI_POLARITY_HIGH);
}
/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIParam
*	功能说明: 配置SPI总线参数，时钟分频，时钟相位和时钟极性。
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
*
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitSPI1Param(uint32_t _BaudRatePrescaler,uint32_t _CLKPhase,uint32_t _CLKPolarity)
{
	/* 提高执行效率，只有在spi硬件参数发生变化时，才执行后续操作 */
	if(s_BaudRatePrescaler == _BaudRatePrescaler && s_CLKPhase == _CLKPhase && s_CLKPolarity == _CLKPolarity)
	{
		return ;
	}
	
	s_BaudRatePrescaler = _BaudRatePrescaler;
	s_CLKPhase = _CLKPhase;
	s_CLKPolarity = _CLKPolarity;
	

	hspi.Instance				= SPIx;					        /* 实例化SPI */
	hspi.Init.BaudRatePrescaler = _BaudRatePrescaler;		    /* 配置分频系数 */
	hspi.Init.Direction 		= SPI_DIRECTION_2LINES;         /* 配置全双工模式 */
	hspi.Init.CLKPhase 			= _CLKPhase;			        /* 配置时钟相位 */
	hspi.Init.CLKPolarity 		= _CLKPolarity;				    /* 配置时钟极性 */
	hspi.Init.DataSize			= SPI_DATASIZE_8BIT;		    /* 配置数据宽度 */
	hspi.Init.FirstBit 			= SPI_FIRSTBIT_MSB;			    /* 数据传输先传高位 */
	hspi.Init.TIMode			= SPI_TIMODE_DISABLE;		    /* 禁止TI模式 */
	hspi.Init.CRCCalculation	= SPI_CRCCALCULATION_DISABLE;   /* 禁止CRC运算 */
	hspi.Init.CRCPolynomial		= 7;						    /* 配置CRC多项式，当然此时无效 */
	hspi.Init.NSS				= SPI_NSS_SOFT;				    /* 使用软件方式管理片选引脚 */
	hspi.Init.Mode 				= SPI_MODE_MASTER;			    /* 配置master模式 */
	
		
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
*********************************************************************************************************
*	函 数 名: bsp_InitSPIParam
*	功能说明: 配置SPI总线时钟，GPIO，中断，DMA等
*	形    参: SPI_HandleTypeDef 类型指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
	/* 配置SPI总线GPIO:SCK MOSI MISO */
	/* 区分外部flash与inkscreen */
	if(_hspi->Instance == SPIx)
		{
			GPIO_InitTypeDef	gpio_initstruct;
			
			/* SPI1 引脚时钟打开 */
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

			/* SPI2 引脚时钟打开 */
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
	
	/* 配置DMA与NVIC */
	#ifdef	USE_SPI_DMA
	{
		/* 使能DMA时钟 */
		DMAx_CLK_ENABLE();
		
		/* SPI DMA 发送配置 */
		hdma_tx.Instance					= SPIx_TX_DMA_STREAM;	    /* 示例化DMA使用的数据流 */
		hdma_tx.Init.Channel				= SPIx_TX_DMA_CHANNEL;	    /* 配置DMA使用的通道 */
		hdma_tx.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;	    /* 禁止FIFO模式 */
		hdma_tx.Init.FIFOThreshold			= DMA_FIFO_THRESHOLD_FULL;  /* 禁止FIFO模式此位不起作用，用于配置阈值 */
		hdma_tx.Init.MemBurst				= DMA_MBURST_SINGLE;	    /* 禁止FIFO模式此位不起作用，用于配置存储器突发 */
		hdma_tx.Init.PeriphBurst			= DMA_PBURST_SINGLE;	    /* 禁止FIFO模式此位不起作用，用于外设突发 */
		hdma_tx.Init.Direction				= DMA_MEMORY_TO_PERIPH;	    /* 配置传输方向是从存储器到外设 */
		hdma_tx.Init.PeriphInc				= DMA_PINC_DISABLE;		    /* 配置外设地址禁止自增 */
		hdma_tx.Init.MemInc					= DMA_MINC_ENABLE;		    /* 配置存储器地址自增 */
		hdma_tx.Init.PeriphDataAlignment	= DMA_PDATAALIGN_BYTE;	    /* 配置外设数据传输位宽选择字节，8bit */
		hdma_tx.Init.MemDataAlignment		= DMA_MDATAALIGN_BYTE;	    /* 配置存储器数据传输位宽选择字节，8bit */
		hdma_tx.Init.Mode                   = DMA_NORMAL;               /* 配置正常模式 */
		hdma_tx.Init.Priority				= DMA_PRIORITY_LOW;		    /* 配置优先级低 */
		
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
		
		
		/* SPI DMA 接收配置 */
		hdma_rx.Instance 				= SPIx_RX_DMA_STREAM;	    /* 实例化使用的DMA数据流 */
		hdma_rx.Init.Channel			= SPIx_RX_DMA_CHANNEL;	    /* 配置DMA使用的通道 */
		hdma_rx.Init.FIFOMode			= DMA_FIFOMODE_DISABLE;	    /* 禁止FIFO模式 */
		hdma_rx.Init.FIFOThreshold		= DMA_FIFO_THRESHOLD_FULL;  /* 禁止FIFO模式此位不起作用，用于配置阈值 */
		hdma_rx.Init.MemBurst			= DMA_MBURST_SINGLE;	    /* 禁止FIFO模式此位不起作用，用于配置存储器突发 */
		hdma_rx.Init.Direction			= DMA_PERIPH_TO_MEMORY;	    /* 禁止FIFO模式此位不起作用，用于外设突发 */
		hdma_rx.Init.PeriphInc			= DMA_PINC_DISABLE;		    /* 配置外设地址禁止自增 */
		hdma_rx.Init.MemInc				= DMA_MINC_ENABLE;		    /* 配置存储器地址自增 */
		hdma_rx.Init.PeriphDataAlignment= DMA_PDATAALIGN_BYTE;	    /* 配置外设数据传输位宽选择字节，8bit */
		hdma_rx.Init.MemDataAlignment	= DMA_MDATAALIGN_BYTE;	    /* 配置存储器数据传输位宽选择字节，8bit */
		hdma_rx.Init.Mode				= DMA_NORMAL;			    /* 配置正常模式 */
		hdma_rx.Init.Priority			= DMA_PRIORITY_HIGH;	    /* 配置优先级低 */
		
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
        /* 配置SPI中断优先级并使能中断 */
        HAL_NVIC_SetPriority(SPIx_IRQn,1,0);
        HAL_NVIC_EnableIRQ(SPIx_IRQn);
    }
	#endif
}
/*
*********************************************************************************************************
*	函 数 名: bsp_spiTransfer
*	功能说明: 启动数据传输
*	形    参: 无	
*	返 回 值: 无		return ;
********************************************************************************************************
*/
void bsp_spi1Transfer(void)
{
	if (g_spiLen > SPI_BUFFER_SIZE)
	{
        return;
	}
	/* DMA方式传输 */
#ifdef  USE_SPI_DMA
	wTransferState = TRANSFER_WAIT;
	if(HAL_SPI_TransmitReceive_DMA(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d,spi state %d\r\n", __FILE__,__LINE__,HAL_SPI_GetState(&hspi));
	}
	while(wTransferState  == TRANSFER_WAIT)
	{
		;
	}
#endif
	/* 中断方式传输 */
#ifdef  USE_SPI_INT
	wTransferState = TRANSFER_WAIT;
    //SET_BIT(hspi.Instance->CR2,SPI_IT_TXE);
	if(HAL_SPI_TransmitReceive_IT(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen) != HAL_OK)
	{
		printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
	while(wTransferState == TRANSFER_WAIT)
	{
		;
	}
#endif
	
	/* 轮询方式传输 */
#ifdef  USE_SPI_POLL
	if(HAL_SPI_TransmitReceive(&hspi,(uint8_t*)g_spiTxBuf,(uint8_t*)g_spiRxBuf,g_spiLen,1000) != HAL_OK)
	{
        printf("Wrong parameters value: file %s on line %d\r\n", __FILE__,__LINE__);
	}
#endif
}

/*
*********************************************************************************************************
*	函 数 名: HAL_SPI_TxRxCpltCallback，HAL_SPI_ErrorCallback
*	功能说明: SPI数据传输完成回调和传输错误回调
*	形    参: SPI_HandleTypeDef 类型指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_COMPLETE;
}


void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi)
{
	wTransferState = TRANSFER_ERROR;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusEnter
*	功能说明: 占用SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_Spi1BusEnter(void)
{
	g_spi_busy = 1;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusExit
*	功能说明: 释放占用的SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_Spi1BusExit(void)
{
	g_spi_busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusBusy
*	功能说明: 判断SPI总线忙，方法是检测其他SPI芯片的片选信号是否为1
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}

/*
*********************************************************************************************************
*	函 数 名: SPIx_IRQHandler，SPIx_DMA_RX_IRQHandler，SPIx_DMA_TX_IRQHandler
*	功能说明: 中断服务程序
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
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

void bsp_InitSPI2Bus(void)
{
	/*
		ʱ����λ��CPHA = 0���ڴ���ͬ��ʱ�ӵĵڶ���������(�������½�)���ݱ�����
		ʱ�Ӽ��ԣ�CPOL = 0���ڴ���ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
	*/
	bsp_InitSPI2Param(SPI_APB1_BAUDRATEPRESCALER_164_0625K,SPI_PHASE_1EDGE,SPI_POLARITY_LOW);
}


void bsp_InitSPI2Param(uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity)
{
	/* ����SPI���� */
	ink_spi.Instance					= SPIx_INK;						/* ָ��SPI */
	ink_spi.Init.BaudRatePrescaler		= _BaudRatePrescaler;			/* �����ٶ� */
	ink_spi.Init.Direction				= SPI_DIRECTION_2LINES;			/* ȫ˫�� */
	ink_spi.Init.CLKPhase				= _CLKPhase;					/* ����ʱ����λ */
	ink_spi.Init.CLKPolarity			= _CLKPolarity;					/* ����ʱ�Ӽ��� */
	ink_spi.Init.DataSize				= SPI_DATASIZE_8BIT;			/* �������ݿ��� */
	ink_spi.Init.FirstBit				= SPI_FIRSTBIT_MSB;				/* �������ݸ�λ��ǰ */
	ink_spi.Init.TIMode					= SPI_TIMODE_DISABLE;			/* ����TIģʽ */
	ink_spi.Init.CRCCalculation			= SPI_CRCCALCULATION_DISABLE;	/* ��ֹCRCУ�� */
	ink_spi.Init.CRCPolynomial			= 10;							/* ��ֹCRC�󣬴�������Ч */
	ink_spi.Init.NSS					= SPI_NSS_SOFT;					/* ����ʹ��������ʽ����Ƭѡ���� */
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


void bsp_spi2Transfer(uint8_t _value)
{
	if (HAL_SPI_Transmit(&ink_spi,&_value,1,1000) != HAL_OK)
		{
			printf("Wrong parameters valude: file %s on line %d\r\n",__FILE__,__LINE__);
		}
	//printf("%d\r\n",HAL_SPI_Transmit(&ink_spi,&_value,1,1000));
	//printf("%d\r\n",_value);
}



