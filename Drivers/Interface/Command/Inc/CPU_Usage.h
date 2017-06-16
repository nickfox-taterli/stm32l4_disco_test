#ifndef _CPU_UTILS_H__
#define _CPU_UTILS_H__

#include "stm32l4xx.h"

#define CALCULATION_PERIOD    1000

void CPU_Usage_Init(void);

void trace_StartIdleMonitor (void);
void trace_EndIdleMonitor (void);

#endif
