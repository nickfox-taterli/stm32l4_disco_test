/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

/* USER CODE BEGIN 0 */
/* Includes ------------------------------------------------------------------*/
/* LL drivers common to all LL examples */
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_gpio.h"
/* LL drivers specific to LL examples IPs */
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_comp.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_crc.h"
#include "stm32l4xx_ll_dac.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_lptim.h"
#include "stm32l4xx_ll_lpuart.h"
#include "stm32l4xx_ll_opamp.h"
#include "stm32l4xx_ll_rng.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_swpmi.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_wwdg.h"
/* USER CODE END 0 */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* External variables --------------------------------------------------------*/
#include "AudioPlayBack.h"

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1)
    {
    }
    /* USER CODE BEGIN HardFault_IRQn 1 */

    /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1)
    {
    }
    /* USER CODE BEGIN MemoryManagement_IRQn 1 */

    /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1)
    {
    }
    /* USER CODE BEGIN BusFault_IRQn 1 */

    /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1)
    {
    }
    /* USER CODE BEGIN UsageFault_IRQn 1 */

    /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/* DMA2_CH7 - REQUEST 3 => QSPI */
void DMA2_Channel7_IRQHandler(void)
{
    if(LL_DMA_GetPeriphRequest(DMA2, LL_DMA_CHANNEL_7) == LL_DMA_REQUEST_3)
    {
        if(LL_DMA_IsActiveFlag_TC7(DMA2) && LL_DMA_IsEnabledIT_TC(DMA2, LL_DMA_CHANNEL_7))
        {
            LL_DMA_ClearFlag_TC7(DMA2);
            LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_7);

            /* 此处执行中断回传. */
        }
        if(LL_DMA_IsActiveFlag_HT7(DMA2) && LL_DMA_IsEnabledIT_HT(DMA2, LL_DMA_CHANNEL_7))
        {
            LL_DMA_ClearFlag_HT7(DMA2);
            /* 此处执行中断回传. */
        }
        if(LL_DMA_IsActiveFlag_TE7(DMA2) && LL_DMA_IsEnabledIT_TE(DMA2, LL_DMA_CHANNEL_7))
        {
            LL_DMA_ClearFlag_TE7(DMA2);
            LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_7);
            /* 此处执行中断回传. */
        }

        LL_DMA_ClearFlag_GI7(DMA2);
    }
}

/* DMA2_CH1 - REQUEST 1 => SAI */
void DMA2_Channel1_IRQHandler(void)
{
    if(LL_DMA_GetPeriphRequest(DMA2, LL_DMA_CHANNEL_1) == LL_DMA_REQUEST_1)
    {
        if(LL_DMA_IsActiveFlag_TC1(DMA2))
        {
            LL_DMA_ClearFlag_TC1(DMA2);

            /* Call function Transmission complete Callback */

        }
        if(LL_DMA_IsActiveFlag_HT1(DMA2))
        {
            LL_DMA_ClearFlag_HT1(DMA2);

            /* Call function Transmission complete Callback */
        }
        else if(LL_DMA_IsActiveFlag_TE1(DMA2))
        {
            LL_DMA_ClearFlag_TE1(DMA2);
            /* Call Error function */
        }
        LL_DMA_ClearFlag_GI1(DMA2);
    }

}

void DMA1_Channel4_IRQHandler(void)
{
    static BaseType_t xHigherPriorityTaskWoken;

    if(LL_DMA_GetPeriphRequest(DMA1, LL_DMA_CHANNEL_4) == LL_DMA_REQUEST_0)
    {
        if(LL_DMA_IsActiveFlag_TC4(DMA1))
        {
            LL_DMA_ClearFlag_TC4(DMA1);
            xSemaphoreGiveFromISR( DmaRecBuffCplt, &xHigherPriorityTaskWoken );

        }
        if(LL_DMA_IsActiveFlag_HT4(DMA1))
        {
            LL_DMA_ClearFlag_HT4(DMA1);
            xSemaphoreGiveFromISR( DmaRecHalfBuffCplt, &xHigherPriorityTaskWoken );
        }
        else if(LL_DMA_IsActiveFlag_TE4(DMA1))
        {
            LL_DMA_ClearFlag_TE4(DMA1);
            /* Call Error function */
        }
        LL_DMA_ClearFlag_GI4(DMA1);
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
