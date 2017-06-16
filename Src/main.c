#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "QSPI.h"
#include "I2C1.h"
#include "SAI1.h"
#include "RCC.h"

#include "N25Q128.h"
#include "CS43L22.h"

#define AUDIO_FILE_ADDRESS   0x90000000
#define AUDIO_FILE_SIZE      8388608U
#define PLAY_BUFF_SIZE       4096

BSP_QSPI_ID_TypeDef pID;
uint8_t cs43l22_id;
uint16_t PlayBuff[4096];
__IO int32_t UpdatePointer = -1;
uint32_t PlaybackPosition = PLAY_BUFF_SIZE;

void StartDefaultTask(void)
{
    BSP_QSPI_Init();
    BSP_QSPI_RDID(&pID); /* N25Q128 has unique id. */
    BSP_QSPI_EnableMemoryMappedMode();
    cs43l22_id = CS43L22_Init(70);
    //CS43L22_Play((uint8_t *)PlayBuff, 4096, SAI_AUDIO_FREQUENCY_8K,SAI_STEREOMODE);
    CS43L22_Play((uint8_t *)PlayBuff, 4096, SAI_AUDIO_FREQUENCY_8K, SAI_MONOMODE);
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
    vRegisterSampleCLICommands();
    vUARTCommandConsoleStart(1000, 0);
    xTaskCreate((TaskFunction_t)StartDefaultTask, "StartDefaultTask", 128, NULL, 0, NULL);

    vTaskStartScheduler();

    while (1)
    {

    }

}

