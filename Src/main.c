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
#include "USART2_vUSR.h"

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
    USART2_Init();
    while(1)
    {
        L3GD20_ReadXYZAngRate(Angle);
        LSM303C_AccReadXYZ(AccX);
        LSM303C_MagReadXYZ(MagX);

        Prepare_Data(AccX, AccX_Avg);
        IMUupdate(Angle, AccX, Angle_IMU);

        _cnt = 0;
        sum = 0;
        Alt_Value++;



        data_to_send[_cnt++] = 0xAA;
        data_to_send[_cnt++] = 0xAA;
        data_to_send[_cnt++] = 0x01;
        data_to_send[_cnt++] = 0;

        _temp = (int16_t)(Angle_IMU[0] * 100);
        data_to_send[_cnt++] = BYTE1(_temp);
        data_to_send[_cnt++] = BYTE0(_temp);
        _temp = (int16_t)(Angle_IMU[1] * 100);
        data_to_send[_cnt++] = BYTE1(_temp);
        data_to_send[_cnt++] = BYTE0(_temp);
        _temp = (int16_t)(Angle_IMU[2] * 100);
        data_to_send[_cnt++] = BYTE1(_temp);
        data_to_send[_cnt++] = BYTE0(_temp);
        data_to_send[_cnt++] = BYTE3(Alt_Value);
        data_to_send[_cnt++] = BYTE2(Alt_Value);
        data_to_send[_cnt++] = BYTE1(Alt_Value);
        data_to_send[_cnt++] = BYTE0(Alt_Value);

        data_to_send[_cnt++] = 0xA0;

        data_to_send[3] = _cnt - 4;

        for(i = 0; i < _cnt; i++)
            sum += data_to_send[i];
        data_to_send[_cnt++] = sum;

        USART2_SendBuf(data_to_send, _cnt);

        _cnt = 0;
        sum = 0;

        data_to_send[_cnt++] = 0xAA;
        data_to_send[_cnt++] = 0xAA;
        data_to_send[_cnt++] = 0x02;
        data_to_send[_cnt++] = 0;
        data_to_send[_cnt++] = (AccX[0] >> 8) & 0xFF;
        data_to_send[_cnt++] = AccX[0] & 0xFF;
        data_to_send[_cnt++] = (AccX[1] >> 8) & 0xFF;
        data_to_send[_cnt++] = AccX[1] & 0xFF;
        data_to_send[_cnt++] = (AccX[2] >> 8) & 0xFF;;
        data_to_send[_cnt++] = AccX[2] & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[0] >> 8) & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[0]) & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[1] >> 8) & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[1]) & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[2] >> 8) & 0xFF;
        data_to_send[_cnt++] = ((int16_t)Angle[2]) & 0xFF;
        data_to_send[_cnt++] = (MagX[0] >> 8) & 0xFF;
        data_to_send[_cnt++] = (MagX[0]) & 0xFF;
        data_to_send[_cnt++] = (MagX[1] >> 8) & 0xFF;
        data_to_send[_cnt++] = (MagX[1]) & 0xFF;
        data_to_send[_cnt++] = (MagX[2] >> 8) & 0xFF;
        data_to_send[_cnt++] = (MagX[2]) & 0xFF;

        data_to_send[3] = _cnt - 4;

        for(i = 0; i < _cnt; i++)
            sum += data_to_send[i];
        data_to_send[_cnt++] = sum & 0xFF;
        USART2_SendBuf(data_to_send, _cnt);
        vTaskDelay(10);
    }
}

int main(void)
{

    SystemClock_Config();

    /*
        vRegisterSampleCLICommands();
        vUARTCommandConsoleStart(1000, 0);
    		Audio_PlayBack_Init();
    */

    xTaskCreate((TaskFunction_t)MEMS_Exec, "AudioPB", 1024, NULL, 0, NULL);
    vTaskStartScheduler();

    while (1)
    {

    }

}

