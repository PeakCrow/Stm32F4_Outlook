#include "bsp_can_bus.h"

/* ʹ�����µ�hal���ṩ��can�����ļ� */
/* ���ļ���û��ʹ�û�����App_Printf���� */
CAN_HandleTypeDef hCAN;
CAN_RxHeaderTypeDef can_rx_msg;
uint8_t g_canrxbuf[8] = {0};			/* ��������ȫ�ֱ�������Ȼ�޷������ж�(?????) */

/*******************************************************************************
  * @FunctionName: bsp_InitCan1Bus
  * @Author:       trx
  * @DateTime:     2022��5��22�� 17:36:06 
  * @Purpose:      can1���������ʼ��
  * @param:        void
  * @return:       none
*******************************************************************************/
void bsp_InitCan1Bus(void)
{
	CAN_FilterTypeDef  sFilterConfig;
	CAN_FilterTypeDef  sFilterConfig2;
	
	/*CAN��Ԫ��ʼ��*/
	hCAN.Instance 					  = CANx_BUS_1;				/* CAN���� */
	hCAN.Init.Prescaler 			  = CAN1_BUS_BUUDE_RATE;	/* BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 42/(1+6+7)/6=500kbps */
	hCAN.Init.Mode 					  = CAN_MODE_NORMAL; 		/* ��������ģʽ */
	hCAN.Init.SyncJumpWidth 		  = CAN_SJW_1TQ;	  		/* BTR-SJW ����ͬ����Ծ��� 1��ʱ�䵥Ԫ */
	hCAN.Init.TimeSeg1 				  = CAN_BS1_6TQ;	  		/* BTR-TS1 ʱ���1 ռ����6��ʱ�䵥Ԫ */
	hCAN.Init.TimeSeg2 				  = CAN_BS2_7TQ;	  		/* BTR-TS1 ʱ���2 ռ����7��ʱ�䵥Ԫ */
	hCAN.Init.TimeTriggeredMode 	  = DISABLE;		  		/* MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ�� */
	hCAN.Init.AutoBusOff 			  = ENABLE;			  		/* MCR-ABOM  �Զ����߹��� */
	hCAN.Init.AutoWakeUp 			  = ENABLE;		 	  		/* MCR-AWUM  ʹ���Զ�����ģʽ */
	hCAN.Init.AutoRetransmission  	  = DISABLE;		  		/* MCR-NART  ��ֹ�����Զ��ش� 	DISABLE-�Զ��ش� */
	hCAN.Init.ReceiveFifoLocked 	  = DISABLE;		  		/* MCR-RFLM  ����FIFO ����ģʽ	DISABLE-���ʱ�±��ĻḲ��ԭ�б��� */
	hCAN.Init.TransmitFifoPriority 	  = ENABLE;			  		/* MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� */
	HAL_CAN_Init(&hCAN);
	
	/*CAN��������ʼ��*/
	sFilterConfig.FilterMode 		  = CAN_FILTERMODE_IDMASK;  /* �����ڱ�ʶ������λģʽ */
	sFilterConfig.FilterScale 		  = CAN_FILTERSCALE_32BIT; /* ������λ��Ϊ����32λ��*/
	/* ʹ�ܱ��ı�ʶ�����������ձ�ʶ�������ݽ��жԱȹ��ˣ���չID�������µľ����������ǵĻ��������FIFO0 */
	/* id����3λ����Ϊ�˽�0λ����λ��1λRTRλ��2λIDEλ��0 */
	/* ����16λ�ǽ�����id�ĸ�16λȡ����ֵ����������λ */
	sFilterConfig.FilterIdHigh		  = (((uint32_t)0x1314<<3)&0xFFFF0000)>>16;			  /* Ҫ���˵�ID��λ */
	sFilterConfig.FilterIdLow		  = (((uint32_t)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* Ҫ���˵�ID��λ */
	sFilterConfig.FilterMaskIdHigh	  = 0xFFFF;		  	  /* ��������16λÿλ����ƥ�� */
	sFilterConfig.FilterMaskIdLow	  = 0xFFFF;		  	  /* ��������16λÿλ����ƥ�� */
	sFilterConfig.FilterFIFOAssignment= CAN_FILTER_FIFO0; /* ��������������FIFO 0 */
	sFilterConfig.FilterActivation 	  = ENABLE;		  	  /* ʹ�ܹ����� */ 
	sFilterConfig.FilterBank 		  = 14;
	HAL_CAN_ConfigFilter(&hCAN, &sFilterConfig);
	
	/*CAN��������ʼ��*/
	sFilterConfig2.FilterMode 		  = CAN_FILTERMODE_IDMASK;  /* �����ڱ�ʶ������λģʽ */
	sFilterConfig2.FilterScale 		  = CAN_FILTERSCALE_32BIT; /* ������λ��Ϊ����32λ��*/
	sFilterConfig2.FilterIdHigh		  = (((uint32_t)0x1315<<3)&0xFFFF0000)>>16;			  /* Ҫ���˵�ID��λ */
	sFilterConfig2.FilterIdLow		  = (((uint32_t)0x1315<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; /* Ҫ���˵�ID��λ */
	sFilterConfig2.FilterMaskIdHigh	  = 0xFFFF;		  	  /* ��������16λÿλ����ƥ�� */
	sFilterConfig2.FilterMaskIdLow	  = 0xFFFF;		  	  /* ��������16λÿλ����ƥ�� */
	sFilterConfig2.FilterFIFOAssignment= CAN_FILTER_FIFO1;/* ��������������FIFO 1 */
	sFilterConfig2.FilterActivation   = ENABLE;		  	  /* ʹ�ܹ����� */ 
	sFilterConfig2.FilterBank 		  = 10;
	HAL_CAN_ConfigFilter(&hCAN, &sFilterConfig2);

	HAL_CAN_Start(&hCAN);								  /* �����ô˺����޷�����can���� */
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO0_FULL);
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO0_OVERRUN);

	
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO1_FULL);
	HAL_CAN_ActivateNotification(&hCAN,CAN_IT_RX_FIFO1_OVERRUN);
}



/*******************************************************************************
  * @FunctionName: HAL_CAN_MspInit
  * @Author:       trx
  * @DateTime:     2022��5��25��22:14:29 
  * @Purpose:      can�������ų�ʼ�����������ж�����
  * @param:        hcan��can������(ȫ�ֱ���)
  * @return:       none
*******************************************************************************/
void HAL_CAN_MspInit(CAN_HandleTypeDef * hcan)
{
	GPIO_InitTypeDef	gpio_init;
	if(hcan->Instance == CANx_BUS_1)
	{
		CAN1_CLK_ENABLE();			/* can����ʱ��ʹ�� */
		CAN1_GPIO_CLK_ENABLE();		/* ����ʱ��ʹ�� */

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

		HAL_NVIC_SetPriority(CAN1_RX_IRQN,0,0);/* ��ʼ���ж����ȼ� */
		HAL_NVIC_EnableIRQ(CAN1_RX_IRQN);
	}
}

/*******************************************************************************
  * @FunctionName: bsp_Can1_Send_buf
  * @Author:       trx
  * @DateTime:     2022��5��25�� 20:58:40 
  * @Purpose:      can���ͺ�����֧��һ�η���3�����ϵı��ģ���Ȼһ�����ֻ�ܷ���3��
  * @param:        _id                Ҫ���͵ı���id
  * @param:        _buf[]             ���ĵ�����
  * @param:        _dlc               �������ݵĳ��ȣ�0-8��ѡ
  * @return:       HAL_OK:�����ͳɹ�;HAL_ERROR:����ʧ��
*******************************************************************************/
HAL_StatusTypeDef bsp_Can1_Send_buf(uint32_t _id,uint8_t _buf[],uint8_t _dlc)
{
	CAN_TxHeaderTypeDef can_tx_msg;
	if(IS_CAN_STDID(_id))
		{
			can_tx_msg.StdId = _id;
			can_tx_msg.IDE = CAN_ID_STD;
		}
	else if(IS_CAN_EXTID(_id))
		{
			can_tx_msg.ExtId = _id;
			can_tx_msg.IDE = CAN_ID_EXT;
		}
	else
		{
			printf("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
		}
	if (_dlc >8)
		return HAL_ERROR;
	else
		can_tx_msg.DLC = _dlc;
	can_tx_msg.RTR = CAN_RTR_DATA;			/* Ĭ�϶�������֡ */
	can_tx_msg.TransmitGlobalTime = DISABLE;
	
	if(HAL_CAN_IsTxMessagePending(&hCAN,CAN_TX_MAILBOX0) == HAL_OK)/* ����û�й���ķ��ͱ���  */
		HAL_CAN_AddTxMessage(&hCAN,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX0);
	else if(HAL_CAN_IsTxMessagePending(&hCAN,CAN_TX_MAILBOX0) != HAL_OK)
		HAL_CAN_AddTxMessage(&hCAN,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX1);
	else if(HAL_CAN_IsTxMessagePending(&hCAN,CAN_TX_MAILBOX0) != HAL_OK)
		HAL_CAN_AddTxMessage(&hCAN,&can_tx_msg,_buf,(uint32_t*)CAN_TX_MAILBOX2);
	else
		return HAL_ERROR;
	while(HAL_CAN_GetTxMailboxesFreeLevel(&hCAN) == 0){}			/* ������з�������Ϊ0������ѭ�����ȴ��������䲻Ϊ�գ��п��õ����� */
																	/* ���һ������ţ����ⱨ����û�з���״̬ */
	return HAL_OK;

}

/*******************************************************************************
  * @FunctionName: HAL_CAN_MspDeInit
  * @Author:       trx
  * @DateTime:     2022��5��25��23:55:24 
  * @Purpose:      can����ǳ�ʼ��
  * @param:        hcan��can������
  * @return:       none
*******************************************************************************/
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
    HAL_NVIC_DisableIRQ(CAN1_RX_IRQN);
  }
} 

/*******************************************************************************
  * @FunctionName: HAL_CAN_RxFifo0FullCallback
  * @Author:       trx
  * @DateTime:     2022��5��26�� 20:07:04 
  * @Purpose:      can����fifo0�����ص�����
  * @param:        hcan��can������
  * @return:       none
*******************************************************************************/
void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
	printf("fifo0���ݽ�����\r\n");			/* Ŀǰ�޷�����FIFO����������� */
}

/*******************************************************************************
  * @FunctionName: HAL_CAN_RxFifo0MsgPendingCallback
  * @Author:       trx
  * @DateTime:     2022��5��26�� 20:07:03 
  * @Purpose:      can����fifo0���ڽ������ݻص�����
  * @param:        hcan��can������
  * @return:       none
*******************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	//App_Printf("������������fifo0\r\n");
}
/*******************************************************************************
  * @FunctionName: CAN1_RX0_IRQHandler
  * @Author:       trx
  * @DateTime:     2022��5��26�� 18:15:34 
  * @Purpose:      can�����жϺ������ײ���ṩ
  * @param:        void
  * @return:       none
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hCAN);				/* ��Ҫ���жϺ����е��ô˺���������жϱ�־λ */
	HAL_CAN_GetRxMessage(&hCAN, CAN_FILTER_FIFO0,&can_rx_msg,g_canrxbuf);
}


/*******************************************************************************
  * @FunctionName: bsp_Can1_Receive_buf
  * @Author:       trx
  * @DateTime:     2022��5��26�� 20:37:22 
  * @Purpose:      can���Ľ��պ�����Ŀǰֻ������һ��id�����Һ���fifo1�޷�����ʹ�ã�����fifo0��fifo1���޷�����������������
  * @param:        _id   ��Ҫ���ձ��ĵ�id
  * @param:        _buf[]��������������
  * @return:       DLC:���ر��ĵ����ݳ���
*******************************************************************************/
uint8_t bsp_Can1_Receive_buf(uint32_t _id,uint8_t _buf[])
{
	int i;
	if(IS_CAN_STDID(_id))
		{
			if(can_rx_msg.StdId == _id)
				{
					for(i = 0;i < can_rx_msg.DLC;i++)
						_buf[i] = g_canrxbuf[i];
				}
			else
				printf("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
		}
	else if(IS_CAN_EXTID(_id))
		{
			if(can_rx_msg.ExtId == _id)
				{
					for(i = 0;i < can_rx_msg.DLC;i++)
						{
							_buf[i] = g_canrxbuf[i];
						}
				}
			else				
				printf("Wrong parameters value: file %s on line %d\r\n",__FILE__,__LINE__);
		}
	return can_rx_msg.DLC;
}

