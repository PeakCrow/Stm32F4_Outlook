#include "bsp_iic_bus.h"

I2C_HandleTypeDef	iic_handle;

static void I2C_Mode_Config(void);



void bsp_I2C_EE_Init()
{
	I2C_Mode_Config();
}



/*******************************************************************************
  * @FunctionName: I2C_Mode_Config
  * @Author:       trx
  * @DateTime:     2022年5月5日 12:46:56 
  * @Purpose:      iic工作模式配置
  * @param:        void
  * @return:       none
*******************************************************************************/
static void I2C_Mode_Config(void)
{
    iic_handle.Instance                 = I2Cx;

    iic_handle.Init.AddressingMode      = I2C_ADDRESSINGMODE_7BIT;
    iic_handle.Init.ClockSpeed          = 60000;/* 默认eeprom速度是400k */
    iic_handle.Init.DualAddressMode     = I2C_DUALADDRESS_DISABLE;
    iic_handle.Init.DutyCycle           = I2C_DUTYCYCLE_2;
    iic_handle.Init.GeneralCallMode     = I2C_GENERALCALL_DISABLE;
    iic_handle.Init.NoStretchMode       = I2C_NOSTRETCH_DISABLE;
    iic_handle.Init.OwnAddress1         = I2C_OWN_ADDRESS7;
    iic_handle.Init.OwnAddress2         = 0;

/* 初始化I2C */
HAL_I2C_Init(&iic_handle);
}
/*******************************************************************************
  * @FunctionName: HAL_I2C_MspInit
  * @Author:       trx
  * @DateTime:     2022年5月7日 19:56:26 
  * @Purpose:      iic总线引脚初始化
  * @param:        hi2c：iic总线外设句柄
  * @return:       none
*******************************************************************************/
void HAL_I2C_MspInit(I2C_HandleTypeDef * hi2c)
{
GPIO_InitTypeDef	gpio_initstruct;

    /* 初始化iic通讯的引脚时钟 */
    I2Cx_SCL_GPIO_CLK_ENABLE();
    I2Cx_SDA_GPIO_CLK_ENABLE();

    /* 使能iic总线时钟 */
    I2Cx_CLK_ENABLE();

    /* 配置iic总线时钟引脚 */
    gpio_initstruct.Pin         = I2Cx_SCL_PIN;
    gpio_initstruct.Mode        = GPIO_MODE_AF_OD;
    gpio_initstruct.Pull        = GPIO_NOPULL;
    gpio_initstruct.Speed       = GPIO_SPEED_FAST;
    gpio_initstruct.Alternate   = I2Cx_SCL_AF;
    HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT,&gpio_initstruct);

    /* 配置iic总线数据引脚 */
    gpio_initstruct.Pin         = I2Cx_SDA_PIN;
    gpio_initstruct.Alternate   = I2Cx_SDA_AF;
    HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT,&gpio_initstruct);

    /* 占用iic总线外设时钟 */
    I2Cx_FORCE_RESET();
    /* 释放iic总线外设时钟 */
    I2Cx_RELEASE_RESET();
}



/*******************************************************************************
  * @FunctionName: I2C_EE_ByteWrite
  * @Author:       trx
  * @DateTime:     2022年5月7日 20:15:08 
  * @Purpose:      写一个字节到iic eeprom中
  * @param:        pBuffer  ：缓冲区写指针
  * @param:        WriteAddr：写地址
  * @return:       status   ：iic总线通讯状态
*******************************************************************************/
uint32_t I2C_EE_ByteWrite(uint8_t * pBuffer, uint8_t WriteAddr)
{
    HAL_StatusTypeDef   status = HAL_OK;

    status = HAL_I2C_Mem_Write(&iic_handle,EEPROM_ADDRESS,
                                (uint16_t)WriteAddr,
                                I2C_MEMADD_SIZE_8BIT,pBuffer,1,100);

    /* 检查通讯状态 */
    if (status != HAL_OK)
    {
    	/* 执行用户定义的超时回调函数 */
    }
    while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY){}

    /* 检查eeprom芯片是否准备好对于下一个新的操作 */
    while(HAL_I2C_IsDeviceReady(&iic_handle,EEPROM_ADDRESS,
                            I2Cx_TIMEOUT_MAX,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT){}
    /* 等待数据传输结束 */
    while(HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY){}

    return status;
}



/*******************************************************************************
  * @FunctionName: I2C_EE_BufferRead
  * @Author:       trx
  * @DateTime:     2022年5月7日 20:22:07 
  * @Purpose:      从eeprom里面读取数据
  * @param:        pBuffer      ：存放读取eeprom芯片数据的缓冲区指针
  * @param:        ReadAdder    ：读取数据eeporm芯片数据的地址
  * @param:        NumByteToRead：读取数据的字节个数
  * @return:       none
*******************************************************************************/
uint32_t I2C_EE_BufferRead(uint8_t * pBuffer, uint8_t ReadAdder, uint16_t NumByteToRead)
{
    HAL_StatusTypeDef status = HAL_OK;

    status = HAL_I2C_Mem_Read(&iic_handle,EEPROM_ADDRESS,
                            ReadAdder,I2C_MEMADD_SIZE_8BIT,
                            (uint8_t*)pBuffer,NumByteToRead,1000);

    return status;
}

/*******************************************************************************
  * @FunctionName: I2C_EE_PageWrite
  * @Author:       trx
  * @DateTime:     2022年5月7日 20:37:36 
  * @Purpose:      在eeprom中的一个写循环可以写多个字节，但一次写入的字节数不能超过eeprom页的大小，AT24C02每页有8个字节
  * @param:        pBuffer       ：缓冲去地址
  * @param:        WriteAddr     ：写地址
  * @param:        NumByteToWrite：写入的数据字数
  * @return:       status：iic总线通讯的状态
*******************************************************************************/
uint32_t I2C_EE_PageWrite(uint8_t * pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
    HAL_StatusTypeDef status = HAL_OK;

    /* 写eeprom的页大小 */
    status = HAL_I2C_Mem_Write(&iic_handle,EEPROM_ADDRESS,
                                WriteAddr,I2C_MEMADD_SIZE_8BIT,
                                (uint8_t*)(pBuffer),NumByteToWrite,100);

    while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
    {
        //printf("iic总线发生错误");
    }

    /* 检查eeprom芯片是否准备好下一次新操作 */
    while (HAL_I2C_IsDeviceReady(&iic_handle,EEPROM_ADDRESS,
                                I2Cx_MAX_TRIALS,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
    {
        //printf("iic总线没有准备好");
    }

    /* 等待数据传输结束 */
    while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
    {
        //printf("iic总线数据传输未结束");
    }

	return status;
}


void I2C_EE_BufferWrite(uint8_t * pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0;

    Addr = WriteAddr % EEPROM_PAGESIZE;             /* 先得到页数 */
    count = EEPROM_PAGESIZE - Addr;                 /* 在得到当前页数的第几个位置 */
    NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;   /* 计算写入的数据是否满足一页 */
    NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE; /* 计算写入一页或者不满一页的数据个数 */

    /* 如果写入地址与eeprom页大小对齐 */
    if (Addr == 0)
    {
        if (NumOfPage == 0)
            {
                I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
            }
        else
        {
            while (NumOfPage--)
                {
                    I2C_EE_PageWrite(pBuffer,WriteAddr,EEPROM_PAGESIZE);
                    WriteAddr += EEPROM_PAGESIZE;	/* 芯片数据写入的地址递增一页 */
                    pBuffer += EEPROM_PAGESIZE;		/* 数据数组下入的索引递增一页 */
                }
            /* 如果剩余的个数为0，则不写入，不为0，则写入 */
            if (NumOfSingle != 0)
                {
                    I2C_EE_PageWrite(pBuffer,WriteAddr, NumOfSingle);
                }
        }
    }
    /* 如果写入的地址不与iic芯片页大小对齐 */
    else
    {
        if (NumOfPage == 0)
        {
        I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
        }
    else
    {
        NumByteToWrite -= count;
        NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;
        NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

        if (count != 0)
        {
            I2C_EE_PageWrite(pBuffer,WriteAddr,count);
            WriteAddr += count;
            pBuffer += count;
        }

        while (NumOfPage--)
        {
            I2C_EE_PageWrite(pBuffer,WriteAddr,EEPROM_PAGESIZE);
            WriteAddr += EEPROM_PAGESIZE;
            pBuffer += EEPROM_PAGESIZE;
        }

        if (NumOfSingle != 0)
        {
            I2C_EE_PageWrite(pBuffer,WriteAddr,NumOfSingle);
        }
        }
    }
}

#if 1
uint8_t     I2Cx_BUFFER_WRITE[I2Cx_MAX_BYTE_NUM];
uint8_t     I2Cx_BUFFER_READ[I2Cx_MAX_BYTE_NUM];

/*******************************************************************************
  * @FunctionName: DemoIicEeprom
  * @Author:       trx
  * @DateTime:     2022年5月7日 21:13:33 
  * @Purpose:      eeprom芯片读写测试
  * @param:        none
  * @return:       none
*******************************************************************************/
void DemoIicEeprom()
{
    uint16_t i;
    printf("写入数据\r\n");

    for (i = 0; i < I2Cx_MAX_BYTE_NUM; ++i)
    {
        I2Cx_BUFFER_WRITE[i] = i;
        printf("0x%02x ",I2Cx_BUFFER_WRITE[i]);
        if (i % 16 == 15)
        {
            printf("\r\n");
        }
    }

    /* 将ii2cx_buffer_write中顺序递增的数据写入eeprom中 */
    /* 死循环到此函数中 */
    I2C_EE_BufferWrite(I2Cx_BUFFER_WRITE,EEP_Firstpage,I2Cx_MAX_BYTE_NUM);

    printf("读出的数据\r\n");

    /* 将eeprim中读出的数据按照顺序放入到iicx_buffer_read中 */
    I2C_EE_BufferRead(I2Cx_BUFFER_READ,EEP_Firstpage,I2Cx_MAX_BYTE_NUM);

    /* 将iicx_buffer_read中的数据打印出来 */
    for (i = 0; i < I2Cx_MAX_BYTE_NUM; ++i)
    {
        if (I2Cx_BUFFER_READ[i] != I2Cx_BUFFER_WRITE[i])
        {
            printf("0x%02x ",I2Cx_BUFFER_READ[i]);
            printf("错误：eeprom芯片写入的数据与读出的数据不一致!");
            break;
        }
    printf("0x%02x ",I2Cx_BUFFER_READ[i]);
    if (i % 16 == 15)
    {
        printf("\r\n");
    }
    }
    printf("AT24C02读写测试成功\r\n");
}

#endif

