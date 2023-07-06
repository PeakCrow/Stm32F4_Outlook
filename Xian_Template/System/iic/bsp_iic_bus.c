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
  * @DateTime:     2022��5��5�� 12:46:56 
  * @Purpose:      iic����ģʽ����
  * @param:        void               
  * @return:       none
*******************************************************************************/
static void I2C_Mode_Config(void)
{
	iic_handle.Instance					= I2Cx;

	iic_handle.Init.AddressingMode		= I2C_ADDRESSINGMODE_7BIT;
	iic_handle.Init.ClockSpeed			= 60000;/* Ĭ��eeprom�ٶ���400k */
	iic_handle.Init.DualAddressMode		= I2C_DUALADDRESS_DISABLE;
	iic_handle.Init.DutyCycle			= I2C_DUTYCYCLE_2;
	iic_handle.Init.GeneralCallMode		= I2C_GENERALCALL_DISABLE;
	iic_handle.Init.NoStretchMode		= I2C_NOSTRETCH_DISABLE;
	iic_handle.Init.OwnAddress1			= I2C_OWN_ADDRESS7;
	iic_handle.Init.OwnAddress2			= 0;

	/* ��ʼ��I2C */
	HAL_I2C_Init(&iic_handle);
}
/*******************************************************************************
  * @FunctionName: HAL_I2C_MspInit
  * @Author:       trx
  * @DateTime:     2022��5��7�� 19:56:26 
  * @Purpose:      iic�������ų�ʼ��
  * @param:        hi2c��iic����������
  * @return:       none
*******************************************************************************/
void HAL_I2C_MspInit(I2C_HandleTypeDef * hi2c)
{
	GPIO_InitTypeDef	gpio_initstruct;

	/* ��ʼ��iicͨѶ������ʱ�� */
	I2Cx_SCL_GPIO_CLK_ENABLE();
	I2Cx_SDA_GPIO_CLK_ENABLE();

	/* ʹ��iic����ʱ�� */
	I2Cx_CLK_ENABLE();

	/* ����iic����ʱ������ */
	gpio_initstruct.Pin			= I2Cx_SCL_PIN;
	gpio_initstruct.Mode		= GPIO_MODE_AF_OD;
	gpio_initstruct.Pull		= GPIO_NOPULL;
	gpio_initstruct.Speed		= GPIO_SPEED_FAST;
	gpio_initstruct.Alternate	= I2Cx_SCL_AF;
	HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT,&gpio_initstruct);

	/* ����iic������������ */
	gpio_initstruct.Pin			= I2Cx_SDA_PIN;
	gpio_initstruct.Alternate	= I2Cx_SDA_AF;
	HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT,&gpio_initstruct);

	/* ռ��iic��������ʱ�� */
	I2Cx_FORCE_RESET();
	/* �ͷ�iic��������ʱ�� */
	I2Cx_RELEASE_RESET();
}



/*******************************************************************************
  * @FunctionName: I2C_EE_ByteWrite
  * @Author:       trx
  * @DateTime:     2022��5��7�� 20:15:08 
  * @Purpose:      дһ���ֽڵ�iic eeprom��
  * @param:        pBuffer  ��������дָ��
  * @param:        WriteAddr��д��ַ
  * @return:       status   ��iic����ͨѶ״̬
*******************************************************************************/
uint32_t I2C_EE_ByteWrite(uint8_t * pBuffer, uint8_t WriteAddr)
{
	HAL_StatusTypeDef	status = HAL_OK;

	status = HAL_I2C_Mem_Write(&iic_handle,EEPROM_ADDRESS,(uint16_t)WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,1,100);

	/* ���ͨѶ״̬ */
	if (status != HAL_OK)
		{
			/* ִ���û�����ĳ�ʱ�ص����� */
		}
	while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		{
			
		}

	/* ���eepromоƬ�Ƿ�׼���ö�����һ���µĲ��� */
	while(HAL_I2C_IsDeviceReady(&iic_handle,EEPROM_ADDRESS,I2Cx_TIMEOUT_MAX,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
		{
			
		}
	/* �ȴ����ݴ������ */
	while(HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		{
			
		}
	
	return status;
}



/*******************************************************************************
  * @FunctionName: I2C_EE_BufferRead
  * @Author:       trx
  * @DateTime:     2022��5��7�� 20:22:07 
  * @Purpose:      ��eeprom�����ȡ����
  * @param:        pBuffer      ����Ŷ�ȡeepromоƬ���ݵĻ�����ָ��
  * @param:        ReadAdder    ����ȡ����eepormоƬ���ݵĵ�ַ
  * @param:        NumByteToRead����ȡ���ݵ��ֽڸ���
  * @return:       none
*******************************************************************************/
uint32_t I2C_EE_BufferRead(uint8_t * pBuffer, uint8_t ReadAdder, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(&iic_handle,EEPROM_ADDRESS,ReadAdder,I2C_MEMADD_SIZE_8BIT,(uint8_t*)pBuffer,NumByteToRead,1000);

	return status;
}

/*******************************************************************************
  * @FunctionName: I2C_EE_PageWrite
  * @Author:       trx
  * @DateTime:     2022��5��7�� 20:37:36 
  * @Purpose:      ��eeprom�е�һ��дѭ������д����ֽڣ���һ��д����ֽ������ܳ���eepromҳ�Ĵ�С��AT24C02ÿҳ��8���ֽ�
  * @param:        pBuffer       ������ȥ��ַ
  * @param:        WriteAddr     ��д��ַ
  * @param:        NumByteToWrite��д�����������
  * @return:       status��iic����ͨѶ��״̬
*******************************************************************************/
uint32_t I2C_EE_PageWrite(uint8_t * pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	HAL_StatusTypeDef status = HAL_OK;

	/* дeeprom��ҳ��С */
	status = HAL_I2C_Mem_Write(&iic_handle,EEPROM_ADDRESS,WriteAddr,I2C_MEMADD_SIZE_8BIT,(uint8_t*)(pBuffer),NumByteToWrite,100);

	while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		{
			//printf("iic���߷�������");
		}

	/* ���eepromоƬ�Ƿ�׼������һ���²��� */
	while (HAL_I2C_IsDeviceReady(&iic_handle,EEPROM_ADDRESS,I2Cx_MAX_TRIALS,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
		{
			//printf("iic����û��׼����");
		}

	/* �ȴ����ݴ������ */
	while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		{
			//printf("iic�������ݴ���δ����");
		}

	return status;
}


void I2C_EE_BufferWrite(uint8_t * pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0,NumOfSingle = 0,Addr = 0,count = 0;

	Addr = WriteAddr % EEPROM_PAGESIZE;				/* �ȵõ�ҳ�� */
	count = EEPROM_PAGESIZE - Addr;					/* �ڵõ���ǰҳ���ĵڼ���λ�� */
	NumOfPage = NumByteToWrite / EEPROM_PAGESIZE;	/* ����д��������Ƿ�����һҳ */
	NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;	/* ����д��һҳ���߲���һҳ�����ݸ��� */

	/* ���д���ַ��eepromҳ��С���� */
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
							WriteAddr += EEPROM_PAGESIZE;	/* оƬ����д��ĵ�ַ����һҳ */
							pBuffer += EEPROM_PAGESIZE;		/* ���������������������һҳ */
						}
					/* ���ʣ��ĸ���Ϊ0����д�룬��Ϊ0����д�� */
					if (NumOfSingle != 0)
						{
							I2C_EE_PageWrite(pBuffer,WriteAddr, NumOfSingle);
						}
				}
		}
	/* ���д��ĵ�ַ����iicоƬҳ��С���� */
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
uint8_t		I2Cx_BUFFER_WRITE[I2Cx_MAX_BYTE_NUM];
uint8_t		I2Cx_BUFFER_READ[I2Cx_MAX_BYTE_NUM];

/*******************************************************************************
  * @FunctionName: DemoIicEeprom
  * @Author:       trx
  * @DateTime:     2022��5��7�� 21:13:33 
  * @Purpose:      eepromоƬ��д����
  * @param:        none
  * @return:       none
*******************************************************************************/
void DemoIicEeprom()
{
	uint16_t i;
	printf("д������\r\n");

	for (i = 0; i < I2Cx_MAX_BYTE_NUM; ++i)
		{
			I2Cx_BUFFER_WRITE[i] = i;
			printf("0x%02x ",I2Cx_BUFFER_WRITE[i]);
			if (i % 16 == 15)
				{
					printf("\r\n");
				}
		}

	/* ��ii2cx_buffer_write��˳�����������д��eeprom�� */
	/* ��ѭ�����˺����� */
	I2C_EE_BufferWrite(I2Cx_BUFFER_WRITE,EEP_Firstpage,I2Cx_MAX_BYTE_NUM);

	printf("����������\r\n");

	/* ��eeprim�ж��������ݰ���˳����뵽iicx_buffer_read�� */
	I2C_EE_BufferRead(I2Cx_BUFFER_READ,EEP_Firstpage,I2Cx_MAX_BYTE_NUM);

	/* ��iicx_buffer_read�е����ݴ�ӡ���� */
	for (i = 0; i < I2Cx_MAX_BYTE_NUM; ++i)
		{
			if (I2Cx_BUFFER_READ[i] != I2Cx_BUFFER_WRITE[i])
				{
					printf("0x%02x ",I2Cx_BUFFER_READ[i]);
					printf("����eepromоƬд�����������������ݲ�һ��!");
					break;
				}
			printf("0x%02x ",I2Cx_BUFFER_READ[i]);
			if (i % 16 == 15)
				{
					printf("\r\n");
				}
		}	
	printf("AT24C02��д���Գɹ�\r\n");
}

#endif

