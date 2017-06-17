#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "RCC.h"
#include "AudioPlayBack.h"

#include "MEMS.h"

uint8_t tmp = 0x00;

int main(void)
{

    SystemClock_Config();

    for(;;)
    {
        tmp = L3GD20_ReadID();
        tmp = LSM303C_AccReadID();
        tmp = 0x00;
    }

    vRegisterSampleCLICommands();
    vUARTCommandConsoleStart(1000, 0);

    Audio_PlayBack_Init();

    vTaskStartScheduler();

    while (1)
    {

    }

}

