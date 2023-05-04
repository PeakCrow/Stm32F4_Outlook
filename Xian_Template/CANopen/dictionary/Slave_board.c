/**
  ******************************************************************************
  * @file CAN/Normal/main.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/

//① 映射关系
//② TPDO类型 SYNC 修改发送间隔
//③ 利用SDO修改TPDO的对象字典

#include "stm32f10x.h"
#include "platform_config.h"

/* test_master */
#include "canfestival.h"
#include "Master.h"
#include "Slave.h"
#include "TestMasterSlave.h"



/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup CAN_Normal
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;
CanRxMsg RxMessage;


static void CAN_Config(void);
static int  test_slave(void);
unsigned char deviceid=0;


extern void TIM5_start(void);

Message m;

unsigned char CanreveiveFlag=0;



/* Private functions ---------------------------------------------------------*/

/**
  * @brief  can_slave_thread
  * @param  None
  * @retval : None
  */
void can_slave_init(void)
{

/* CAN configuration */
    CAN_Config();
    
    
}

/**
  * @brief  can_slave_thread
  * @param  None
  * @retval : None
  */
void can_recv_thread(void)
{
	
	int i = 0;
	can_slave_init();
  test_slave();


}




/**
  * @brief  Configures the CAN.
  * @param  None
  * @retval : None
  */
void CAN_Config(void)
{
	CAN_DeInit(CAN1);
    CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_5tq,CAN_BS1_12tq,2,CAN_Mode_Normal);  
	   /* transmit */
    //TxMessage.StdId = 0x321;
	  TxMessage.StdId = 0x333;
    TxMessage.ExtId = 0x00;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.IDE = CAN_ID_STD;
    //TxMessage.DLC = 1;
	  TxMessage.DLC = 2;
}

#include <applicfg.h>
unsigned char canSend(CAN_PORT notused, Message *m)
{
	
	uint32_t	i;
	CanTxMsg *ptx_msg;//zxb
	ptx_msg = (CanTxMsg *)malloc(sizeof(CanTxMsg));//zxb add this
	
   ptx_msg->StdId = m->cob_id;
	//ptx_msg->StdId = 0X0701;

	if(m->rtr)
  		ptx_msg->RTR = CAN_RTR_REMOTE;
	else
		ptx_msg->RTR = CAN_RTR_DATA;

  	ptx_msg->IDE = CAN_ID_STD;
  	ptx_msg->DLC = m->len;
	for(i = 0; i < m->len; i++)
		ptx_msg->Data[i] = m->data[i];

	MSG_WAR(0x00000,"ptx_msg->Data[0] is ", ptx_msg->Data[0]);
	MSG_WAR(0x00000,"ptx_msg->Data[1] is ", ptx_msg->Data[1]);
	MSG_WAR(0x00000,"ptx_msg->Data[2] is ", ptx_msg->Data[2]);
	MSG_WAR(0x00000,"ptx_msg->Data[3] is ", ptx_msg->Data[3]);
	MSG_WAR(0x00000,"ptx_msg->Data[4] is ", ptx_msg->Data[4]);
	MSG_WAR(0x00000,"ptx_msg->Data[5] is ", ptx_msg->Data[5]);
	MSG_WAR(0x00000,"ptx_msg->Data[6] is ", ptx_msg->Data[6]);
	MSG_WAR(0x00000,"ptx_msg->Data[7] is ", ptx_msg->Data[7]);
	//*(TestSlave_Data.bDeviceNodeId) = 1;
	
	
    if( CAN_Transmit( CAN1, ptx_msg )==CAN_TxStatus_NoMailBox)
    {
         free(ptx_msg);
        return 0xff;
    }
    else
    {
         free(ptx_msg);
        return 0x00;
	}
	

    
}

unsigned char canReceive(Message *m) // 接收消息
/******************************************************************************
The driver pass a received CAN message to the stack
INPUT   Message *m pointer to received CAN message
OUTPUT  1 if a message received
******************************************************************************/
{
    u32 i = 0;
    CanRxMsg RxMessage;

    CAN_Receive( CAN1, CAN_FIFO0, &RxMessage );

    m->cob_id = (UNS16)RxMessage.StdId;        
    RxMessage.IDE   = CAN_ID_STD;      

    if( RxMessage.RTR == CAN_RTR_Remote )
    {
        m->rtr = 1; 
    }
    else
    {
        m->rtr = 0; 
    }

    m->len = RxMessage.DLC; 

    for( i=0; i<m->len; i++ )
    {
        m->data[i] = RxMessage.Data[i]; 
    }

    return TRUE; 
}


void USB_LP_CAN1_RX0_IRQHandler(void)
{
	__disable_irq();
    if( CAN_GetITStatus( CAN1, CAN_IT_FMP0 )== SET )
    {  
       
        CAN_ClearITPendingBit( CAN1, CAN_IT_FMP0 );

        while( CAN_MessagePending( CAN1, CAN_FIFO0 ) !=0 )
        {
            // a message was received pass it to the CANstack
           if(CanreveiveFlag==0)
           {               
                if( canReceive(&m) )
                {
									  //while(1) printf("is has receve the message........\r\n");
                    canDispatch(&TestSlave_Data, &m);//zxb
                    printf("%s(%d)TestSlave_Data->bDeviceNodeId is 0x[%x]\r\n ",__FILE__,__LINE__,*(TestSlave_Data.bDeviceNodeId));
                }
           }
        }   
    }

    if(CAN_GetITStatus( CAN1, CAN_IT_FF0) )
    {
        
        CAN_ClearITPendingBit(CAN1, CAN_IT_FF0 );
    }
    
    if(CAN_GetITStatus( CAN1, CAN_IT_FOV0) )
    {
        
        CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0 );
    }
    __enable_irq();
}

static TimerCallback_t init_callback;

s_BOARD SlaveBoard  = {"0", "1M"};

void help(void)
{
  eprintf("**************************************************************\n");
  eprintf("*  TestMasterSlave                                           *\n");
  eprintf("*                                                            *\n");
  eprintf("*  A simple example for PC. It does implement 2 CanOpen      *\n");
  eprintf("*  nodes in the same process. A master and a slave. Both     *\n");
  eprintf("*  communicate together, exchanging periodically NMT, SYNC,  *\n");
  eprintf("*  SDO and PDO. Master configure heartbeat producer time     *\n");
  eprintf("*  at 1000 ms for slave node-id 0x02 by concise DCF.         *\n");                                  
  eprintf("*                                                            *\n");
  eprintf("*   Usage:                                                   *\n");
  eprintf("*   ./TestMasterSlave  [OPTIONS]                             *\n");
  eprintf("*                                                            *\n");
  eprintf("*   OPTIONS:                                                 *\n");
  eprintf("*     -l : Can library [\"libcanfestival_can_virtual.so\"]     *\n");
  eprintf("*                                                            *\n");
  eprintf("*    Slave:                                                  *\n");
  eprintf("*     -s : bus name [\"0\"]                                    *\n");
  eprintf("*     -S : 1M,500K,250K,125K,100K,50K,20K,10K,none(disable)  *\n");
  eprintf("*                                                            *\n");
  eprintf("*    Master:                                                 *\n");
  eprintf("*     -m : bus name [\"1\"]                                    *\n");
  eprintf("*     -M : 1M,500K,250K,125K,100K,50K,20K,10K,none(disable)  *\n");
  eprintf("*                                                            *\n");
  eprintf("**************************************************************\n");
}



/***************************  INIT  *****************************************/
void InitNodes(CO_Data* d, UNS32 id)
{

	/****************************** INITIALISATION SLAVE *******************************/
	if(strcmp(SlaveBoard.baudrate, "none")) 
    {
		/*TestSlave.C是由专业工具生成的对象字典*/
		/*建立映射对应关系 
		     SDO(rx)：602 SDO(tx): 582 
		     TPDO: 182 282 382 482   
		     RPDO: 从节点没有对象字典对应接收PDO*/
		//unsigned char deviceid=0;
		setNodeId(&TestSlave_Data, id);  //set node id @bruce
        deviceid=*(TestSlave_Data.bDeviceNodeId);
		printf(" 11111111 deviceid is 0x[%x]\r\n",deviceid);
		/* init */
		setState(&TestSlave_Data, Initialisation);    //boot-up该节点 进入Pre-optional状态
		deviceid=*(TestSlave_Data.bDeviceNodeId);
		printf(" 22222222 deviceid is 0x[%x]\r\n",deviceid);
	}
}

static StartTimerLoop(TimerCallback_t _init_callback) 
{
	init_callback = _init_callback;

	SetAlarm(NULL, 0, init_callback, 0, 0);
	TIM5_start();
}

/***************************  EXIT  *****************************************/
void Exit(CO_Data* d, UNS32 id)
{

}

/****************************************************************************/
/*****************************  test_salve  *********************************/
/****************************************************************************/
static int test_slave(void)
{
	if(strcmp(SlaveBoard.baudrate, "none")){
		
		TestSlave_Data.heartbeatError = TestSlave_heartbeatError;
		TestSlave_Data.initialisation = TestSlave_initialisation;
		TestSlave_Data.preOperational = TestSlave_preOperational;
		TestSlave_Data.operational = TestSlave_operational;
		TestSlave_Data.stopped   = TestSlave_stopped;
		TestSlave_Data.post_sync = TestSlave_post_sync;
		TestSlave_Data.post_TPDO = TestSlave_post_TPDO;
		TestSlave_Data.storeODSubIndex = TestSlave_storeODSubIndex;
		TestSlave_Data.post_emcy = TestSlave_post_emcy;
	}
        
		// Start timer thread
		//StartTimerLoop(&InitNodes);

		InitNodes(&TestSlave_Data, 0x2);

	return 0;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
