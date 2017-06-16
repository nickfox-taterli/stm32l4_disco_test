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

uint8_t  osCPU_Usage = 0; 
uint32_t       osCPU_IdleStartTime = 0; 
uint32_t       osCPU_IdleSpentTime = 0; 
uint32_t       osCPU_TotalIdleTime = 0; 

static BaseType_t prvGetCPUUsageCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

static const CLI_Command_Definition_t xGetCPUUsage =
{
    "cpu",
    "\r\ncpu:\r\n Get CPU Usage\r\n",
    prvGetCPUUsageCommand, /* The function to run. */
    0 /* No parameters are expected. */
};

static BaseType_t prvGetCPUUsageCommand( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
    /* Remove compile time warnings about unused parameters, and check the
    write buffer is not NULL.  NOTE - for simplicity, this example assumes the
    write buffer length is adequate, so does not check for buffer overflows. */
    ( void ) pcCommandString;
    ( void ) xWriteBufferLen;

		sprintf( pcWriteBuffer, "CPU Usage:%d%%.\r\n",osCPU_Usage );

    /* There is no more data to return after this single string, so return
    pdFALSE. */
    return pdFALSE;
}

void vApplicationTickHook (void)
{
  static int tick = 0;
  
  if(tick ++ > CALCULATION_PERIOD)
  {
    tick = 0;
    
    if(osCPU_TotalIdleTime > 1000)
    {
      osCPU_TotalIdleTime = 1000;
    }
    osCPU_Usage = (100 - (osCPU_TotalIdleTime * 100) / CALCULATION_PERIOD);
    osCPU_TotalIdleTime = 0;
  }
}

/**
  * @brief  Start Idle monitor
  * @param  None 
  * @retval None
  */
void trace_StartIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == xTaskGetIdleTaskHandle() ) 
  {
    osCPU_IdleStartTime = xTaskGetTickCountFromISR();
  }
}

/**
  * @brief  Stop Idle monitor
  * @param  None 
  * @retval None
  */
void trace_EndIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == xTaskGetIdleTaskHandle() )
  {
    /* Store the handle to the idle task. */
    osCPU_IdleSpentTime = xTaskGetTickCountFromISR() - osCPU_IdleStartTime;
    osCPU_TotalIdleTime += osCPU_IdleSpentTime; 
  }
}


void CPU_Usage_Init(void){
		FreeRTOS_CLIRegisterCommand( &xGetCPUUsage );
}
