#include "bsp_mlx90614.h"

/*******************************************************************************
  * @FunctionName: bsp_Mlx90614_Write
  * @Author:       trx
  * @DateTime:     2022��7��2��23:01:18 
  * @Purpose:      mlx90614�����¶ȴ��������ݵ�д��
  * @param:        pBuffer��Ҫд�����ݵĵ�ַ
  * @param:        WriteAddr��mlx90614�����¶ȴ������Ĵ����ĵ�ַ
  * @return:       IIC���ߵ�״̬
*******************************************************************************/
 uint32_t bsp_Mlx90614_Write(uint8_t * pBuffer, uint8_t WriteAddr)
 {
	 HAL_StatusTypeDef	 status = HAL_OK;
 
	 status = HAL_I2C_Mem_Write(&iic_handle,SA,(uint16_t)WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,1,100);
 
	 /* ���ͨѶ״̬ */
	 if (status != HAL_OK)
		 {
			 /* ִ���û�����ĳ�ʱ�ص����� */
		 }
	 while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		 {
			 
		 }
 
	 /* ���eepromоƬ�Ƿ�׼���ö�����һ���µĲ��� */
	 while(HAL_I2C_IsDeviceReady(&iic_handle,SA,I2Cx_TIMEOUT_MAX,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
		 {
			 
		 }
	 /* �ȴ����ݴ������ */
	 while(HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		 {
			 
		 }
	 
	 return status;
 }
/*******************************************************************************
  * @FunctionName: bsp_Mlx90614_ReadBuf
  * @Author:       trx
  * @DateTime:     2022��7��2��23:01:17 
  * @Purpose:      mlx90614�����¶ȴ��������ݵĶ�ȡ
  * @param:        pBuffer��������ݵ������ַ
  * @param:        ReadAdder����Ҫ��ȡ��mlx90614�����¶ȴ������Ĵ�����ַ
  * @param:        NumByteToRead����ȡ�����ֽ�
  * @return:       IIC���ߵ�״̬
*******************************************************************************/
 uint32_t bsp_Mlx90614_ReadBuf(uint8_t * pBuffer, uint8_t ReadAdder, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Read(&iic_handle,SA,ReadAdder,I2C_MEMADD_SIZE_8BIT,(uint8_t*)pBuffer,NumByteToRead,1000);

	return status;
}

/*******************************************************************************
  * @FunctionName: bsp_MLX90614_ReadTemp
  * @Author:       trx
  * @DateTime:     2022��7��2��23:01:13 
  * @Purpose:      ��ȡmlx90614�����¶ȴ��������¶�
  * @param:        void
  * @return:       С����1λ���¶�
*******************************************************************************/
float bsp_MLX90614_ReadTemp(void)
{
	float temp = 0;
	uint8_t buf1[1] = {RAM_TOBJ1},buf_read[3];
	if(bsp_Mlx90614_Write(buf1,SA) != HAL_OK)
		{
			printf("���⴫������װ����\r\n");
			return temp;
		}
	bsp_Mlx90614_ReadBuf(buf_read,RAM_TOBJ1,3);
	temp = (buf_read[1] << 8 | buf_read[0]) * 0.02 - 273.15;
	return temp;
}

/*********************************END OF FILE*********************************/















