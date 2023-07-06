#ifndef __BSP_MLX90614_H_
#define __BSP_MLX90614_H_
#include "sys.h"


#define SA            0x00 //Slave address ����MLX90614ʱ��ַΪ0x00,���ʱ��ַĬ��Ϊ0x5a
#define EEPROM_ACCESS 0x20 //EEPROM access command
#define RAM_TOBJ1     0x07 //To1 address in the eeprom

uint32_t bsp_Mlx90614_Write(uint8_t * pBuffer, uint8_t WriteAddr);
uint32_t bsp_Mlx90614_ReadBuf(uint8_t * pBuffer, uint8_t ReadAdder, uint16_t NumByteToRead);
float    bsp_MLX90614_ReadTemp(void);




#endif



