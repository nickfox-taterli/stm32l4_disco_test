#include "RCC.h"

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
