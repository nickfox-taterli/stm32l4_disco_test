#ifndef __DISCO_DFSDM_H
#define __DISCO_DFSDM_H

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

/** @defgroup DFSDM_Filter_SincOrder DFSDM filter sinc order
  * @{
  */
#define DFSDM_FILTER_FASTSINC_ORDER ((uint32_t)0x00000000U)               /*!< FastSinc filter type */
#define DFSDM_FILTER_SINC1_ORDER    DFSDM_FLTFCR_FORD_0                         /*!< Sinc 1 filter type */
#define DFSDM_FILTER_SINC2_ORDER    DFSDM_FLTFCR_FORD_1                         /*!< Sinc 2 filter type */
#define DFSDM_FILTER_SINC3_ORDER    (DFSDM_FLTFCR_FORD_0 | DFSDM_FLTFCR_FORD_1) /*!< Sinc 3 filter type */
#define DFSDM_FILTER_SINC4_ORDER    DFSDM_FLTFCR_FORD_2                         /*!< Sinc 4 filter type */
#define DFSDM_FILTER_SINC5_ORDER    (DFSDM_FLTFCR_FORD_0 | DFSDM_FLTFCR_FORD_2) /*!< Sinc 5 filter type */


/*### RECORD ###*/
#define DFSDMOverSampling(__FREQUENCY__) \
        (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_8K)  ? 256 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_11K) ? 256 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_16K) ? 128 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_22K) ? 128 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_32K) ? 64 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_44K) ? 64  \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_48K) ? 32 : 16  \

#define DFSDMClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_8K)  ? 24 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_11K) ? 4 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_16K) ? 24 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_22K) ? 4 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_32K) ? 24 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_44K) ? 4  \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_48K) ? 32 : 32  \

#define DFSDMFilterOrder(__FREQUENCY__) \
        (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_8K)  ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_11K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_16K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_22K) ? DFSDM_FILTER_SINC3_ORDER \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_32K) ? DFSDM_FILTER_SINC4_ORDER \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_44K) ? DFSDM_FILTER_SINC3_ORDER  \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_48K) ? DFSDM_FILTER_SINC4_ORDER : DFSDM_FILTER_SINC5_ORDER  \

#define DFSDMRightBitShift(__FREQUENCY__) \
        (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_8K)  ? 6 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_11K) ? 6 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_16K) ? 3 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_22K) ? 3 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_32K) ? 6 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == SAI_AUDIO_FREQUENCY_48K) ? 2 : 0  \
			

#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

void DFSDM_Init(uint32_t *pData,uint16_t PlayBufSize,uint32_t AudioFrequency);
	

#endif
