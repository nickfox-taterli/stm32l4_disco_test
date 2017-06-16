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

int main(void)
{

    SystemClock_Config();

    vRegisterSampleCLICommands();
    vUARTCommandConsoleStart(1000, 0);

		Audio_PlayBack_Init();
	
    vTaskStartScheduler();

    while (1)
    {

    }

}

