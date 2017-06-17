#include "LED.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static BaseType_t prvLEDControlCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static const CLI_Command_Definition_t xLEDControl =
{
    "led",
    "\r\nled [red | green] [on | off]:\r\n Expects two parameters, red or green led on or off.\r\n",
    prvLEDControlCommand, /* The function to run. */
    2
};

static BaseType_t prvLEDControlCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter_red_or_green, *pcParameter_on_or_off;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* Obtain the parameter string. */
    pcParameter_red_or_green = FreeRTOS_CLIGetParameter
                               (
                                   pcCommandString,		/* The command string itself. */
                                   1,						/* Return the first parameter. */
                                   NULL	/* Store the parameter string length. */
                               );

    /* Obtain the parameter string. */
    pcParameter_on_or_off = FreeRTOS_CLIGetParameter
                            (
                                pcCommandString,		/* The command string itself. */
                                2,						/* Return the first parameter. */
                                NULL	/* Store the parameter string length. */
                            );

    /* Sanity check something was returned. */
    configASSERT( pcParameter_red_or_green );
    configASSERT( pcParameter_on_or_off );

    /* There are only two valid parameter values. */
    if( strncmp( pcParameter_red_or_green, "red", strlen( "red" ) ) == 0 )
    {

        if( strncmp( pcParameter_on_or_off, "on", strlen( "on" ) ) == 0 )
        {
            LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_2);

            sprintf( pcWriteBuffer, "Red LED On.\r\n" );

        }
        else if( strncmp( pcParameter_on_or_off, "off", strlen( "off" ) ) == 0 )
        {
            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
            sprintf( pcWriteBuffer, "Red LED Off.\r\n" );
        }
        else
        {
            sprintf( pcWriteBuffer, "Valid third parameters are 'on' and 'off'.\r\n" );

        }

    }
    else if( strncmp( pcParameter_red_or_green, "green", strlen( "green" ) ) == 0 )
    {
        if( strncmp( pcParameter_on_or_off, "on", strlen( "on" ) ) == 0 )
        {
            LL_GPIO_SetOutputPin(GPIOE, LL_GPIO_PIN_8);
            sprintf( pcWriteBuffer, "Green LED On.\r\n" );

        }
        else if( strncmp( pcParameter_on_or_off, "off", strlen( "off" ) ) == 0 )
        {
            LL_GPIO_ResetOutputPin(GPIOE, LL_GPIO_PIN_8);
            sprintf( pcWriteBuffer, "Green LED Off.\r\n" );
        }
        else
        {
            sprintf( pcWriteBuffer, "Valid third parameters are 'on' and 'off'.\r\n" );

        }

    }
    else
    {
        sprintf( pcWriteBuffer, "Valid second parameters are 'red' and 'green'.\r\n" );
    }

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

void LED_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    FreeRTOS_CLIRegisterCommand( &xLEDControl );
}
