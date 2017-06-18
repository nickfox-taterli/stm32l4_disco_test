#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "RCC.h"
#include "N25Q128.h"
#include "CS43L22.h"
#include "AT24C256.h"
#include "LED.h"

typedef struct
{
    uint32_t ChunkID;       /* 0 */
    uint32_t FileSize;      /* 4 */
    uint32_t FileFormat;    /* 8 */
    uint32_t SubChunk1ID;   /* 12 */
    uint32_t SubChunk1Size; /* 16*/
    uint16_t AudioFormat;   /* 20 */
    uint16_t NbrChannels;   /* 22 */
    uint32_t SampleRate;    /* 24 */

    uint32_t ByteRate;      /* 28 */
    uint16_t BlockAlign;    /* 32 */
    uint16_t BitPerSample;  /* 34 */
    uint32_t SubChunk2ID;   /* 36 */
    uint32_t SubChunk2Size; /* 40 */
} WAVE_FormatTypeDef;

SemaphoreHandle_t DmaPlayHalfBuffCplt;
SemaphoreHandle_t DmaPlayBuffCplt;
uint16_t PlayBuff[1024];

SemaphoreHandle_t I2C_Lock;

void Play_Exec(void)
{
    WAVE_FormatTypeDef WaveFormat;
    static uint32_t PlaybackPosition = 0;

    if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
    {
        CS43L22_Init(65);

        xSemaphoreGive( I2C_Lock );
    }

    BSP_QSPI_Read((uint8_t *)&WaveFormat, 0x00000000, sizeof(WaveFormat));

    DmaPlayBuffCplt = xSemaphoreCreateBinary();
    DmaPlayHalfBuffCplt = xSemaphoreCreateBinary();


    if(WaveFormat.NbrChannels == 1)
    {
        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            CS43L22_Play((uint8_t *)PlayBuff, 1024, WaveFormat.SampleRate, SAI_MONOMODE);

            xSemaphoreGive( I2C_Lock );
        }
    }
    else if(WaveFormat.NbrChannels == 2)
    {
        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            CS43L22_Play((uint8_t *)PlayBuff, 1024, WaveFormat.SampleRate, SAI_STEREOMODE);

            xSemaphoreGive( I2C_Lock );
        }
    }
    else
    {
        vTaskSuspend(NULL);
    }

    for(;;)
    {
        if( xSemaphoreTake( DmaPlayHalfBuffCplt, portMAX_DELAY ) == pdTRUE )
        {
            BSP_QSPI_Read((uint8_t *)PlayBuff, sizeof(WaveFormat) + PlaybackPosition, 1024);
            PlaybackPosition += 1024;
        }
        if( xSemaphoreTake( DmaPlayBuffCplt, portMAX_DELAY ) == pdTRUE )
        {
            BSP_QSPI_Read((uint8_t *)&PlayBuff[512], sizeof(WaveFormat) + PlaybackPosition, 1024);
            PlaybackPosition += 1024;
        }
        if((PlaybackPosition + 1024) > (WaveFormat.FileSize - 8))
        {
            PlaybackPosition = 0;
        }
    }
}

uint8_t eeprom_val = 0;

void EEPROM_Test(void)
{
    while(1)
    {
        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            EEPROM_Write(0, 0x55);
            xSemaphoreGive( I2C_Lock );
        }
        vTaskDelay(100);

        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            eeprom_val = EEPROM_Read(0);
            xSemaphoreGive( I2C_Lock );
        }
        vTaskDelay(100);

        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            EEPROM_Write(0, 0xAA);
            xSemaphoreGive( I2C_Lock );
        }
        vTaskDelay(100);
				
        if(xSemaphoreTake( I2C_Lock, portMAX_DELAY ) == pdTRUE)
        {
            eeprom_val = EEPROM_Read(0);
            xSemaphoreGive( I2C_Lock );
        }
        vTaskDelay(100);
    }
}

int main(void)
{
    SystemClock_Config();

    vRegisterSampleCLICommands();
    vUARTCommandConsoleStart(1000, 0);
    LED_Init();
    BSP_QSPI_Init();
    AT24C256_Init();

    I2C_Lock = xSemaphoreCreateMutex();
    xTaskCreate((TaskFunction_t)EEPROM_Test, "EEPROM_Test", 1024, NULL, 0, NULL);
    xTaskCreate((TaskFunction_t)Play_Exec, "Play_Exec", 1024, NULL, 0, NULL);

    vTaskStartScheduler();

    while (1)
    {

    }

}

