#include "SAI1.h"

void SAI1_MspInit(uint8_t *pData,
                  uint16_t PlayBufSize,
                  uint32_t AudioFrequency)
{

    uint16_t Mckdiv = 0;
    uint8_t *pBuffPtr = pData;

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SAI1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_2, LL_GPIO_AF_13);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_4, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_4, LL_GPIO_AF_13);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_5, LL_GPIO_AF_13);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_5, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOE, LL_GPIO_PIN_6, LL_GPIO_AF_13);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);

    /* Disable the SAI peripheral */
    SAI1_Block_A->CR1 &=  ~SAI_xCR1_SAIEN;
    while((SAI1_Block_A->CR1 & SAI_xCR1_SAIEN)) {}

    SAI1->GCR = 0x00000000;

    /*
    	 Configure Master Clock using the following formula :
    	 MCLK_x = SAI_CK_x / (MCKDIV[3:0] * 2) with MCLK_x = 256 * FS
    	 FS = SAI_CK_x / (MCKDIV[3:0] * 2) * 256
    	 MCKDIV[3:0] = SAI_CK_x / FS * 512
    	 SAI_CK_x = 49142857 Hz
    */


    switch(AudioFrequency)
    {
    case SAI_AUDIO_FREQUENCY_192K: /* Real 191.964kHz offset = -0.01% */
        Mckdiv = 0;
        break;
    case SAI_AUDIO_FREQUENCY_96K: /* Real 95.982kHz offset = -0.01% */
        Mckdiv = 1;
        break;
    case SAI_AUDIO_FREQUENCY_48K: /* Real 47.991kHz offset = -0.01% */
        Mckdiv = 2;
        break;
        //			case SAI_AUDIO_FREQUENCY_44K: /* Real 47.991kHz offset = -0.01% */
        //				Mckdiv = 2;
        //				break;
    case SAI_AUDIO_FREQUENCY_32K: /* Real 31.994kHz offset = -0.01% */
        Mckdiv = 3;
        break;
        //			case SAI_AUDIO_FREQUENCY_22K: /* Real 31.994kHz offset > 10% */
        //				Mckdiv = 4;
        //				break;
    case SAI_AUDIO_FREQUENCY_16K: /* Real 15.997kHz offset = -0.01% */
        Mckdiv = 6;
        break;
        //			case SAI_AUDIO_FREQUENCY_11K: /* Real 15.997kHz offset > 10% */
        //				Mckdiv = 8;
        //				break;
    case SAI_AUDIO_FREQUENCY_8K: /* Real 7.998kHz offset = -0.02% */
        Mckdiv = 12;
        break;
    default:
        return;
    }

    SAI1_Block_A->CR1 = 0x00002280 | (Mckdiv << 20);
    SAI1_Block_A->CR2 =  0x00000001;
    /* SAI Frame Configuration -----------------------------------------*/
    SAI1_Block_A->FRCR = 0x00050F1F;

    /* SAI Block_x SLOT Configuration ------------------------------------------*/
    SAI1_Block_A->SLOTR = 0x00030100;

    LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_SetPeriphRequest(DMA2, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_1);
    LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA2, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_HALFWORD);

    LL_DMA_ClearFlag_TE1(DMA2);
    LL_DMA_ClearFlag_HT1(DMA2);
    LL_DMA_ClearFlag_TC1(DMA2);
    LL_DMA_ClearFlag_GI1(DMA2);

    LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_1, PlayBufSize);

    /* Configure DMA Channel destination address */
    LL_DMA_SetPeriphAddress(DMA2, LL_DMA_CHANNEL_1, (uint32_t)&SAI1_Block_A->DR);
    /* Configure DMA Channel source address */
    LL_DMA_SetMemoryAddress(DMA2, LL_DMA_CHANNEL_1, (uint32_t)pBuffPtr);


    LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TE(DMA2, LL_DMA_CHANNEL_1);

    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_1);

    SAI1_Block_A->CR1 |=  SAI_xCR1_SAIEN;
    SAI1_Block_A->CR1 |= SAI_xCR1_DMAEN;

    NVIC_SetPriority(DMA2_Channel1_IRQn, 0x0F);
    NVIC_EnableIRQ(DMA2_Channel1_IRQn);

}
