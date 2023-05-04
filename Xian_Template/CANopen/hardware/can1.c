// Includes for the Canfestival driver
#include "can1.h"


static CO_Data *co_data = NULL;


//Initialize the CAN hardware 
unsigned char CAN1_Init(CO_Data * d, uint32_t bitrate)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
  
  /* save the canfestival handle */  
  co_data = d;

  /* CAN GPIOs configuration **************************************************/

  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(CAN_GPIO_CLK, ENABLE);

  /* Connect CAN pins to AF7 */
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, GPIO_AF_CANx);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, GPIO_AF_CANx); 
  
  /* Configure CAN RX and TX pins */
  GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

  /* NVIC configuration *******************************************************/
  NVIC_InitStructure.NVIC_IRQChannel = CANx_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* CAN configuration ********************************************************/  
  /* Enable CAN clock */
  RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);
  
  /* CAN register init */
  CAN_DeInit(CANx);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    
  /* CAN Baudrate (CAN clocked at 42 MHz)  42e6 / ( 6 * (1+BS1+BS2))  */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  if(bitrate == 1000000){
  	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
  	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
  }
  else {	//除去1M频率。剩下都配置为500K
  	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
  	CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
  }

  CAN_InitStructure.CAN_Prescaler = 6;
  CAN_Init(CANx, &CAN_InitStructure);

  /* CAN filter init */
  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure);
  
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);

  return 1;
}

// The driver send a CAN message passed from the CANopen stack
unsigned char canSend(CAN_PORT notused, Message *m)
{
	int i, res;
	CanTxMsg TxMessage = {0};
	TxMessage.StdId = m->cob_id;
	TxMessage.IDE = CAN_ID_STD;
	if(m->rtr)
  		TxMessage.RTR = CAN_RTR_REMOTE;
	else
  		TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = m->len;
	for(i=0 ; i<m->len ; i++)
		TxMessage.Data[i] = m->data[i]; 
    res = CAN_Transmit(CANx, &TxMessage);
	if(res == CAN_TxStatus_NoMailBox)
		return 0; 	// error
    return 1;		// succesful
}

//The driver pass a received CAN message to the stack
/*
unsigned char canReceive(Message *m)
{
}
*/
unsigned char canChangeBaudRate_driver( CAN_HANDLE fd, char* baud)
{
	return 0;
}

/**
  * @brief  This function handles CAN1 RX0 interrupt request.
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
	int i;
	CanRxMsg RxMessage = {0};
	Message rxm = {0};
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	// Drop extended frames
	if(RxMessage.IDE == CAN_ID_EXT) //不处理扩展帧
		return;
	rxm.cob_id = RxMessage.StdId;
  	if(RxMessage.RTR == CAN_RTR_REMOTE)//远程帧
		rxm.rtr = 1;
	rxm.len = RxMessage.DLC;
	for(i=0 ; i<rxm.len ; i++)
  		 rxm.data[i] = RxMessage.Data[i];
		
		/*****这里是自己的数据接收部分*******/
	if( rxm.cob_id>>7==0xB)	//快速SDO回应ID为0x580+对方id, 右移7位即为0xB 
	{
		int16_t test=0;
		switch(rxm.cob_id)
		{
			case 0x582:
				test=rxm.data[4]|rxm.data[5]<<8;
				printf("test=%d,\r\n",test);	//其实在中断不应该使用打印函数的，这里是方便调试而已
				break;
			default:
				break;
		}
	
	}
	else
		canDispatch(co_data, &rxm);//CANopen自身的处理函数，因为快速SDO不需要反馈，所以在上边处理后就不需要调用这步了
}

