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
static void MEMS_SPI2_Init(void)
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



void L3GD20_Init(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;

    /* Configure the low level interface */
    MEMS_SPI2_Init();

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

    /* Configure the low level interface */
    MEMS_SPI2_Init();

    /* Read WHO I AM register */
    GYRO_IO_Read(&tmp, L3GD20_WHO_AM_I_ADDR, 1);

    /* Return the ID */
    return (uint8_t)tmp;
}

/**
  * @brief  Read LSM303C ID.
  * @param  None
  * @retval ID
  */
uint8_t LSM303C_AccReadID(void)
{
    uint8_t ctrl = 0x00;

    /* Low level init */
    MEMS_SPI2_Init();

    /* Enabled SPI/I2C read communication */
    ACCELERO_IO_Write(LSM303C_CTRL_REG4_A, 0x5);

    /* Read value at Who am I register address */
    ctrl = ACCELERO_IO_Read(LSM303C_WHO_AM_I_ADDR);

    return ctrl;
}