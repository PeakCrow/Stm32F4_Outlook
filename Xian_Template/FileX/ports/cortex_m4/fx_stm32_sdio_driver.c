/**************************************************************************/
/*                                                                        */
/*       Partial Copyright (c) Microsoft Corporation. All rights reserved.*/
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*      Partial Copyright (c) STMicroelctronics 2020. All rights reserved */
/**************************************************************************/

/* Include necessary system files.  */
#include "sys.h"
#include "fx_stm32_sd_driver.h"

extern SD_HandleTypeDef uSdHandle;

UINT  _fx_partition_offset_calculate(void  *partition_sector, UINT partition, ULONG *partition_start, ULONG *partition_size);

static UINT sd_read_data(FX_MEDIA *media_ptr, ULONG sector, UINT num_sectors, UINT use_scratch_buffer);
static UINT sd_write_data(FX_MEDIA *media_ptr, ULONG sector, UINT num_sectors, UINT use_scratch_buffer);

static TX_SEMAPHORE transfer_semaphore;

static uint8_t is_initialized = 0;

#define BSP_ERROR_NONE                    0
#define BSP_ERROR_BUSY                   -3

static int32_t check_sd_status()
{
    uint32_t start = tx_time_get();

    while (tx_time_get() - start < DEFAULT_TIMEOUT)
    {
      if (BSP_SD_GetCardState() == SD_TRANSFER_OK)
      {
        return BSP_ERROR_NONE;
      }
    }

    return BSP_ERROR_BUSY;
}

/**
  * @brief This function is the entry point to the STM32 SDIO disk driver.     */
 /*        It relies on the STM32 peripheral library from ST.
  * @param None
  * @retval None
  */
VOID  fx_stm32_sd_driver(FX_MEDIA *media_ptr)
{
    int32_t status;
    ULONG       partition_start;
    ULONG       partition_size;

#if (FX_DRIVER_CALLS_BSP_SD_INIT == 0)
    is_initialized = 1; /* the SD  was initialized by the application*/
#endif
//TX_INTERRUPT_SAVE_AREA
   /* before performing any operation, check the status of the SDMMC */
    if (is_initialized == 1)
    {
      if (check_sd_status() != BSP_ERROR_NONE)
      {
          media_ptr->fx_media_driver_status =  FX_IO_ERROR;
          return;
      }
    }

    /* Process the driver request specified in the media control block.  */
    switch(media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
#if (FX_DRIVER_CALLS_BSP_SD_INIT == 1)
            /* Initialize the SD instance */
            if (is_initialized == 0)
            {
                status = BSP_SD_Init();

                if (status == BSP_ERROR_NONE)
                {
                    is_initialized = 1;
#endif
                    /* create a binary semaphore to check the DMA transfer status */
                    if (tx_semaphore_create(&transfer_semaphore, "sdmmc dma transfer semaphore", 0) != TX_SUCCESS)
                    {
                        media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                    }
                    else
                    {
                        media_ptr->fx_media_driver_status =  FX_SUCCESS;
                    }
#if (FX_DRIVER_CALLS_BSP_SD_INIT == 1)
                }
                else
                {
                    media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                }
            }
#endif
            break;
        }

        case FX_DRIVER_UNINIT:
        {
            tx_semaphore_delete(&transfer_semaphore);

#if (FX_DRIVER_CALLS_BSP_SD_INIT == 1)
            BSP_SD_DeInit();
            is_initialized = 0;
#endif
            /* Successful driver request.  */
           media_ptr->fx_media_driver_status = FX_SUCCESS;
           break;
        }

        case FX_DRIVER_READ:
        {
            media_ptr->fx_media_driver_status = FX_IO_ERROR;
            //TX_DISABLE /* disable interrupts */
           if ((ULONG)(media_ptr->fx_media_driver_buffer) & 0x3)
           {
              if (sd_read_data(media_ptr, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,
                                media_ptr->fx_media_driver_sectors, 1) == FX_SUCCESS)
              {
                  media_ptr->fx_media_driver_status = FX_SUCCESS;
              }
           }
           else
           {
               if (sd_read_data(media_ptr, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,
                       media_ptr->fx_media_driver_sectors, 0) == FX_SUCCESS)
               {
                   media_ptr->fx_media_driver_status = FX_SUCCESS;
               }
           }
           //TX_RESTORE /* restore interrupts */

            break;
        }

        case FX_DRIVER_WRITE:
        {
            media_ptr->fx_media_driver_status = FX_IO_ERROR;
            //TX_DISABLE /* disable interrupts */

           if (sd_write_data(media_ptr, media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,
                   media_ptr->fx_media_driver_sectors, 0) == FX_SUCCESS)
           {
               media_ptr->fx_media_driver_status = FX_SUCCESS;
           }
           
           //TX_RESTORE /* restore interrupts */

            break;
        }

        case FX_DRIVER_FLUSH:
        {
            /* Return driver success.  */
           media_ptr->fx_media_driver_status =  FX_SUCCESS;
            break;
        }

        case FX_DRIVER_ABORT:
        {
            /* Return driver success.  */
           media_ptr->fx_media_driver_status =  FX_SUCCESS;
            break;
        }

        case FX_DRIVER_BOOT_READ:
        {

            /* the boot sector is the sector 0 */
            status = BSP_SD_ReadBlocks_DMA((uint32_t*)media_ptr->fx_media_driver_buffer, 0, 1);

            if (status != BSP_ERROR_NONE)
            {
                media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                break;
            }

            if(tx_semaphore_get(&transfer_semaphore, DEFAULT_TIMEOUT) != TX_SUCCESS)
            {
                media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                break;
            }

            /* Check if the sector 0 is the actual boot sector, otherwise calculate the offset into it.
            Please note that this should belong to higher level of MW to do this check and it is here
            as a temporary work solution */

            partition_start =  0;

            status =  _fx_partition_offset_calculate(media_ptr -> fx_media_driver_buffer, 0,
                                                                &partition_start, &partition_size);

            /* Check partition read error.  */
            if (status)
            {
                /* Unsuccessful driver request.  */
                media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
                return;
            }

            /* Now determine if there is a partition...   */
            if (partition_start)
            {

                if (check_sd_status() != BSP_ERROR_NONE)
                {
                    media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                    return;
                }

                /* Yes, now lets read the actual boot record.  */
                status = BSP_SD_ReadBlocks_DMA((uint32_t*)media_ptr -> fx_media_driver_buffer, partition_start , 1);

                /* Check status of SDIO Read.  */
                if (status != BSP_ERROR_NONE)
                {

                    /* Unsuccessful driver request.  */
                    media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
                    return;
                }

                /* Wait for Rx Transfer completion */
                if(tx_semaphore_get(&transfer_semaphore, DEFAULT_TIMEOUT) != TX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                    break;
                }
            }

            /* Successful driver request.  */
            media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;
        }

        case FX_DRIVER_BOOT_WRITE:
        {
            status = BSP_SD_WriteBlocks_DMA((uint32_t*)media_ptr->fx_media_driver_buffer, 0, 1);
            if (status == BSP_ERROR_NONE)
            {
                if(tx_semaphore_get(&transfer_semaphore, DEFAULT_TIMEOUT) == TX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status =  FX_SUCCESS;
                }
                else
                {
                    media_ptr->fx_media_driver_status =  FX_IO_ERROR;
                }
            }
            break;
        }

        default:
        {
           media_ptr->fx_media_driver_status =  FX_IO_ERROR;
            break;
        }
    }
}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @param Instance the SD instance
  * @retval None
  */
void BSP_SD_WriteCpltCallback()
{
    tx_semaphore_put(&transfer_semaphore);
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @param Instance the sd instance
  * @retval None
  */
void BSP_SD_ReadCpltCallback()
{
    tx_semaphore_put(&transfer_semaphore);
}

/**
  * @brief Read buffer using BSP SD API taking into account the scratch buffer
  * @param FX_MEDIA *media_ptr a pointer the main FileX structure
  * @param ULONG start_sector first sector to start reading from
  * @param UINT num_sectors number of sectors to be read
  * @param UINT use_scratch_buffer to enable scratch buffer usage or not.
  * @retval FX_SUCCESS on success FX_BUFFER_ERROR otherwise
  */

static UINT sd_read_data(FX_MEDIA *media_ptr, ULONG start_sector, UINT num_sectors, UINT use_scratch_buffer)
{
    UINT status;
   
    status = BSP_SD_ReadBlocks_DMA((uint32_t*)media_ptr->fx_media_driver_buffer, start_sector, num_sectors);

    if (status == BSP_ERROR_NONE)
    {
        if(tx_semaphore_get(&transfer_semaphore, DEFAULT_TIMEOUT) == TX_SUCCESS)
        {
            status = FX_SUCCESS;
        }
        else
        {
            status =  FX_BUFFER_ERROR;
        }
    }

    return status;
}

/**
  * @brief write buffer using BSP SD API taking into account the scratch buffer
  * @param FX_MEDIA *media_ptr a pointer the main FileX structure
  * @param ULONG start_sector first sector to start writing from
  * @param UINT num_sectors number of sectors to be written
  * @param UINT use_scratch_buffer to enable scratch buffer usage or not.
  * @retval FX_SUCCESS on success FX_BUFFER_ERROR otherwise
  */

static UINT sd_write_data(FX_MEDIA *media_ptr, ULONG start_sector, UINT num_sectors, UINT use_scratch_buffer)
{
    UINT status;

    status = BSP_SD_WriteBlocks_DMA((uint32_t*)media_ptr->fx_media_driver_buffer, start_sector, num_sectors);

    if (status == BSP_ERROR_NONE)
    {
        if(tx_semaphore_get(&transfer_semaphore, DEFAULT_TIMEOUT) == TX_SUCCESS)
        {
            status = FX_SUCCESS;
        }
        else
        {
            status = FX_IO_ERROR;
        }
    }

    return status;
}

void SDIO_IRQHandler(void)
{
	HAL_SD_IRQHandler(&uSdHandle);
}

void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(uSdHandle.hdmatx);
}

void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}
