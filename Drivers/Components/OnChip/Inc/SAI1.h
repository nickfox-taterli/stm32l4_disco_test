#ifndef __DISCO_SAI_H
#define __DISCO_SAI_H

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


/** @defgroup SAI_Block_SyncExt SAI External synchronisation
  * @{
  */
#define SAI_SYNCEXT_DISABLE          0
#define SAI_SYNCEXT_OUTBLOCKA_ENABLE 1
#define SAI_SYNCEXT_OUTBLOCKB_ENABLE 2
/**
  * @}
  */

/** @defgroup SAI_Protocol SAI Supported protocol
  * @{
  */
#define SAI_I2S_STANDARD      0
#define SAI_I2S_MSBJUSTIFIED  1
#define SAI_I2S_LSBJUSTIFIED  2
#define SAI_PCM_LONG          3
#define SAI_PCM_SHORT         4
/**
  * @}
  */

/** @defgroup SAI_Protocol_DataSize SAI protocol data size
  * @{
  */
#define SAI_PROTOCOL_DATASIZE_16BIT         0
#define SAI_PROTOCOL_DATASIZE_16BITEXTENDED 1
#define SAI_PROTOCOL_DATASIZE_24BIT         2
#define SAI_PROTOCOL_DATASIZE_32BIT         3
/**
  * @}
  */

/** @defgroup SAI_Audio_Frequency SAI Audio Frequency
  * @{
  */
#define SAI_AUDIO_FREQUENCY_192K          ((uint32_t)192000U)
#define SAI_AUDIO_FREQUENCY_96K           ((uint32_t)96000U)
#define SAI_AUDIO_FREQUENCY_48K           ((uint32_t)48000U)
#define SAI_AUDIO_FREQUENCY_44K           ((uint32_t)44100U)
#define SAI_AUDIO_FREQUENCY_32K           ((uint32_t)32000U)
#define SAI_AUDIO_FREQUENCY_22K           ((uint32_t)22050U)
#define SAI_AUDIO_FREQUENCY_16K           ((uint32_t)16000U)
#define SAI_AUDIO_FREQUENCY_11K           ((uint32_t)11025U)
#define SAI_AUDIO_FREQUENCY_8K            ((uint32_t)8000U)
#define SAI_AUDIO_FREQUENCY_MCKDIV        ((uint32_t)0U)
/**
  * @}
  */

/** @defgroup SAI_Block_Mode SAI Block Mode
  * @{
  */
#define SAI_MODEMASTER_TX         ((uint32_t)0x00000000U)
#define SAI_MODEMASTER_RX         ((uint32_t)SAI_xCR1_MODE_0)
#define SAI_MODESLAVE_TX          ((uint32_t)SAI_xCR1_MODE_1)
#define SAI_MODESLAVE_RX          ((uint32_t)(SAI_xCR1_MODE_1 | SAI_xCR1_MODE_0))

/**
  * @}
  */

/** @defgroup SAI_Block_Protocol SAI Block Protocol
  * @{
  */
#define SAI_FREE_PROTOCOL                 ((uint32_t)0x00000000U)
#define SAI_SPDIF_PROTOCOL                ((uint32_t)SAI_xCR1_PRTCFG_0)
#define SAI_AC97_PROTOCOL                 ((uint32_t)SAI_xCR1_PRTCFG_1)
/**
  * @}
  */

/** @defgroup SAI_Block_Data_Size SAI Block Data Size
  * @{
  */
#define SAI_DATASIZE_8     ((uint32_t)SAI_xCR1_DS_1)
#define SAI_DATASIZE_10    ((uint32_t)(SAI_xCR1_DS_1 | SAI_xCR1_DS_0))
#define SAI_DATASIZE_16    ((uint32_t)SAI_xCR1_DS_2)
#define SAI_DATASIZE_20    ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_0))
#define SAI_DATASIZE_24    ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_1))
#define SAI_DATASIZE_32    ((uint32_t)(SAI_xCR1_DS_2 | SAI_xCR1_DS_1 | SAI_xCR1_DS_0))
/**
  * @}
  */

/** @defgroup SAI_Block_MSB_LSB_transmission SAI Block MSB LSB transmission
  * @{
  */
#define SAI_FIRSTBIT_MSB                  ((uint32_t)0x00000000U)
#define SAI_FIRSTBIT_LSB                  ((uint32_t)SAI_xCR1_LSBFIRST)
/**
  * @}
  */

/** @defgroup SAI_Block_Clock_Strobing SAI Block Clock Strobing
  * @{
  */
#define SAI_CLOCKSTROBING_FALLINGEDGE     0
#define SAI_CLOCKSTROBING_RISINGEDGE      1
/**
  * @}
  */

/** @defgroup SAI_Block_Synchronization SAI Block Synchronization
  * @{
  */
#define SAI_ASYNCHRONOUS                  0 /*!< Asynchronous */
#define SAI_SYNCHRONOUS                   1 /*!< Synchronous with other block of same SAI */
#define SAI_SYNCHRONOUS_EXT_SAI1          2 /*!< Synchronous with other SAI, SAI1 */
#define SAI_SYNCHRONOUS_EXT_SAI2          3 /*!< Synchronous with other SAI, SAI2 */
/**
  * @}
  */

/** @defgroup SAI_Block_Output_Drive SAI Block Output Drive
  * @{
  */
#define SAI_OUTPUTDRIVE_DISABLE          ((uint32_t)0x00000000U)
#define SAI_OUTPUTDRIVE_ENABLE           ((uint32_t)SAI_xCR1_OUTDRIV)
/**
  * @}
  */

/** @defgroup SAI_Block_NoDivider SAI Block NoDivider
  * @{
  */
#define SAI_MASTERDIVIDER_ENABLE         ((uint32_t)0x00000000U)
#define SAI_MASTERDIVIDER_DISABLE        ((uint32_t)SAI_xCR1_NODIV)
/**
  * @}
  */


/** @defgroup SAI_Block_FS_Definition SAI Block FS Definition
  * @{
  */
#define SAI_FS_STARTFRAME                 ((uint32_t)0x00000000U)
#define SAI_FS_CHANNEL_IDENTIFICATION     ((uint32_t)SAI_xFRCR_FSDEF)
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Polarity SAI Block FS Polarity
  * @{
  */
#define SAI_FS_ACTIVE_LOW                  ((uint32_t)0x00000000U)
#define SAI_FS_ACTIVE_HIGH                 ((uint32_t)SAI_xFRCR_FSPOL)
/**
  * @}
  */

/** @defgroup SAI_Block_FS_Offset SAI Block FS Offset
  * @{
  */
#define SAI_FS_FIRSTBIT                   ((uint32_t)0x00000000U)
#define SAI_FS_BEFOREFIRSTBIT             ((uint32_t)SAI_xFRCR_FSOFF)
/**
  * @}
  */


/** @defgroup SAI_Block_Slot_Size SAI Block Slot Size
* @{
*/
#define SAI_SLOTSIZE_DATASIZE             ((uint32_t)0x00000000U)
#define SAI_SLOTSIZE_16B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_0)
#define SAI_SLOTSIZE_32B                  ((uint32_t)SAI_xSLOTR_SLOTSZ_1)
/**
  * @}
  */

/** @defgroup SAI_Block_Slot_Active SAI Block Slot Active
  * @{
  */
#define SAI_SLOT_NOTACTIVE           ((uint32_t)0x00000000U)
#define SAI_SLOTACTIVE_0             ((uint32_t)0x00000001U)
#define SAI_SLOTACTIVE_1             ((uint32_t)0x00000002U)
#define SAI_SLOTACTIVE_2             ((uint32_t)0x00000004U)
#define SAI_SLOTACTIVE_3             ((uint32_t)0x00000008U)
#define SAI_SLOTACTIVE_4             ((uint32_t)0x00000010U)
#define SAI_SLOTACTIVE_5             ((uint32_t)0x00000020U)
#define SAI_SLOTACTIVE_6             ((uint32_t)0x00000040U)
#define SAI_SLOTACTIVE_7             ((uint32_t)0x00000080U)
#define SAI_SLOTACTIVE_8             ((uint32_t)0x00000100U)
#define SAI_SLOTACTIVE_9             ((uint32_t)0x00000200U)
#define SAI_SLOTACTIVE_10            ((uint32_t)0x00000400U)
#define SAI_SLOTACTIVE_11            ((uint32_t)0x00000800U)
#define SAI_SLOTACTIVE_12            ((uint32_t)0x00001000U)
#define SAI_SLOTACTIVE_13            ((uint32_t)0x00002000U)
#define SAI_SLOTACTIVE_14            ((uint32_t)0x00004000U)
#define SAI_SLOTACTIVE_15            ((uint32_t)0x00008000U)
#define SAI_SLOTACTIVE_ALL           ((uint32_t)0x0000FFFFU)
/**
  * @}
  */

/** @defgroup SAI_Mono_Stereo_Mode SAI Mono Stereo Mode
  * @{
  */
#define SAI_STEREOMODE               ((uint32_t)0x00000000U)
#define SAI_MONOMODE                 ((uint32_t)SAI_xCR1_MONO)
/**
  * @}
  */

/** @defgroup SAI_TRIState_Management SAI TRIState Management
  * @{
  */
#define SAI_OUTPUT_NOTRELEASED    ((uint32_t)0x00000000U)
#define SAI_OUTPUT_RELEASED       ((uint32_t)SAI_xCR2_TRIS)
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Threshold SAI Block Fifo Threshold
  * @{
  */
#define SAI_FIFOTHRESHOLD_EMPTY  ((uint32_t)0x00000000U)
#define SAI_FIFOTHRESHOLD_1QF    ((uint32_t)(SAI_xCR2_FTH_0))
#define SAI_FIFOTHRESHOLD_HF     ((uint32_t)(SAI_xCR2_FTH_1))
#define SAI_FIFOTHRESHOLD_3QF    ((uint32_t)(SAI_xCR2_FTH_1 | SAI_xCR2_FTH_0))
#define SAI_FIFOTHRESHOLD_FULL   ((uint32_t)(SAI_xCR2_FTH_2))
/**
  * @}
  */

/** @defgroup SAI_Block_Companding_Mode SAI Block Companding Mode
  * @{
  */
#define SAI_NOCOMPANDING                 ((uint32_t)0x00000000U)
#define SAI_ULAW_1CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1))
#define SAI_ALAW_1CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_COMP_0))
#define SAI_ULAW_2CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_CPL))
#define SAI_ALAW_2CPL_COMPANDING         ((uint32_t)(SAI_xCR2_COMP_1 | SAI_xCR2_COMP_0 | SAI_xCR2_CPL))
/**
  * @}
  */

/** @defgroup SAI_Block_Mute_Value SAI Block Mute Value
  * @{
  */
#define SAI_ZERO_VALUE                     ((uint32_t)0x00000000U)
#define SAI_LAST_SENT_VALUE                ((uint32_t)SAI_xCR2_MUTEVAL)
/**
  * @}
  */

/** @defgroup SAI_Block_Interrupts_Definition SAI Block Interrupts Definition
  * @{
  */
#define SAI_IT_OVRUDR                     ((uint32_t)SAI_xIMR_OVRUDRIE)
#define SAI_IT_MUTEDET                    ((uint32_t)SAI_xIMR_MUTEDETIE)
#define SAI_IT_WCKCFG                     ((uint32_t)SAI_xIMR_WCKCFGIE)
#define SAI_IT_FREQ                       ((uint32_t)SAI_xIMR_FREQIE)
#define SAI_IT_CNRDY                      ((uint32_t)SAI_xIMR_CNRDYIE)
#define SAI_IT_AFSDET                     ((uint32_t)SAI_xIMR_AFSDETIE)
#define SAI_IT_LFSDET                     ((uint32_t)SAI_xIMR_LFSDETIE)
/**
  * @}
  */

/** @defgroup SAI_Block_Flags_Definition  SAI Block Flags Definition
  * @{
  */
#define SAI_FLAG_OVRUDR                   ((uint32_t)SAI_xSR_OVRUDR)
#define SAI_FLAG_MUTEDET                  ((uint32_t)SAI_xSR_MUTEDET)
#define SAI_FLAG_WCKCFG                   ((uint32_t)SAI_xSR_WCKCFG)
#define SAI_FLAG_FREQ                     ((uint32_t)SAI_xSR_FREQ)
#define SAI_FLAG_CNRDY                    ((uint32_t)SAI_xSR_CNRDY)
#define SAI_FLAG_AFSDET                   ((uint32_t)SAI_xSR_AFSDET)
#define SAI_FLAG_LFSDET                   ((uint32_t)SAI_xSR_LFSDET)
/**
  * @}
  */

/** @defgroup SAI_Block_Fifo_Status_Level   SAI Block Fifo Status Level
  * @{
  */
#define SAI_FIFOSTATUS_EMPTY              ((uint32_t)0x00000000U)
#define SAI_FIFOSTATUS_LESS1QUARTERFULL   ((uint32_t)0x00010000U)
#define SAI_FIFOSTATUS_1QUARTERFULL       ((uint32_t)0x00020000U)
#define SAI_FIFOSTATUS_HALFFULL           ((uint32_t)0x00030000U)
#define SAI_FIFOSTATUS_3QUARTERFULL       ((uint32_t)0x00040000U)
#define SAI_FIFOSTATUS_FULL               ((uint32_t)0x00050000U)
/**
  * @}
  */

void SAI1_MspInit(void);
void SAI1_Play(uint8_t *pData, uint16_t PlayBufSize, uint32_t AudioFrequency,uint32_t SAI_Mono_Stereo_Mode);
void SAI1_Stop(void);

#endif
