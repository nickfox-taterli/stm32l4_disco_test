#include "AudioPlayBack.h"

#include "I2C1.h"
#include "SAI1.h"
#include "RCC.h"
#include "DFSDM.h"

#include "CS43L22.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SemaphoreHandle_t DmaRecHalfBuffCplt;
SemaphoreHandle_t DmaRecBuffCplt;

static int32_t *RecBuff;
static int16_t *PlayBuff;

static uint8_t IsAlloc = 0;

static TaskHandle_t PlayBackHandle;

static BaseType_t prvStartStopPlayBackCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static const CLI_Command_Definition_t xStartStopPlayBack =
{
    "playback",
    "\r\nplayback [start | stop]:\r\n Starts or stops a playback\r\n",
    prvStartStopPlayBackCommand, /* The function to run. */
    1 /* One parameter is expected.  Valid values are "start" and "stop". */
};

static BaseType_t prvStartStopPlayBackCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    const char *pcParameter;
    BaseType_t lParameterStringLength;

    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;
    configASSERT( pcWriteBuffer );

    /* Obtain the parameter string. */
    pcParameter = FreeRTOS_CLIGetParameter
                  (
                      pcCommandString,		/* The command string itself. */
                      1,						/* Return the first parameter. */
                      &lParameterStringLength	/* Store the parameter string length. */
                  );

    /* Sanity check something was returned. */
    configASSERT( pcParameter );

    /* There are only two valid parameter values. */
    if( strncmp( pcParameter, "start", strlen( "start" ) ) == 0 )
    {
        /* Start or restart the Audio PlayBack. */
        CS43L22_Stop();
        DFSDM_Stop();

        if(IsAlloc == 1)
        {

            vPortFree(RecBuff);
            vPortFree(PlayBuff);
            IsAlloc = 0;
        }

        if(IsAlloc == 0)
        {

            RecBuff = pvPortMalloc(512 * sizeof(int32_t));
            PlayBuff = pvPortMalloc(512 * sizeof(int32_t));
            IsAlloc = 1;
        }

        CS43L22_Play((uint8_t *)PlayBuff, 512, SAI_AUDIO_FREQUENCY_48K, SAI_MONOMODE);
        DFSDM_Play((uint32_t *)RecBuff, 512, SAI_AUDIO_FREQUENCY_48K);
        vTaskResume(PlayBackHandle);
        sprintf( pcWriteBuffer, "playback (re)started.\r\n" );
    }
    else if( strncmp( pcParameter, "stop", strlen( "stop" ) ) == 0 )
    {
        /* End the PlayBack, if one is running. */
        CS43L22_Stop();
        DFSDM_Stop();
        if(IsAlloc == 1)
        {

            vPortFree(RecBuff);
            vPortFree(PlayBuff);
            IsAlloc = 0;
        }
        sprintf( pcWriteBuffer, "Stopping playback.\r\n" );
        vTaskSuspend(PlayBackHandle);

    }
    else
    {
        sprintf( pcWriteBuffer, "Valid parameters are 'start' and 'stop' and 'free'.\r\n" );
    }

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}


void Aduio_PlayBack_Exec(void)
{
		/* 目前只 */
    DFSDM_Init();
    CS43L22_Init(70);
    DmaRecBuffCplt = xSemaphoreCreateBinary();
    DmaRecHalfBuffCplt = xSemaphoreCreateBinary();
	
    vTaskSuspend(NULL);
	
    for(;;)
    {
        if( xSemaphoreTake( DmaRecHalfBuffCplt, portMAX_DELAY ) == pdTRUE )
        {
            /* Store values on Play buff */
            for(uint16_t i = 0; i < 256; i++)
            {
                PlayBuff[i]     = SaturaLH((RecBuff[i] >> 8), -32768, 32767);
            }
        }
        if( xSemaphoreTake( DmaRecBuffCplt, portMAX_DELAY ) == pdTRUE )
        {
            /* Store values on Play buff */
            for(uint16_t i = 256; i < 512; i++)
            {
                PlayBuff[i]     = SaturaLH((RecBuff[i] >> 8), -32768, 32767);
            }
        }
    }
}

void Audio_PlayBack_Init(void){
		FreeRTOS_CLIRegisterCommand( &xStartStopPlayBack );
    xTaskCreate((TaskFunction_t)Aduio_PlayBack_Exec, "AudioPB", configMINIMAL_STACK_SIZE, NULL, 0, &PlayBackHandle);
}
