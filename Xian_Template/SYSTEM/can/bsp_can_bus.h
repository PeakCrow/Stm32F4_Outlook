#ifndef __BSP_CAN_BUS_H_
#define __BSP_CAN_BUS_H_



#define		CANx_BUS_1				CAN1
#define		CAN1_CLK_ENABLE()		__HAL_RCC_CAN1_CLK_ENABLE()
#define		CAN1_FPRCE_RESET()		__HAL_RCC_CAN1_FORCE_RESET()
#define		CAN1_RELEASE_RESET()	__HAL_RCC_CAN1_RELEASE_RESET()

#define		CAN1_GPIO_CLK_ENABLE()	{__HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOA_CLK_ENABLE();}
#define		CAN1_TX_GPIO_PORT		GPIOA
#define		CAN1_TX_PIN				GPIO_PIN_11

#define		CAN1_RX_GPIO_PORT		GPIOA
#define		CAN1_RX_PIN				GPIO_PIN_12

#define CANx_RX_IRQn                   CAN1_RX0_IRQn

void bsp_InitCan1Bus(void);
HAL_StatusTypeDef bsp_Can1_Send_buf(uint32_t _id,uint8_t _buf[], uint8_t _dlc);

#endif

