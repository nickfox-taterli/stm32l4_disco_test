#include "DFSDM.h"
#include "SAI1.h"

void DFSDM_Init(uint32_t *pData,
                uint16_t PlayBufSize,
                uint32_t AudioFrequency)
{
    uint8_t *pBuffPtr = (uint8_t *)pData;

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DFSDM1);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_9, LL_GPIO_AF_6);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_9, LL_GPIO_PULL_DOWN);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_7, LL_GPIO_AF_6);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);

    if(AudioFrequency == SAI_AUDIO_FREQUENCY_44K || AudioFrequency == SAI_AUDIO_FREQUENCY_22K || AudioFrequency == SAI_AUDIO_FREQUENCY_11K)
    {
        /* SAI_CK_x = 11294117 Hz */
        LL_RCC_PLLSAI1_Disable();
        LL_RCC_PLLSAI1_DisableDomain_SAI();
        LL_RCC_PLLSAI1_ConfigDomain_SAI(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 48, LL_RCC_PLLSAI1P_DIV_17);
        LL_RCC_PLLSAI1_EnableDomain_SAI();
        LL_RCC_PLLSAI1_Enable();

    }
    else
    {
        /* SAI_CK_x = 49142857 Hz */
        LL_RCC_PLLSAI1_Disable();
        LL_RCC_PLLSAI1_DisableDomain_SAI();
        LL_RCC_PLLSAI1_ConfigDomain_SAI(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 86, LL_RCC_PLLSAI1P_DIV_7);
        LL_RCC_PLLSAI1_EnableDomain_SAI();
        LL_RCC_PLLSAI1_Enable();
    }

    DFSDM1_Channel0->CHCFGR1 = 0xC0000000 | (uint32_t) ((DFSDMClockDivider(AudioFrequency) - 1) << 16);
    DFSDM1_Channel2->CHCFGR1 = 0x00000004;
    DFSDM1_Channel2->CHCFGR2 = (DFSDMRightBitShift(AudioFrequency) << 3);
    DFSDM1_Channel2->CHAWSCDR = 0x00090000;
    DFSDM1_Channel2->CHCFGR1 |= DFSDM_CHCFGR1_CHEN;

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMA_REQUEST_0);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_WORD);

    LL_DMA_ClearFlag_TE4(DMA1);
    LL_DMA_ClearFlag_HT4(DMA1);
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_ClearFlag_GI4(DMA1);

    DFSDM1_Filter0->FLTFCR = (DFSDMFilterOrder(AudioFrequency)) | ((DFSDMOverSampling(AudioFrequency) - 1) << 16);
    DFSDM1_Filter0->FLTCR1 = 0x22240011;

    LL_DMA_ClearFlag_TE4(DMA1);
    LL_DMA_ClearFlag_HT4(DMA1);
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_ClearFlag_GI4(DMA1);

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, PlayBufSize);

    /* Configure DMA Channel destination address */
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)&DFSDM1_Filter0->FLTRDATAR);
    /* Configure DMA Channel source address */
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)pBuffPtr);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_4);
    LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);

    /* Start regular conversion */
    DFSDM1_Filter0->FLTCR1 |= DFSDM_FLTCR1_RSWSTART;

    NVIC_SetPriority(DMA1_Channel4_IRQn, 0x0E);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}
