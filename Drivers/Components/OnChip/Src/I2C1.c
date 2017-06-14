#include "I2C1.h"

void I2C1_Init(void)
{

    LL_I2C_InitTypeDef I2C_InitStruct;

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

    /**I2C1 GPIO Configuration
    PB6   ------> I2C1_SCL
    PB7   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /**I2C Initialization ≈ 80kHz
    */
    I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing = 0x90112626;
    I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter = 0;
    I2C_InitStruct.OwnAddress1 = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init(I2C1, &I2C_InitStruct);

    LL_I2C_EnableAutoEndMode(I2C1);

    LL_I2C_DisableOwnAddress2(I2C1);

    LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);

    LL_I2C_DisableGeneralCall(I2C1);

    LL_I2C_EnableClockStretching(I2C1);

}

uint8_t I2C1_Master_Read(uint8_t device, uint8_t reg)
{
    uint8_t pData;

    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, device, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, reg);
    while(!LL_I2C_IsActiveFlag_TC(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, device, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_RESTART_7BIT_READ);
    while(!LL_I2C_IsActiveFlag_RXNE(I2C1)) {};

    pData = LL_I2C_ReceiveData8(I2C1);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1)) {};

    LL_I2C_ClearFlag_STOP(I2C1);

    return pData;
}


void I2C1_Master_Write(uint8_t device, uint8_t reg, uint8_t value)
{
    while(LL_I2C_IsActiveFlag_BUSY(I2C1)) {};

    LL_I2C_HandleTransfer(I2C1, device, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, reg);
    while(!LL_I2C_IsActiveFlag_TXIS(I2C1)) {};
    LL_I2C_TransmitData8(I2C1, value);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1)) {};

    LL_I2C_ClearFlag_STOP(I2C1);
}
