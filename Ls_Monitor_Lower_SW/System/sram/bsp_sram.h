#ifndef __BSP_SRAM_H_
#define __BSP_SRAM_H_

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//SDRAM驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/18
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//使用NOR/SRAM的 Bank1.sector3,地址位HADDR[27,26]=10 
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define Bank1_SRAM3_ADDR    ((u32)(0x68000000))	


extern SRAM_HandleTypeDef SRAM_Handler;    //SRAM句柄

void bsp_InitSram(void);
void FSMC_SRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n);
void FSMC_SRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n);
#endif
