#include "sys.h"

/* 使用最新的hal库提供的can外设文件 */
CAN_HandleTypeDef hCAN;
//static CanRxMsgTypeDef        RxMessage;
//static CanTxMsgTypeDef can_tx_msg;
/*******************************************************************************
  * @FunctionName: bsp_InitCan1Bus
  * @Author:       trx
  * @DateTime:     2022年5月22日 17:36:06 
  * @Purpose:      can1总线外设初始化
  * @param:        void               
  * @return:       none
*******************************************************************************/
void bsp_InitCan1Bus(void)
{
	CAN_FilterTypeDef  sFilterConfig;
	
	/*CAN单元初始化*/
	hCAN.Instance = CANx_BUS_1;			  /* CAN外设 */
//	hCAN.pTxMsg = &can_tx_msg;
//	hCAN.pRxMsg = &RxMessage;
	
	hCAN.Init.Prescaler = 3;		  /* BTR-BRP 波特率分频器  定义了时间单元的时间长度 42/(1+6+7)/3=1Mbps */
	hCAN.Init.Mode = CAN_MODE_NORMAL; /* 正常工作模式 */
	hCAN.Init.SyncJumpWidth = CAN_SJW_1TQ;	  /* BTR-SJW 重新同步跳跃宽度 1个时间单元 */
	hCAN.Init.TimeSeg1 = CAN_BS1_6TQ;	  /* BTR-TS1 时间段1 占用了6个时间单元 */
	hCAN.Init.TimeSeg2 = CAN_BS2_7TQ;	  /* BTR-TS1 时间段2 占用了7个时间单元 */
	hCAN.Init.TimeTriggeredMode = DISABLE;		  /* MCR-TTCM  关闭时间触发通信模式使能 */
	hCAN.Init.AutoBusOff = ENABLE;		  /* MCR-ABOM  自动离线管理 */
	hCAN.Init.AutoWakeUp = ENABLE;		  /* MCR-AWUM  使用自动唤醒模式 */
	hCAN.Init.AutoRetransmission = DISABLE;		  /* MCR-NART  禁止报文自动重传 	DISABLE-自动重传 */
	hCAN.Init.ReceiveFifoLocked = DISABLE;		  /* MCR-RFLM  接收FIFO 锁定模式	DISABLE-溢出时新报文会覆盖原有报文 */
	hCAN.Init.TransmitFifoPriority = DISABLE;		  /* MCR-TXFP  发送FIFO优先级 DISABLE-优先级取决于报文标示符 */
	HAL_CAN_Init(&hCAN);
	
	/*CAN过滤器初始化*/
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  /* 工作在标识符屏蔽位模式 */
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; /* 过滤器位宽为单个32位。*/
	  /* 使能报文标识符过滤器按照标识符的内容进行对比过滤，扩展ID不是如下的旧抛弃掉，是的话，会存入FIFO0 */
	  /* id左移3位，是为了将0位保留位、1位RTR位、2位IDE位补0 */
	  /* 右移16位是将整个id的高16位取出赋值给过滤器高位 */
	sFilterConfig.FilterIdHigh		   = (((uint32_t)0x1314<<3)&0xFFFF0000)>>16;			  /* 要过滤的ID高位 */
	sFilterConfig.FilterIdLow		   = (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* 要过滤的ID低位 */
	sFilterConfig.FilterMaskIdHigh	   = 0xFFFF;		  /* 过滤器高16位每位必须匹配 */
	sFilterConfig.FilterMaskIdLow	   = 0xFFFF;		  /* 过滤器低16位每位必须匹配 */
	sFilterConfig.FilterFIFOAssignment = 0; 		  /* 过滤器被关联到FIFO 0 */
	sFilterConfig.FilterActivation = ENABLE;		  /* 使能过滤器 */ 
	sFilterConfig.FilterBank = 14;
	HAL_CAN_ConfigFilter(&hCAN, &sFilterConfig);

	HAL_CAN_Start(&hCAN);
}



void HAL_CAN_MspInit(CAN_HandleTypeDef * hcan)
{
	GPIO_InitTypeDef	gpio_init;
	if(hcan->Instance == CANx_BUS_1)
	{
		CAN1_CLK_ENABLE();			/* can外设时钟使能 */
		CAN1_GPIO_CLK_ENABLE();		/* 引脚时钟使能 */

		gpio_init.Pin		= CAN1_TX_PIN;
		gpio_init.Mode		= GPIO_MODE_AF_PP;
		gpio_init.Pull		= GPIO_NOPULL;
		gpio_init.Alternate	= GPIO_AF9_CAN1;
		HAL_GPIO_Init(CAN1_TX_GPIO_PORT,&gpio_init);

		gpio_init.Pin		= CAN1_RX_PIN;
		gpio_init.Mode		= GPIO_MODE_AF_PP;
		gpio_init.Speed		= GPIO_SPEED_FREQ_HIGH;
		gpio_init.Alternate	= GPIO_AF9_CAN1;
		HAL_GPIO_Init(CAN1_RX_GPIO_PORT,&gpio_init);

		HAL_NVIC_SetPriority(CAN1_RX0_IRQn,0,0);/* 初始化中断优先级 */
		HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	}
}


HAL_StatusTypeDef bsp_Can1_Send_buf(uint32_t _id,uint8_t _buf[],uint8_t _dlc)
{
	CAN_TxHeaderTypeDef can_tx_msg;
	if(IS_CAN_STDID(_id))
		{
			can_tx_msg.StdId = _id;			
			can_tx_msg.IDE = CAN_ID_STD;
		}
	else
		{
			can_tx_msg.ExtId = _id;
			can_tx_msg.IDE = CAN_ID_EXT;
		}
		//return HAL_ERROR;
	can_tx_msg.DLC = _dlc;
	can_tx_msg.RTR = CAN_RTR_DATA;			/* 默认都是数据帧 */
	can_tx_msg.TransmitGlobalTime = DISABLE;
//	for(int i = 0;i < 8;i++)
//		can_tx_msg.Data[i] = buf[i];
//	HAL_CAN_Transmit(&hCAN,10);
	HAL_CAN_AddTxMessage(&hCAN,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX0);

		return HAL_OK;
}


void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{

  if(hcan->Instance==CANx_BUS_1)
  {
    CAN1_FPRCE_RESET();
    CAN1_RELEASE_RESET();
  
    /**CAN GPIO Configuration    
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(CAN1_TX_GPIO_PORT, CAN1_TX_PIN);
    HAL_GPIO_DeInit(CAN1_RX_GPIO_PORT, CAN1_RX_PIN);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(CANx_RX_IRQn);
  }
} 


