#include "stm32l4xx.h"

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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "QSPI.h"
#include "N25Q128.h"

void SystemClock_Config(void)
{

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
    {

    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    LL_RCC_MSI_Enable();

    while(LL_RCC_MSI_IsReady() != 1)
    {

    }
    LL_RCC_MSI_EnableRangeSelection();

    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);

    LL_RCC_MSI_SetCalibTrimming(0);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);

    LL_RCC_PLL_EnableDomain_SYS();

    LL_RCC_PLL_Enable();

    while(LL_RCC_PLL_IsReady() != 1)
    {

    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {

    }
    LL_Init1msTick(80000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(80000000);

    NVIC_SetPriority(SysTick_IRQn, 0x0F);
}

/* Buffer used for tx */
uint8_t aTxBuffer[0x100];
/* Buffer used for rx */
uint8_t aRxBuffer[0x100];
/* Buffer fill pointer */
uint16_t i = 0;
/* Buffer index pointer */
uint16_t v = 0;

BSP_QSPI_ID_TypeDef pID;


void StartDefaultTask(void)
{
    BSP_QSPI_Init();
    BSP_QSPI_RDID(&pID); /* N25Q128 has unique id. */

    for(;;)
    {

        v++;

        for(i = 0; i < 0x100; i++)
        {
            aTxBuffer[i] = v + i;
            aRxBuffer[i] = 0xAA;
        }

        BSP_QSPI_Erase_Block(0x00000000);
        BSP_QSPI_Write(aTxBuffer, 0x00000000, 0x100);
        BSP_QSPI_Read(aRxBuffer, 0x00000000, 0x100);

        vTaskDelay(1000);
    }
}

int main(void)
{

    SystemClock_Config();

    xTaskCreate((TaskFunction_t)StartDefaultTask, "StartDefaultTask", 128, NULL, 0, NULL);

    vTaskStartScheduler();

    while (1)
    {

    }

}

