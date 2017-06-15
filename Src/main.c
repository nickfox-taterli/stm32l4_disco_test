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
#include "I2C1.h"
#include "SAI1.h"

#include "N25Q128.h"
#include "CS43L22.h"

void SystemClock_Config(void)
{

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
    {

    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    LL_RCC_MSI_Enable();

    /* Wait till MSI is ready */
    while(LL_RCC_MSI_IsReady() != 1)
    {

    }
    LL_RCC_MSI_EnableRangeSelection();

    LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);

    LL_RCC_MSI_SetCalibTrimming(0);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);

    LL_RCC_PLL_EnableDomain_SYS();

    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while(LL_RCC_PLL_IsReady() != 1)
    {

    }
    LL_RCC_PLLSAI1_ConfigDomain_SAI(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 86, LL_RCC_PLLSAI1P_DIV_7);

    LL_RCC_PLLSAI1_EnableDomain_SAI();

    LL_RCC_PLLSAI1_Enable();

    /* Wait till PLLSAI1 is ready */
    while(LL_RCC_PLLSAI1_IsReady() != 1)
    {

    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {

    }
    LL_Init1msTick(80000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(80000000);

    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);

    LL_RCC_SetSAIClockSource(LL_RCC_SAI1_CLKSOURCE_PLLSAI1);

    /* SysTick_IRQn interrupt configuration */
    NVIC_SetPriority(SysTick_IRQn, 0x0F);
}


#define AUDIO_FILE_ADDRESS   0x90000000
#define AUDIO_FILE_SIZE      (6144 *1024)
#define PLAY_BUFF_SIZE       4096

BSP_QSPI_ID_TypeDef pID;
uint8_t cs43l22_id;
uint16_t PlayBuff[4096];
__IO int32_t UpdatePointer = -1;
uint32_t PlaybackPosition = PLAY_BUFF_SIZE;


void DMA2_Channel1_IRQHandler(void)
{
    if(LL_DMA_IsActiveFlag_TC1(DMA2))
    {
        LL_DMA_ClearFlag_TC1(DMA2);
        UpdatePointer = PLAY_BUFF_SIZE / 2;
        /* Call function Transmission complete Callback */ /* 传输完剩下一半,这时候可以填充后面的数据. */

    }
    if(LL_DMA_IsActiveFlag_HT1(DMA2))
    {
        LL_DMA_ClearFlag_HT1(DMA2);
        UpdatePointer = 0;
        /* Call function Transmission complete Callback */ /* 传输完前面一半,这时候可以填充前面的数据. */
    }
    else if(LL_DMA_IsActiveFlag_TE1(DMA2))
    {
        /* Call Error function */
    }

    LL_DMA_ClearFlag_GI1(DMA2);
}

void StartDefaultTask(void)
{
    I2C1_Init();

    BSP_QSPI_Init();
    BSP_QSPI_RDID(&pID); /* N25Q128 has unique id. */
    BSP_QSPI_EnableMemoryMappedMode();
    cs43l22_id = CS43L22_Init(80);
    SAI1_MspInit((uint8_t *)PlayBuff, 4096, SAI_AUDIO_FREQUENCY_8K);
    CS43L22_Play(4096);


    for(;;)
    {
        /* Wait a callback event */
        while(UpdatePointer == -1);

        int position = UpdatePointer;
        UpdatePointer = -1;

        /* Upate the first or the second part of the buffer */
        for(int i = 0; i < PLAY_BUFF_SIZE / 2; i++)
        {
            PlayBuff[i + position] = *(uint16_t *)(AUDIO_FILE_ADDRESS + PlaybackPosition);
            PlaybackPosition += 2;
        }

        /* check the end of the file */
        if((PlaybackPosition + PLAY_BUFF_SIZE / 2) > AUDIO_FILE_SIZE)
        {
            PlaybackPosition = 0;
        }

        if(UpdatePointer != -1)
        {
						vTaskDelay(1);/* Error */
        }
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

