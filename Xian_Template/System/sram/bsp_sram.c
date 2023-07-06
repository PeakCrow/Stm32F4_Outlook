#include "bsp_sram.h"


SRAM_HandleTypeDef SRAM_Handler;    //SRAM���

//SRAM��ʼ??
void bsp_InitSram(void)
{	
	GPIO_InitTypeDef GPIO_Initure;
	FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;

	__HAL_RCC_FSMC_CLK_ENABLE();                //???FSMC???
	__HAL_RCC_GPIOD_CLK_ENABLE();               //???GPIOD???
	__HAL_RCC_GPIOE_CLK_ENABLE();               //???GPIOE???
	__HAL_RCC_GPIOF_CLK_ENABLE();               //???GPIOF???
	__HAL_RCC_GPIOG_CLK_ENABLE();               //???GPIOG???
    
	//PD0,1,4,5,8~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_8|\
					 GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|\
					 GPIO_PIN_14|GPIO_PIN_15;              
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //��??
    GPIO_Initure.Alternate=GPIO_AF12_FSMC;     	//����ΪFSMC    
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);     	
    
	//PE0,1,7~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|\
					 GPIO_PIN_10| GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|\
					 GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);    
    
	//PF0~5,12~15
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|\
					 GPIO_PIN_5|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;              
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);     
    
	//PG0~5,10
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10;              
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);      
	
	SRAM_Handler.Instance=FSMC_NORSRAM_DEVICE;                
	SRAM_Handler.Extended=FSMC_NORSRAM_EXTENDED_DEVICE;    
    
	SRAM_Handler.Init.NSBank=FSMC_NORSRAM_BANK3;     					//ʹ��NE3
	SRAM_Handler.Init.DataAddressMux=FSMC_DATA_ADDRESS_MUX_DISABLE; 	//��ַ/�����߲�����
	SRAM_Handler.Init.MemoryType=FSMC_MEMORY_TYPE_SRAM;   				//SRAM
	SRAM_Handler.Init.MemoryDataWidth=FSMC_NORSRAM_MEM_BUS_WIDTH_16; 	//16λ���ݿ�??
	SRAM_Handler.Init.BurstAccessMode=FSMC_BURST_ACCESS_MODE_DISABLE; 	//�Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����??�˴�δ��??
	SRAM_Handler.Init.WaitSignalPolarity=FSMC_WAIT_SIGNAL_POLARITY_LOW;	//�ȴ��źŵļ�??����ͻ��ģʽ��������??
	SRAM_Handler.Init.WaitSignalActive=FSMC_WAIT_TIMING_BEFORE_WS;   	//�洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT
	SRAM_Handler.Init.WriteOperation=FSMC_WRITE_OPERATION_ENABLE;    	//�洢��дʹ��
	SRAM_Handler.Init.WaitSignal=FSMC_WAIT_SIGNAL_DISABLE;           	//�ȴ�ʹ��??�˴�δ��??
	SRAM_Handler.Init.ExtendedMode=FSMC_EXTENDED_MODE_DISABLE;        	//��дʹ����ͬ��ʱ??
	SRAM_Handler.Init.AsynchronousWait=FSMC_ASYNCHRONOUS_WAIT_DISABLE;	//�Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ��??
	SRAM_Handler.Init.WriteBurst=FSMC_WRITE_BURST_DISABLE;           	//��ֹͻ��??
	SRAM_Handler.Init.ContinuousClock=FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;
    
	//FMC��ʱ����ƼĴ���
	FSMC_ReadWriteTim.AddressSetupTime=0x00;       	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/168M=6ns*16=96ns
	FSMC_ReadWriteTim.AddressHoldTime=0x00;			//��ַ����ʱ�䣨ADDHLD��ģʽAδ��??
	FSMC_ReadWriteTim.DataSetupTime=0x06;			//���ݱ���ʱ��??��HCLK	=6*1=6ns
	FSMC_ReadWriteTim.BusTurnAroundDuration=0X00;
	FSMC_ReadWriteTim.AccessMode=FSMC_ACCESS_MODE_A;//ģʽA
	HAL_SRAM_Init(&SRAM_Handler,&FSMC_ReadWriteTim,&FSMC_ReadWriteTim);	

}

//��ָ����ַ(WriteAddr+Bank1_SRAM3_ADDR)��??����д��n����??
//pBuffer:�ֽ�ָ��
//WriteAddr:Ҫд��ĵ�ַ
//n:Ҫд����ֽ�??
void FSMC_SRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n)
{
	for(;n!=0;n--)
	{
		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
}

//��ָ����ַ((WriteAddr+Bank1_SRAM3_ADDR))��??��������n����??
//pBuffer:�ֽ�ָ��
//ReadAddr:Ҫ��������ʼ��ַ
//n:Ҫд����ֽ�??
void FSMC_SRAM_ReadBuffer(u8 *pBuffer,u32 ReadAddr,u32 n)
{
	for(;n!=0;n--)
	{
		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);
		ReadAddr++;
	}
}
