#include "bsp_mlx90614.h"

/*******************************************************************************
  * @FunctionName: bsp_Mlx90614_Write
  * @Author:       trx
  * @DateTime:     2022年7月2日23:01:18 
  * @Purpose:      mlx90614红外温度传感器数据的写入
  * @param:        pBuffer：要写入数据的地址
  * @param:        WriteAddr：mlx90614红外温度传感器寄存器的地址
  * @return:       IIC总线的状态
*******************************************************************************/
 uint32_t bsp_Mlx90614_Write(uint8_t * pBuffer, uint8_t WriteAddr)
 {
	 HAL_StatusTypeDef	 status = HAL_OK;
 
	 status = HAL_I2C_Mem_Write(&iic_handle,SA,(uint16_t)WriteAddr,I2C_MEMADD_SIZE_8BIT,pBuffer,1,100);
 
	 /* 检查通讯状态 */
	 if (status != HAL_OK)
		 {
			 /* 执行用户定义的超时回调函数 */
		 }
	 while (HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		 {
			 
		 }
 
	 /* 检查eeprom芯片是否准备好对于下一个新的操作 */
	 while(HAL_I2C_IsDeviceReady(&iic_handle,SA,I2Cx_TIMEOUT_MAX,I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT)
		 {
			 
		 }
	 /* 等待数据传输结束 */
	 while(HAL_I2C_GetState(&iic_handle) != HAL_I2C_STATE_READY)
		 {
			 
		 }
	 
	 return status;
 }
/*******************************************************************************
  * @FunctionName: bsp_Mlx90614_ReadBuf
  * @Author:       trx
  * @DateTime:     2022年7月2日23:01:17 
  * @Purpose:      mlx90614红外温度传感器数据的读取
  * @param:        pBuffer：存放数据的数组地址
  * @param:        ReadAdder：想要读取的mlx90614红外温度传感器寄存器地址
  * @param:        NumByteToRead：读取几个字节
  * @return:       IIC总线的状态
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
  * @DateTime:     2022年7月2日23:01:13 
  * @Purpose:      获取mlx90614红外温度传感器的温度
  * @param:        void
  * @return:       小数点1位的温度
*******************************************************************************/
float bsp_MLX90614_ReadTemp(void)
{
	float temp = 0;
	uint8_t buf1[1] = {RAM_TOBJ1},buf_read[3];
	if(bsp_Mlx90614_Write(buf1,SA) != HAL_OK)
		{
			printf("红外传感器安装错误\r\n");
			return temp;
		}
	bsp_Mlx90614_ReadBuf(buf_read,RAM_TOBJ1,3);
	temp = (buf_read[1] << 8 | buf_read[0]) * 0.02 - 273.15;
	return temp;
}

/*********************************END OF FILE*********************************/















