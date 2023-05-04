/**
  ******************************************************************************
  * @file CAN/Normal/platform_config.h 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Evaluation board specific configuration file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line corresponding to the STMicroelectronics evaluation board
   used to run the example */
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)
 //#define USE_STM3210B_EVAL
 #define USE_STM3210E_EVAL
#endif

/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM3210B_EVAL
  #define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOD
  #define RCC_APB2Periph_GPIO_LED    RCC_APB2Periph_GPIOC
  #define RCC_APB2Periph_GPIO_Key    RCC_APB2Periph_GPIOB
  #define GPIO_Remap_CAN             GPIO_Remap2_CAN1
  #define GPIO_LED                   GPIOC  
  #define GPIO_CAN                   GPIOD  
  #define GPIO_Key                   GPIOB
  #define GPIO_Pin_RX                GPIO_Pin_0
  #define GPIO_Pin_TX                GPIO_Pin_1
  #define GPIO_Pin_Key               GPIO_Pin_9

#elif defined USE_STM3210E_EVAL

  #define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA   //zxb
  #define RCC_APB2Periph_GPIO_LED    RCC_APB2Periph_GPIOF
  #define RCC_APB2Periph_GPIO_Key    RCC_APB2Periph_GPIOG
  #define GPIO_Remap_CAN             GPIO_Remap1_CAN1
  #define GPIO_LED                   GPIOF    
  #define GPIO_CAN                   GPIOA
  #define GPIO_Key                   GPIOG
  #define GPIO_Pin_RX                GPIO_Pin_8
  #define GPIO_Pin_TX                GPIO_Pin_9
  #define GPIO_Pin_Key               GPIO_Pin_8
#endif /* USE_STM3210B_EVAL */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
