#include "CS43L22.h"
#include "I2C1.h"
#include "SAI1.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

static uint8_t Is_CS43L22_Stop = 1;

uint8_t CS43L22_Init(uint8_t Volume)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

		if(!LL_I2C_IsEnabled(I2C1)){
			 I2C1_Init();
		}	
	
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_3);
    vTaskDelay(5);
    LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_3);
    vTaskDelay(5);

    /* Keep Codec powered OFF */
    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL1, 0x01);


    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL2, 0xAF); /* OUTPUT_DEVICE_HEADPHONE */

    /* Clock configuration: Auto detection */
    I2C1_Master_Write(0x94, CS43L22_REG_CLOCKING_CTL, 0x81);

    /* Set the Slave Mode and the audio Standard */
    I2C1_Master_Write(0x94, CS43L22_REG_INTERFACE_CTL1, CODEC_STANDARD);

    /* Set the Master volume */
    CS43L22_SetVolume(Volume);

    /* Additional configuration for the CODEC. These configurations are done to reduce
    the time needed for the Codec to power off. If these configurations are removed,
    then a long delay should be added between powering off the Codec and switching
    off the I2S peripheral MCLK clock (which is the operating clock for Codec).
    If this delay is not inserted, then the codec will not shut down properly and
    it results in high noise after shut down. */

    /* Disable the analog soft ramp */
    I2C1_Master_Write(0x94, CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);
    /* Disable the digital soft ramp */
    I2C1_Master_Write(0x94, CS43L22_REG_MISC_CTL, 0x04);
    /* Disable the limiter attack level */
    I2C1_Master_Write(0x94, CS43L22_REG_LIMIT_CTL1, 0x00);
    /* Adjust Bass and Treble levels */
    I2C1_Master_Write(0x94, CS43L22_REG_TONE_CTL, 0x0F);
    /* Adjust PCM volume level */
    I2C1_Master_Write(0x94, CS43L22_REG_PCMA_VOL, 0x0A);
    I2C1_Master_Write(0x94, CS43L22_REG_PCMB_VOL, 0x0A);

    SAI1_MspInit();

    return CS43L22_ReadID();
}

/**
  * @brief Sets higher or lower the codec volume level.
  * @param Volume: a byte value from 0 to 255 (refer to codec registers
  *         description for more details).
  * @retval None
  */
void CS43L22_SetVolume(uint8_t Volume)
{
    uint8_t convertedvol = VOLUME_CONVERT(Volume);

    if(Volume > 0xE6)
    {
        /* Set the Master volume */
        I2C1_Master_Write(0x94, CS43L22_REG_MASTER_A_VOL, convertedvol - 0xE7);
        I2C1_Master_Write(0x94, CS43L22_REG_MASTER_B_VOL, convertedvol - 0xE7);
    }
    else
    {
        /* Set the Master volume */
        I2C1_Master_Write(0x94, CS43L22_REG_MASTER_A_VOL, convertedvol + 0x19);
        I2C1_Master_Write(0x94, CS43L22_REG_MASTER_B_VOL, convertedvol + 0x19);
    }
}

/**
  * @brief  Get the CS43L22 ID.
  * @retval The CS43L22 ID
  */
uint8_t CS43L22_ReadID(void)
{
    uint8_t Value;

    Value = I2C1_Master_Read(0x94, CS43L22_CHIPID_ADDR);
    Value = (Value & CS43L22_ID_MASK);

    return((uint32_t) Value);
}


/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @retval None
  */
void CS43L22_Play(uint8_t *pData,
                  uint16_t PlayBufSize,
                  uint32_t AudioFrequency,
									uint32_t SAI_Mono_Stereo_Mode)
{

    if(Is_CS43L22_Stop == 1)
    {


        /* Enable the digital soft ramp */
        I2C1_Master_Write(0x94, CS43L22_REG_MISC_CTL, 0x06);

        /* Enable Output device */
        CS43L22_SetMute(AUDIO_MUTE_OFF);

        /* Power on the Codec */
        I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL1, 0x9E);

        SAI1_Play(pData, PlayBufSize, AudioFrequency,SAI_Mono_Stereo_Mode);

        Is_CS43L22_Stop = 0;
    }

}

/**
  * @brief Pauses playing on the audio codec.
  * @retval None
  */
void CS43L22_Pause(void)
{

    /* Pause the audio file playing */
    /* Mute the output first */
    CS43L22_SetMute(AUDIO_MUTE_ON);

    /* Put the Codec in Power save mode */
    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL1, 0x01);

}


/**
  * @brief Resumes playing on the audio codec.
  * @retval None
  */
void CS43L22_Resume(void)
{
    volatile uint32_t index = 0x00;
    /* Resumes the audio file playing */
    /* Unmute the output first */
    CS43L22_SetMute(AUDIO_MUTE_OFF);

    for(index = 0x00; index < 0xFF; index++);

    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL2, 0xAF);

    /* Exit the Power save mode */
    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL1, 0x9E);

}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param CodecPdwnMode: selects the  power down mode.
  *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration
  *                           (user should re-Initialize the codec in order to
  *                            play again the audio stream).
  * @retval None
  */
void CS43L22_Stop(void)
{
		if(Is_CS43L22_Stop == 1)
			return;
		
    /* Mute the output first */
    CS43L22_SetMute(AUDIO_MUTE_ON);

    /* Disable the digital soft ramp */
    I2C1_Master_Write(0x94, CS43L22_REG_MISC_CTL, 0x04);

    /* Power down the DAC and the speaker (PMDAC and PMSPK bits)*/
    I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL1, 0x9F);

		vTaskDelay(1);
		
		SAI1_Stop();
	
    Is_CS43L22_Stop = 1;


}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval None
  */
void CS43L22_SetMute(uint32_t Cmd)
{

    /* Set the Mute mode */
    if(Cmd == AUDIO_MUTE_ON)
    {
        I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL2, 0xFF);
        I2C1_Master_Write(0x94, CS43L22_REG_HEADPHONE_A_VOL, 0x01);
        I2C1_Master_Write(0x94, CS43L22_REG_HEADPHONE_B_VOL, 0x01);
    }
    else /* AUDIO_MUTE_OFF Disable the Mute */
    {
        I2C1_Master_Write(0x94, CS43L22_REG_HEADPHONE_A_VOL, 0x00);
        I2C1_Master_Write(0x94, CS43L22_REG_HEADPHONE_B_VOL, 0x00);
        I2C1_Master_Write(0x94, CS43L22_REG_POWER_CTL2, 0xAF);
    }
}
