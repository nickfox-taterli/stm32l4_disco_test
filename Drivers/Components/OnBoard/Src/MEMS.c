#include "MEMS.h"

#define ACCELERO_CS_LOW()   LL_GPIO_ResetOutputPin(GPIOE,LL_GPIO_PIN_0)
#define ACCELERO_CS_HIGH()  LL_GPIO_SetOutputPin(GPIOE,LL_GPIO_PIN_0)

#define MAGNETO_CS_LOW()   LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0)
#define MAGNETO_CS_HIGH()  LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_0)

#define GYRO_CS_LOW()   LL_GPIO_ResetOutputPin(GPIOD, LL_GPIO_PIN_7)
#define GYRO_CS_HIGH()  LL_GPIO_SetOutputPin(GPIOD, LL_GPIO_PIN_7)

/* Read/Write command */
#define READWRITE_CMD                           ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD                        ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                              ((uint8_t)0x00)

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval none.
  */
static uint8_t MEMS_WriteRead(uint8_t Byte)
{
    uint8_t receivedbyte;


    LL_SPI_Enable(SPI2);
    /* check TXE flag */
    while(!LL_SPI_IsActiveFlag_TXE(SPI2)) {}

    /* Write the data */
    LL_SPI_TransmitData8(SPI2, Byte);

    while(!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}
    receivedbyte = LL_SPI_ReceiveData8(SPI2);

    /* Wait BSY flag */
    while(LL_SPI_GetTxFIFOLevel(SPI2) != LL_SPI_TX_FIFO_EMPTY) {}
    while(LL_SPI_IsActiveFlag_BSY(SPI2)) {}
    LL_SPI_Disable(SPI2);
    return receivedbyte;
}

/**
  * @brief  Sends a Byte through the SPI interface.
  * @param  Byte : Byte to send.
  * @retval none.
  */
static void MEMS_Write(uint8_t Byte)
{

    LL_SPI_Enable(SPI2);
    /* check TXE flag */
    while(!LL_SPI_IsActiveFlag_TXE(SPI2)) {}

    /* Write the data */
    LL_SPI_TransmitData8(SPI2, Byte);

    /* Wait BSY flag */
    while(LL_SPI_IsActiveFlag_BSY(SPI2)) {}
    LL_SPI_Disable(SPI2);

}

static uint8_t MEMS_Read(void)
{
    uint8_t receivedbyte;

    LL_SPI_Enable(SPI2);
    LL_SPI_Disable(SPI2);

    while(!LL_SPI_IsActiveFlag_RXNE(SPI2)) {}

    /* read the received data */

    receivedbyte = LL_SPI_ReceiveData8(SPI2);

    /* Wait for the BSY flag reset */
    while(LL_SPI_IsActiveFlag_BSY(SPI2)) {}



    return receivedbyte;
}

/**
  * @brief  Writes one byte to the COMPASS/MAGNETO.
  * @param  RegisterAddr specifies the COMPASS/MAGNETO register to be written.
  * @param  Value : Data to be written
  * @retval   None
 */
static void MAGNETO_IO_Write(uint8_t RegisterAddr, uint8_t Value)
{
    MAGNETO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_TX);
    /* call SPI Read data bus function */
    MEMS_Write(RegisterAddr);
    MEMS_Write(Value);
    MAGNETO_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the COMPASS/MAGNETO.
  * @param  RegisterAddr : specifies the COMPASS/MAGNETO internal address register to read from
  * @retval ACCELEROMETER register value
  */
static uint8_t MAGNETO_IO_Read(uint8_t RegisterAddr)
{
    MAGNETO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_TX);
    MEMS_Write(RegisterAddr | 0x80);
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_RX);
    uint8_t val = MEMS_Read();
    MAGNETO_CS_HIGH();
    return val;
}

/**
  * @brief  Writes one byte to the COMPASS / ACCELEROMETER.
  * @param  RegisterAddr specifies the COMPASS / ACCELEROMETER register to be written.
  * @param  Value : Data to be written
  * @retval   None
 */
static void ACCELERO_IO_Write(uint8_t RegisterAddr, uint8_t Value)
{
    ACCELERO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_TX);
    /* call SPI Read data bus function */
    MEMS_Write(RegisterAddr);
    MEMS_Write(Value);
    ACCELERO_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the COMPASS / ACCELEROMETER.
  * @param  RegisterAddr : specifies the COMPASS / ACCELEROMETER internal address register to read from
  * @retval ACCELEROMETER register value
  */
static uint8_t ACCELERO_IO_Read(uint8_t RegisterAddr)
{
    RegisterAddr = RegisterAddr | ((uint8_t)0x80);
    ACCELERO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_TX);
    MEMS_Write(RegisterAddr);
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_HALF_DUPLEX_RX);
    uint8_t val = MEMS_Read();
    ACCELERO_CS_HIGH();
    return val;
}

/* MEMS SPI init function */
void MEMS_SPI2_Init(void)
{

    LL_SPI_InitTypeDef SPI_InitStruct;

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    /* GPIO Ports Clock Enable */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    /**SPI2 GPIO Configuration
    PD1   ------> SPI2_SCK
    PD3   ------> SPI2_MISO
    PD4   ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* PC0 = MAG_CS */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PC1 = MAG_INT PC2 = MAG_DRDY */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1 | LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* PD2 = GYRO_INT1 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* PD7 = GYRO_CS */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* PB8 = GYRO_INT2 */
    GPIO_InitStruct.Pin =  LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PE0 = XL_CS */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* PE1 XL_INT */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;//LL_SPI_BAUDRATEPRESCALER_DIV8
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI2, &SPI_InitStruct);
    LL_SPI_SetRxFIFOThreshold(SPI2, LL_SPI_RX_FIFO_TH_QUARTER);


    ACCELERO_CS_HIGH();
    MAGNETO_CS_HIGH();
    GYRO_CS_HIGH();
}


/**
  * @brief  Writes one byte to the GYRO.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the GYRO.
  * @param  WriteAddr : GYRO's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
static void GYRO_IO_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
    /* Configure the MS bit:
         - When 0, the address will remain unchanged in multiple read/write commands.
         - When 1, the address will be auto incremented in multiple read/write commands.
    */
    if(NumByteToWrite > 0x01)
    {
        WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
    }
    /* Set chip select Low at the start of the transmission */
    GYRO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);

    /* Send the Address of the indexed register */
    MEMS_WriteRead(WriteAddr);

    /* Send the data that will be written into the device (MSB First) */
    while(NumByteToWrite >= 0x01)
    {
        MEMS_WriteRead(*pBuffer);
        NumByteToWrite--;
        pBuffer++;
    }

    /* Set chip select High at the end of the transmission */
    GYRO_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the GYROSCOPE.
  * @param  pBuffer : pointer to the buffer that receives the data read from the GYROSCOPE.
  * @param  ReadAddr : GYROSCOPE's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the GYROSCOPE.
  * @retval None
  */
static void GYRO_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    if(NumByteToRead > 0x01)
    {
        ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
    }
    else
    {
        ReadAddr |= (uint8_t)READWRITE_CMD;
    }
    /* Set chip select Low at the start of the transmission */
    GYRO_CS_LOW();
    LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);
    /* Send the Address of the indexed register */
    MEMS_WriteRead(ReadAddr);

    /* Receive the data that will be read from the device (MSB First) */
    while(NumByteToRead > 0x00)
    {
        /* Send dummy byte (0x00) to generate the SPI clock to GYROSCOPE (Slave device) */
        *pBuffer = MEMS_WriteRead(0x00);
        NumByteToRead--;
        pBuffer++;
    }

    /* Set chip select High at the end of the transmission */
    GYRO_CS_HIGH();
}

/**
  * @brief  Set L3GD20 Initialization.
  * @param  L3GD20_InitStruct: pointer to a L3GD20_InitTypeDef structure
  *         that contains the configuration setting for the L3GD20.
  * @retval None
  */
void L3GD20_Init(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;

    /* Write value to MEMS CTRL_REG1 register */
    ctrl = (uint8_t) InitStruct;
    GYRO_IO_Write(&ctrl, L3GD20_CTRL_REG1_ADDR, 1);

    /* Write value to MEMS CTRL_REG4 register */
    ctrl = (uint8_t) (InitStruct >> 8);
    GYRO_IO_Write(&ctrl, L3GD20_CTRL_REG4_ADDR, 1);
}

/**
  * @brief  Read ID address of L3GD20
  * @param  None
  * @retval ID name
  */
uint8_t L3GD20_ReadID(void)
{
    uint8_t tmp;

    /* Read WHO I AM register */
    GYRO_IO_Read(&tmp, L3GD20_WHO_AM_I_ADDR, 1);

    /* Return the ID */
    return (uint8_t)tmp;
}

/**
  * @brief  Reboot memory content of L3GD20
  * @param  None
  * @retval None
  */
void L3GD20_RebootCmd(void)
{
    uint8_t tmpreg;

    /* Read CTRL_REG5 register */
    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG5_ADDR, 1);

    /* Enable or Disable the reboot memory */
    tmpreg |= L3GD20_BOOT_REBOOTMEMORY;

    /* Write value to MEMS CTRL_REG5 register */
    GYRO_IO_Write(&tmpreg, L3GD20_CTRL_REG5_ADDR, 1);
}

/**
  * @brief Set L3GD20 in low-power mode
  * @param
  * @retval  None
  */
void L3GD20_LowPower(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;

    /* Write value to MEMS CTRL_REG1 register */
    ctrl = (uint8_t) InitStruct;
    GYRO_IO_Write(&ctrl, L3GD20_CTRL_REG1_ADDR, 1);
}

/**
  * @brief  Set L3GD20 Interrupt INT1 configuration
  * @param  Int1Config: the configuration setting for the L3GD20 Interrupt.
  * @retval None
  */
void L3GD20_INT1InterruptConfig(uint16_t Int1Config)
{
    uint8_t ctrl_cfr = 0x00, ctrl3 = 0x00;

    /* Read INT1_CFG register */
    GYRO_IO_Read(&ctrl_cfr, L3GD20_INT1_CFG_ADDR, 1);

    /* Read CTRL_REG3 register */
    GYRO_IO_Read(&ctrl3, L3GD20_CTRL_REG3_ADDR, 1);

    ctrl_cfr &= 0x80;
    ctrl_cfr |= ((uint8_t) Int1Config >> 8);

    ctrl3 &= 0xDF;
    ctrl3 |= ((uint8_t) Int1Config);

    /* Write value to MEMS INT1_CFG register */
    GYRO_IO_Write(&ctrl_cfr, L3GD20_INT1_CFG_ADDR, 1);

    /* Write value to MEMS CTRL_REG3 register */
    GYRO_IO_Write(&ctrl3, L3GD20_CTRL_REG3_ADDR, 1);
}

/**
  * @brief  Enable INT1 or INT2 interrupt
  * @param  IntSel: choice of INT1 or INT2
  *      This parameter can be:
  *        @arg L3GD20_INT1
  *        @arg L3GD20_INT2
  * @retval None
  */
void L3GD20_EnableIT(uint8_t IntSel)
{
    uint8_t tmpreg;

    /* Read CTRL_REG3 register */
    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG3_ADDR, 1);

    if(IntSel == L3GD20_INT1)
    {
        tmpreg &= 0x7F;
        tmpreg |= L3GD20_INT1INTERRUPT_ENABLE;
    }
    else if(IntSel == L3GD20_INT2)
    {
        tmpreg &= 0xF7;
        tmpreg |= L3GD20_INT2INTERRUPT_ENABLE;
    }

    /* Write value to MEMS CTRL_REG3 register */
    GYRO_IO_Write(&tmpreg, L3GD20_CTRL_REG3_ADDR, 1);
}

/**
  * @brief  Disable  INT1 or INT2 interrupt
  * @param  IntSel: choice of INT1 or INT2
  *      This parameter can be:
  *        @arg L3GD20_INT1
  *        @arg L3GD20_INT2
  * @retval None
  */
void L3GD20_DisableIT(uint8_t IntSel)
{
    uint8_t tmpreg;

    /* Read CTRL_REG3 register */
    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG3_ADDR, 1);

    if(IntSel == L3GD20_INT1)
    {
        tmpreg &= 0x7F;
        tmpreg |= L3GD20_INT1INTERRUPT_DISABLE;
    }
    else if(IntSel == L3GD20_INT2)
    {
        tmpreg &= 0xF7;
        tmpreg |= L3GD20_INT2INTERRUPT_DISABLE;
    }

    /* Write value to MEMS CTRL_REG3 register */
    GYRO_IO_Write(&tmpreg, L3GD20_CTRL_REG3_ADDR, 1);
}

/**
  * @brief  Set High Pass Filter Modality
  * @param  FilterStruct: contains the configuration setting for the L3GD20.
  * @retval None
  */
void L3GD20_FilterConfig(uint8_t FilterStruct)
{
    uint8_t tmpreg;

    /* Read CTRL_REG2 register */
    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG2_ADDR, 1);

    tmpreg &= 0xC0;

    /* Configure MEMS: mode and cutoff frequency */
    tmpreg |= FilterStruct;

    /* Write value to MEMS CTRL_REG2 register */
    GYRO_IO_Write(&tmpreg, L3GD20_CTRL_REG2_ADDR, 1);
}

/**
  * @brief  Enable or Disable High Pass Filter
  * @param  HighPassFilterState: new state of the High Pass Filter feature.
  *      This parameter can be:
  *         @arg: L3GD20_HIGHPASSFILTER_DISABLE
  *         @arg: L3GD20_HIGHPASSFILTER_ENABLE
  * @retval None
  */
void L3GD20_FilterCmd(uint8_t HighPassFilterState)
{
    uint8_t tmpreg;

    /* Read CTRL_REG5 register */
    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG5_ADDR, 1);

    tmpreg &= 0xEF;

    tmpreg |= HighPassFilterState;

    /* Write value to MEMS CTRL_REG5 register */
    GYRO_IO_Write(&tmpreg, L3GD20_CTRL_REG5_ADDR, 1);
}

/**
  * @brief  Get status for L3GD20 data
  * @param  None
  * @retval Data status in a L3GD20 Data
  */
uint8_t L3GD20_GetDataStatus(void)
{
    uint8_t tmpreg;

    /* Read STATUS_REG register */
    GYRO_IO_Read(&tmpreg, L3GD20_STATUS_REG_ADDR, 1);

    return tmpreg;
}

/**
* @brief  Calculate the L3GD20 angular data.
* @param  pfData: Data out pointer
* @retval None
*/
void L3GD20_ReadXYZAngRate(float *pfData)
{
    uint8_t tmpbuffer[6] = {0};
    int16_t RawData[3] = {0};
    uint8_t tmpreg = 0;
    float sensitivity = 0;
    int i = 0;

    GYRO_IO_Read(&tmpreg, L3GD20_CTRL_REG4_ADDR, 1);

    GYRO_IO_Read(tmpbuffer, L3GD20_OUT_X_L_ADDR, 6);

    /* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
    if(!(tmpreg & L3GD20_BLE_MSB))
    {
        for(i = 0; i < 3; i++)
        {
            RawData[i] = (int16_t)(((uint16_t)tmpbuffer[2 * i + 1] << 8) + tmpbuffer[2 * i]);
        }
    }
    else
    {
        for(i = 0; i < 3; i++)
        {
            RawData[i] = (int16_t)(((uint16_t)tmpbuffer[2 * i] << 8) + tmpbuffer[2 * i + 1]);
        }
    }

    /* Switch the sensitivity value set in the CRTL4 */
    switch(tmpreg & L3GD20_FULLSCALE_SELECTION)
    {
    case L3GD20_FULLSCALE_250:
        sensitivity = L3GD20_SENSITIVITY_250DPS;
        break;

    case L3GD20_FULLSCALE_500:
        sensitivity = L3GD20_SENSITIVITY_500DPS;
        break;

    case L3GD20_FULLSCALE_2000:
        sensitivity = L3GD20_SENSITIVITY_2000DPS;
        break;
    }
    /* Divide by sensitivity */
    for(i = 0; i < 3; i++)
    {
        pfData[i] = (float)(RawData[i] * sensitivity);
    }
}

/**
  * @brief  Set LSM303C Accelerometer Initialization.
  * @param  InitStruct: Init parameters
  * @retval None
  */
void LSM303C_AccInit(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;

    /* Write value to ACC MEMS CTRL_REG1 register */
    ctrl = (uint8_t) InitStruct;
    ACCELERO_IO_Write(LSM303C_CTRL_REG1_A, ctrl);

    /* Write value to ACC MEMS CTRL_REG4 register */
    ctrl = ((uint8_t) (InitStruct >> 8));
    ACCELERO_IO_Write(LSM303C_CTRL_REG4_A, ctrl);
}

/**
  * @brief  Read LSM303C ID.
  * @param  None
  * @retval ID
  */
uint8_t LSM303C_AccReadID(void)
{
    uint8_t ctrl = 0x00;

    /* Enabled SPI/I2C read communication */
    ACCELERO_IO_Write(LSM303C_CTRL_REG4_A, 0x5);

    /* Read value at Who am I register address */
    ctrl = ACCELERO_IO_Read(LSM303C_WHO_AM_I_ADDR);

    return ctrl;
}

/**
  * @brief  Put Accelerometer in power down mode or not.
  * @param  Mode equal to LSM303C_ACC_ODR_OFF means enable Low Power Mode, otherwise Output data rate is set.
  *         This parameter can be a value of @ref Acc_OutPut_DataRate_Selection
  * @retval None
  */
void LSM303C_AccLowPower(uint16_t Mode)
{
    uint8_t ctrl = 0x00;

    /* Read control register 1 value */
    ctrl = ACCELERO_IO_Read(LSM303C_CTRL_REG1_A);

    /* Clear ODR bits */
    ctrl &= ~(LSM303C_ACC_ODR_BITPOSITION);

    /* Set Power down */
    ctrl |= (uint8_t)Mode;

    /* write back control register */
    ACCELERO_IO_Write(LSM303C_CTRL_REG1_A, ctrl);
}

/**
  * @brief  Set High Pass Filter Modality
  * @param  FilterStruct: contains data for filter config
  * @retval None
  */
void LSM303C_AccFilterConfig(uint8_t FilterStruct)
{
    uint8_t tmpreg;

    //  /* Read CTRL_REG2 register */
    //  tmpreg = ACCELERO_IO_Read(LSM303C_CTRL_REG2_A);
    //
    //  tmpreg &= 0x0C;
    tmpreg = FilterStruct;

    /* Write value to ACC MEMS CTRL_REG2 register */
    ACCELERO_IO_Write(LSM303C_CTRL_REG2_A, tmpreg);
}

/**
  * @brief  Read X, Y & Z Acceleration values
  * @param  pData: Data out pointer
  * @retval None
  */
void LSM303C_AccReadXYZ(int16_t *pData)
{
    int16_t pnRawData[3];
    uint8_t ctrlx[2] = {0, 0};
    uint8_t buffer[6];
    uint8_t i = 0;
    uint8_t sensitivity = LSM303C_ACC_SENSITIVITY_2G;

    /* Read the acceleration control register content */
    ctrlx[0] = ACCELERO_IO_Read(LSM303C_CTRL_REG4_A);
    ctrlx[1] = ACCELERO_IO_Read(LSM303C_CTRL_REG5_A);

    /* Read output register X, Y & Z acceleration */
    buffer[0] = ACCELERO_IO_Read(LSM303C_OUT_X_L_A);
    buffer[1] = ACCELERO_IO_Read(LSM303C_OUT_X_H_A);
    buffer[2] = ACCELERO_IO_Read(LSM303C_OUT_Y_L_A);
    buffer[3] = ACCELERO_IO_Read(LSM303C_OUT_Y_H_A);
    buffer[4] = ACCELERO_IO_Read(LSM303C_OUT_Z_L_A);
    buffer[5] = ACCELERO_IO_Read(LSM303C_OUT_Z_H_A);

    for(i = 0; i < 3; i++)
    {
        pnRawData[i] = ((int16_t)((uint16_t)buffer[2 * i + 1] << 8) + buffer[2 * i]);
    }

    /* Normal mode */
    /* Switch the sensitivity value set in the CRTL4 */
    switch(ctrlx[0] & LSM303C_ACC_FULLSCALE_8G)
    {
    case LSM303C_ACC_FULLSCALE_2G:
        sensitivity = LSM303C_ACC_SENSITIVITY_2G;
        break;
    case LSM303C_ACC_FULLSCALE_4G:
        sensitivity = LSM303C_ACC_SENSITIVITY_4G;
        break;
    case LSM303C_ACC_FULLSCALE_8G:
        sensitivity = LSM303C_ACC_SENSITIVITY_8G;
        break;
    }

    /* Obtain the mg value for the three axis */
    for(i = 0; i < 3; i++)
    {
        pData[i] = (pnRawData[i] * sensitivity);
    }
}

/***********************************************************************************************
  Magnetometer driver
***********************************************************************************************/

/**
  * @brief  Set LSM303C Magnetometer Initialization.
  * @param  LSM303C_InitStruct: pointer to a LSM303C_MagInitTypeDef structure
  *         that contains the configuration setting for the LSM303C.
  * @retval None
  */
void LSM303C_MagInit(MAGNETO_InitTypeDef LSM303C_InitStruct)
{
    MAGNETO_IO_Write(LSM303C_CTRL_REG1_M, LSM303C_InitStruct.Register1);
    MAGNETO_IO_Write(LSM303C_CTRL_REG2_M, LSM303C_InitStruct.Register2);
    MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, LSM303C_InitStruct.Register3);
    MAGNETO_IO_Write(LSM303C_CTRL_REG4_M, LSM303C_InitStruct.Register4);
    MAGNETO_IO_Write(LSM303C_CTRL_REG5_M, LSM303C_InitStruct.Register5);
}


/**
  * @brief  Read LSM303C ID.
  * @param  None
  * @retval ID
  */
uint8_t LSM303C_MagReadID(void)
{

    /* Enabled the SPI/I2C read operation */
    MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, 0x84);

    /* Read value at Who am I register address */
    return MAGNETO_IO_Read(LSM303C_WHO_AM_I_ADDR);
}

/**
  * @brief  Put Magnetometer in power down mode or not.
  * @param  Mode equal to LSM303C_MAG_POWERDOWN2_MODE means enable deepest Low Power Mode, otherwise other mode is set.
  *         This parameter can be a value of @ref Mag_Operation_Mode
  * @retval None
  */
void LSM303C_MagLowPower(uint16_t Mode)
{
    uint8_t ctrl = 0x00;

    /* Read control register 1 value */
    ctrl = MAGNETO_IO_Read(LSM303C_CTRL_REG3_M);

    /* Clear ODR bits */
    ctrl &= ~(LSM303C_MAG_SELECTION_MODE);

    /* Set mode */
    ctrl |= (uint8_t)Mode;

    /* write back control register */
    MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, ctrl);
}

/**
  * @brief  Get status for Mag LSM303C data
  * @param  None
  * @retval Data status in a LSM303C Data register
  */
uint8_t LSM303C_MagGetDataStatus(void)
{
    /* Read Mag STATUS register */
    return MAGNETO_IO_Read(LSM303C_STATUS_REG_M);
}

/**
  * @brief  Read X, Y & Z Magnetometer values
  * @param  pData: Data out pointer
  * @retval None
  */
void LSM303C_MagReadXYZ(int16_t *pData)
{
    uint8_t ctrlx;
    uint8_t buffer[6];
    uint8_t i = 0;

    /* Read the magnetometer control register content */
    ctrlx = MAGNETO_IO_Read(LSM303C_CTRL_REG4_M);

    /* Read output register X, Y & Z magnetometer */
    buffer[0] = MAGNETO_IO_Read(LSM303C_OUT_X_L_M);
    buffer[1] = MAGNETO_IO_Read(LSM303C_OUT_X_H_M);
    buffer[2] = MAGNETO_IO_Read(LSM303C_OUT_Y_L_M);
    buffer[3] = MAGNETO_IO_Read(LSM303C_OUT_Y_H_M);
    buffer[4] = MAGNETO_IO_Read(LSM303C_OUT_Z_L_M);
    buffer[5] = MAGNETO_IO_Read(LSM303C_OUT_Z_H_M);

    /* Check in the control register4 the data alignment*/
    if((ctrlx & LSM303C_MAG_BLE_MSB))
    {
        for(i = 0; i < 3; i++)
        {
            pData[i] = ((int16_t)((uint16_t)buffer[2 * i] << 8) + buffer[2 * i + 1]);
        }
    }
    else
    {
        for(i = 0; i < 3; i++)
        {
            pData[i] = ((int16_t)((uint16_t)buffer[2 * i + 1] << 8) + buffer[2 * i]);
        }
    }
}
