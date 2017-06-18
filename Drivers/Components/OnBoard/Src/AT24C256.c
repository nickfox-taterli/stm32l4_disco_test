#include "AT24C256.h"
#include "I2C1.h"

void AT24C256_Init(void){
		if(!LL_I2C_IsEnabled(I2C1)){
			 I2C1_Init();
		}			
}

uint8_t EEPROM_Read(uint16_t reg)
{
    uint8_t pData;

    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, 0xA0, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, (reg >> 8) & 0xFF);
    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, reg & 0xFF);			
    while(!LL_I2C_IsActiveFlag_TC(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, 0xA0, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_RESTART_7BIT_READ);
    while(!LL_I2C_IsActiveFlag_RXNE(I2C1)) {};

    pData = LL_I2C_ReceiveData8(I2C1);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1)) {};

    LL_I2C_ClearFlag_STOP(I2C1);

    return pData;
}

void EEPROM_Write(uint16_t reg, uint8_t value)
{
    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, 0xA0, LL_I2C_ADDRSLAVE_7BIT, 3, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, (reg >> 8) & 0xFF);
    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, reg & 0xFF);		
    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, value);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1)) {};

    LL_I2C_ClearFlag_STOP(I2C1);
}
