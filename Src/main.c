#include "stm32l4xx.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "RCC.h"

#include "LED.h"
#include "LCD.h"
#include "AudioPlayBack.h"


/**
  * @brief converts a 32bit unsigned int into ASCII
  * @caller several callers for display values
  * @param Number digit to displays
  *  p_tab values in array in ASCII
  * @retval None
  */
void Convert_IntegerIntoChar(uint32_t number, uint16_t *p_tab)
{
    uint16_t units = 0, tens = 0, hundreds = 0, thousands = 0, tenthousand = 0, hundredthousand = 0;

    units = ((((number % 100000) % 10000) % 1000) % 100) % 10;
    tens = (((((number - units) / 10) % 10000) % 1000) % 100) % 10;
    hundreds = ((((number - tens - units) / 100) % 1000) % 100) % 10;
    thousands = (((number - hundreds - tens - units) / 1000) % 100) % 10;
    tenthousand = ((number - thousands - hundreds - tens - units) / 10000) % 10;
    hundredthousand = ((number - tenthousand - thousands - hundreds - tens - units) / 100000);

    *(p_tab + 5) = units + 0x30;
    *(p_tab + 4) = tens + 0x30;
    *(p_tab + 3) = hundreds + 0x30;
    *(p_tab + 2) = thousands + 0x30;
    *(p_tab + 1) = tenthousand + 0x30;
    *(p_tab + 0) = hundredthousand + 0x30;
}

static void Idd_Convert(uint32_t Value, uint16_t *DisplayString)
{
    uint32_t valuetoconvert = 0;
    uint16_t SIprefix = 0;
    uint8_t i = 0;
    uint8_t unitindex = 4;
    FlagStatus zerofilter = RESET;

    if(Value < 100)
    {
        /* measured value is in nA & we want 1 digit after coma */
        valuetoconvert = Value * 100;

        /* measured value is in nA */
        /* Add SI prefix information */
        SIprefix = (uint16_t) 'n';
    }
    else if(Value < 100000)
    {
        /* measured value is in ? & we want 1 digit after coma */
        valuetoconvert = Value / 10;
        /* Add SI prefix information */
        SIprefix = (uint16_t) 'u';
    }
    else
    {
        /* measured value is in mA & we want 1 digit after coma */
        valuetoconvert = Value / 10000;

        /* Add SI prefix information */
        SIprefix = (uint16_t) 'm';
    }

    Convert_IntegerIntoChar(valuetoconvert, DisplayString);

    /* reorder display string to have following template WXY.Z mA */
    while(i < unitindex)
    {
        *(DisplayString + i) = *(DisplayString + 2 + i);

        /* remove 0 in front of value */
        if((*(DisplayString + i) == '0') && (zerofilter == RESET))
        {
            *(DisplayString + i) = (uint16_t) ' ';
        }
        else
        {
            zerofilter = SET;
        }
        i++;
    }

    /* Add SI prefix & ampere information */
    *(DisplayString + (unitindex - 2)) |= DOT;

    /* Add SI preficx & ampere information */
    *(DisplayString + (unitindex)) = SIprefix;
    *(DisplayString + (unitindex + 1)) = (uint16_t) 'A';
}

void LCD_Exec(void)
{
    uint16_t Str[6];
    uint32_t Value = 0;
    for(;;)
    {
        if(Value < 100)
            Idd_Convert(Value++, Str);
        else if(Value < 100000)
        {
            Idd_Convert(Value += 50, Str);
        }
        else if(Value < 500000000)
        {
            Idd_Convert(Value += 200, Str);
        }
        else
        {
            Idd_Convert(Value = 0, Str);
        }
        LCD_GLASS_DisplayStrDeci(Str);
        vTaskDelay(100);
    }
}

int main(void)
{
    SystemClock_Config();

    vRegisterSampleCLICommands();
    vUARTCommandConsoleStart(1000, 0);
    Audio_PlayBack_Init();
    LED_Init();
    LCD_Init();

    xTaskCreate((TaskFunction_t)LCD_Exec, "LCD_Exec", 1024, NULL, 0, NULL);

    vTaskStartScheduler();

    while (1)
    {

    }

}

