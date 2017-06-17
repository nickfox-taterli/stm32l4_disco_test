#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "RCC.h"

#include "MEMS.h"
#include "IMU.h"
#include "LED.h"
#include "AudioPlayBack.h"

#define BYTE0(dwTemp)       (*(uint8_t *)(&dwTemp))
#define BYTE1(dwTemp)       (*((uint8_t *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((uint8_t *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((uint8_t *)(&dwTemp) + 3))

float Angle[3];
float Angle_IMU[3];
int16_t AccX[3];
int16_t AccX_Avg[3];
int16_t MagX[3];

uint8_t data_to_send[24];
uint32_t sum = 0;
__IO uint8_t _cnt = 0;
__IO uint8_t i = 0;

__IO int16_t _temp;
__IO int32_t Alt_Value = 0;

void MEMS_Exec(void)
{

    MEMS_Init();
    while(1)
    {
        L3GD20_ReadXYZAngRate(Angle);
        LSM303C_AccReadXYZ(AccX);
        LSM303C_MagReadXYZ(MagX);

        Prepare_Data(AccX, AccX_Avg);
        IMUupdate(Angle, AccX, Angle_IMU);

        _cnt = 0;
        sum = 0;
				
				if(Alt_Value > 1000)
					Alt_Value = 980;
				else
					Alt_Value++;
				
				vTaskDelay(1);
				
    }
}

int main(void)
{

    SystemClock_Config();

    
		vRegisterSampleCLICommands();
		vUARTCommandConsoleStart(1000, 0);
		Audio_PlayBack_Init();
		LED_Init();
    
    xTaskCreate((TaskFunction_t)MEMS_Exec, "MEMS_Exec", 1024, NULL, 0, NULL);
	
    vTaskStartScheduler();

    while (1)
    {

    }

}

