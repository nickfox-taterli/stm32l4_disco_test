#ifndef __OFF_BOARD_AT24C256_H
#define __OFF_BOARD_AT24C256_H

#include "stm32l4xx.h"

void AT24C256_Init(void);
uint8_t EEPROM_Read(uint16_t reg);
void EEPROM_Write(uint16_t reg, uint8_t value);

#endif
