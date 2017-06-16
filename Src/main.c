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
#include "DFSDM.h"

#include "N25Q128.h"
#include "CS43L22.h"

BSP_QSPI_ID_TypeDef pID;
int32_t                      RecBuff[2048];
int16_t                      PlayBuff[2048];

uint32_t DmaRecHalfBuffCplt  = 0;
uint32_t DmaRecBuffCplt  = 0;

uint32_t i = 0;

void StartDefaultTask(void)
{
    BSP_QSPI_Init();
    BSP_QSPI_RDID(&pID); /* N25Q128 has unique id. */
    BSP_QSPI_EnableMemoryMappedMode();
    DFSDM_Init(RecBuff, 2048, SAI_AUDIO_FREQUENCY_44K);
    CS43L22_Init(70);
    CS43L22_Play((uint8_t *)PlayBuff, 2048, SAI_AUDIO_FREQUENCY_44K, SAI_MONOMODE);
    for(;;)
    {
        if(DmaRecHalfBuffCplt == 1)
        {
            /* Store values on Play buff */
            for(i = 0; i < 1024; i++)
            {
                PlayBuff[i]     = SaturaLH((RecBuff[i] >> 8), -32768, 32767);
            }
            DmaRecHalfBuffCplt  = 0;
            if(DmaRecBuffCplt == 1)
            {
                /* Store values on Play buff */
                for(i = 1024; i < 2048; i++)
                {
                    PlayBuff[i]     = SaturaLH((RecBuff[i] >> 8), -32768, 32767);
                }
                DmaRecBuffCplt  = 0;
            }
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

