#ifndef __BSP_CAN_BUS_H_
#define __BSP_CAN_BUS_H_

#include "sys.h"

#define		CANx_BUS_1				CAN1
#define		CAN1_CLK_ENABLE()		__HAL_RCC_CAN1_CLK_ENABLE()
#define		CAN1_FPRCE_RESET()		__HAL_RCC_CAN1_FORCE_RESET()
#define		CAN1_RELEASE_RESET()	__HAL_RCC_CAN1_RELEASE_RESET()

#define		CAN1_GPIO_CLK_ENABLE()	{__HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOA_CLK_ENABLE();}
#define		CAN1_TX_GPIO_PORT		GPIOA
#define		CAN1_TX_PIN				GPIO_PIN_11

#define		CAN1_RX_GPIO_PORT		GPIOA
#define		CAN1_RX_PIN				GPIO_PIN_12

#define 	CAN1_RX_IRQN            CAN1_RX0_IRQn

#define		CAN1_BUS_BAUDE_1M		3					/* 使用1mbps的速度时，在发送报文时(不管时小于3条还是大于3条),都会丢失少量报文，谨慎使用 */
#define		CAN1_BUS_BAUDE_500k		6
#define		CAN1_BUS_BAUDE_250k		12
#define		CAN1_BUS_BUUDE_RATE		CAN1_BUS_BAUDE_500k	/* 默认波特率 */

extern CAN_RxHeaderTypeDef can_rx_msg;
extern uint8_t g_canrxbuf[8];			/* 不能做成全局变量，不然无法触发中断(?????) */



void bsp_InitCan1Bus(void);
HAL_StatusTypeDef bsp_Can1_Send_buf(uint32_t _id,uint8_t _buf[], uint8_t _dlc);
uint8_t bsp_Can1_Receive_buf(uint32_t _id,uint8_t _buf[]);

#endif

