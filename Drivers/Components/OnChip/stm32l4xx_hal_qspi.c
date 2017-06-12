#include "stm32l4xx_hal.h"


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/** @defgroup QSPI_Private_Constants QSPI Private Constants
  * @{
  */
#define QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE ((uint32_t)0x00000000)          /*!<Indirect write mode*/
#define QSPI_FUNCTIONAL_MODE_INDIRECT_READ  ((uint32_t)QUADSPI_CCR_FMODE_0) /*!<Indirect read mode*/
#define QSPI_FUNCTIONAL_MODE_AUTO_POLLING   ((uint32_t)QUADSPI_CCR_FMODE_1) /*!<Automatic polling mode*/
#define QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED  ((uint32_t)QUADSPI_CCR_FMODE)   /*!<Memory-mapped mode*/
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/** @defgroup QSPI_Private_Macros QSPI Private Macros
  * @{
  */
#define IS_QSPI_FUNCTIONAL_MODE(MODE) (((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE) || \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_INDIRECT_READ)  || \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_AUTO_POLLING)   || \
                                       ((MODE) == QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))
/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void QSPI_Config(QSPI_CommandTypeDef *cmd, uint32_t FunctionalMode);

/* Exported functions --------------------------------------------------------*/

/** @defgroup QSPI_Exported_Functions QSPI Exported Functions
  * @{
  */

/** @defgroup QSPI_Exported_Functions_Group1 Initialization/de-initialization functions
  *  @brief    Initialization and Configuration functions
  *
@verbatim
===============================================================================
            ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Initialize the QuadSPI.
      (+) De-initialize the QuadSPI.

@endverbatim
  * @{
  */

/**
  * @brief Initialize the QSPI mode according to the specified parameters
  *        in the QSPI_InitTypeDef and initialize the associated handle.
  * @param hqspi: QSPI handle
  * @retval HAL status
  */
void QSPI_MspInit(uint8_t FifoThreshold, uint8_t ClockPrescaler, uint32_t SampleShifting, uint8_t FlashSize, uint32_t ChipSelectHighTime, uint32_t ClockMode)
{
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable the QuadSPI memory interface clock */
    LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_QSPI);
    /* Reset the QuadSPI memory interface */
    SET_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_QSPIRST);
    CLEAR_BIT(RCC->AHB3RSTR, RCC_AHB3RSTR_QSPIRST);
    /* Enable GPIO clocks */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE);

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* QSPI CS GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_11, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

    /* QSPI CLK GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_10, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_10, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_10, LL_GPIO_PULL_NO);

    /* QSPI D0 GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_12, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_12, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_12, LL_GPIO_PULL_NO);

    /* QSPI D1 GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_13, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_13, LL_GPIO_PULL_NO);

    /* QSPI D2 GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_14, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_14, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_14, LL_GPIO_PULL_NO);

    /* QSPI D3 GPIO pin configuration  */
    LL_GPIO_SetPinMode(GPIOE, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOE, LL_GPIO_PIN_15, LL_GPIO_AF_10);
    LL_GPIO_SetPinSpeed(GPIOE, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOE, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);


    /* Configure QSPI FIFO Threshold */
    MODIFY_REG(QUADSPI->CR, QUADSPI_CR_FTHRES,
               ((FifoThreshold - 1) << POSITION_VAL(QUADSPI_CR_FTHRES)));

    /* Wait till BUSY flag reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET);


    MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PRESCALER | QUADSPI_CR_SSHIFT),
               ((ClockPrescaler << POSITION_VAL(QUADSPI_CR_PRESCALER)) |
                SampleShifting));

    /* Configure QSPI Flash Size, CS High Time and Clock Mode */
    MODIFY_REG(QUADSPI->DCR, (QUADSPI_DCR_FSIZE | QUADSPI_DCR_CSHT | QUADSPI_DCR_CKMODE),
               ((FlashSize << POSITION_VAL(QUADSPI_DCR_FSIZE)) |
                ChipSelectHighTime | ClockMode));

    /* Enable the QSPI peripheral */
    SET_BIT(QUADSPI->CR, QUADSPI_CR_EN);

    /* Write to DMA Channel CR register */
    DMA2_Channel7->CCR = 0x00000080;

    /* Reset request selection for DMA2 Channelx */
    DMA2_CSELR->CSELR &= ~(DMA_CSELR_C1S << 0x00000018);

    /* Configure request selection for DMA2 Channelx */
    DMA2_CSELR->CSELR |= (uint32_t) (DMA_REQUEST_3 << 0x00000018);

    // NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
    // NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    // NVIC_SetPriority(QUADSPI_IRQn, 0x0F);
    //NVIC_EnableIRQ(QUADSPI_IRQn);
}
/**
  * @brief DeInitialize the QSPI MSP.
  * @param hqspi: QSPI handle
  * @retval None
  */


/**
  * @}
  */

/** @defgroup QSPI_Exported_Functions_Group2 Input and Output operation functions
  *  @brief QSPI Transmit/Receive functions
  *
@verbatim
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Handle the interrupts.
      (+) Handle the command sequence.
      (+) Transmit data in blocking, interrupt or DMA mode.
      (+) Receive data in blocking, interrupt or DMA mode.
      (+) Manage the auto-polling functional mode.
      (+) Manage the memory-mapped functional mode.

@endverbatim
  * @{
  */

/**
  * @brief Set the command configuration.
  * @param hqspi: QSPI handle
  * @param cmd : structure that contains the command configuration information
  * @param Timeout : Timeout duration
  * @note   This function is used only in Indirect Read or Write Modes
  * @retval HAL status
  */
void QSPI_Command(QSPI_CommandTypeDef *cmd)
{

    /* Wait till BUSY flag reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET) {}

    /* Call the configuration function */
    QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

    if (cmd->DataMode == QSPI_DATA_NONE)
    {
        /* When there is no data phase, the transfer start as soon as the configuration is done
        so wait until TC flag is set to go back in idle state */
        while((__QSPI_GET_FLAG(QSPI_FLAG_TC)) != SET) {}
        __QSPI_CLEAR_FLAG(QSPI_FLAG_TC);
    }
}


/**
  * @brief Transmit an amount of data in blocking mode.
  * @param pData: pointer to data buffer
  * @param Timeout : Timeout duration
  * @note   This function is used only in Indirect Write Mode
  * @retval HAL status
  */
void QSPI_Transmit(uint8_t *pData)
{
    __IO uint32_t *data_reg = &QUADSPI->DR;

    uint32_t TxXferCount = READ_REG(QUADSPI->DLR) + 1;
    uint32_t TxXferSize = READ_REG(QUADSPI->DLR) + 1;
    uint8_t *pTxBuffPtr = pData;

    /* Configure QSPI: CCR register with functional as indirect write */
    MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);

    while(TxXferCount > 0)
    {
        /* Wait until FT flag is set to send data */
        while((__QSPI_GET_FLAG(QSPI_FLAG_FT)) != SET) {}


        *(__IO uint8_t *)data_reg = *pTxBuffPtr++;
        TxXferCount--;
    }


    /* Wait until TC flag is set to go back in idle state */
    while((__QSPI_GET_FLAG(QSPI_FLAG_FT)) != SET) {}

    /* Clear Transfer Complete bit */
    __QSPI_CLEAR_FLAG(QSPI_FLAG_TC);

    /* Clear Busy bit */
    QSPI_Abort();

}


/**
  * @brief Receive an amount of data in blocking mode.
  * @param pData: pointer to data buffer
  * @param Timeout : Timeout duration
  * @note   This function is used only in Indirect Read Mode
  * @retval HAL status
  */
void QSPI_Receive(uint8_t *pData)
{
    uint32_t addr_reg = READ_REG(QUADSPI->AR);
    __IO uint32_t *data_reg = &QUADSPI->DR;

    /* Configure counters and size of the handle */
    uint32_t RxXferCount = READ_REG(QUADSPI->DLR) + 1;
    uint32_t RxXferSize = READ_REG(QUADSPI->DLR) + 1;
    uint8_t *pRxBuffPtr = pData;

    /* Configure QSPI: CCR register with functional as indirect read */
    MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_READ);

    /* Start the transfer by re-writing the address in AR register */
    WRITE_REG(QUADSPI->AR, addr_reg);

    while(RxXferCount > 0)
    {
        /* Wait until FT or TC flag is set to read received data */
        /* Wait until TC flag is set to go back in idle state */
        while((__QSPI_GET_FLAG((QSPI_FLAG_FT | QSPI_FLAG_TC))) != SET) {}

        *pRxBuffPtr++ = *(__IO uint8_t *)data_reg;
        RxXferCount--;
    }


    /* Wait until TC flag is set to go back in idle state */
    while((__QSPI_GET_FLAG((QSPI_FLAG_TC))) != SET) {}
    /* Clear Transfer Complete bit */
    __QSPI_CLEAR_FLAG(QSPI_FLAG_TC);
    QSPI_Abort();

}


/**
  * @brief  Send an amount of data in non-blocking mode with DMA.
  * @param  hqspi: QSPI handle
  * @param  pData: pointer to data buffer
  * @note   This function is used only in Indirect Write Mode
  * @note   If DMA peripheral access is configured as halfword, the number
  *         of data and the fifo threshold should be aligned on halfword
  * @note   If DMA peripheral access is configured as word, the number
  *         of data and the fifo threshold should be aligned on word
  * @retval HAL status
  */
void QSPI_Transmit_DMA(uint8_t *pData)
{
    uint32_t *tmp = (uint32_t *)&pData;
    uint32_t data_size = (READ_REG(QUADSPI->DLR) + 1);





    /* Clear interrupt */
    __QSPI_CLEAR_FLAG((QSPI_FLAG_TE | QSPI_FLAG_TC));


    /* Configure QSPI: CCR register with functional mode as indirect write */
    MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE);




    /* Disable the peripheral */
    DMA2_Channel7->CCR &=  ~DMA_CCR_EN;



    MODIFY_REG(DMA2_Channel7->CCR, DMA_CCR_DIR, DMA_MEMORY_TO_PERIPH);

    /* Clear all flags */
    DMA2->IFCR = (DMA_ISR_GIF1 << 0x00000018);

    /* Configure DMA Channel data length */
    DMA2_Channel7->CNDTR = data_size;

    /* Configure DMA Channel destination address */
    DMA2_Channel7->CPAR = (uint32_t)&QUADSPI->DR;

    /* Configure DMA Channel source address */
    DMA2_Channel7->CMAR = *(uint32_t *)tmp;

    /* Enable the Half transfer complete interrupt as well */
    //   __HAL_DMA_ENABLE_IT(hqspi->hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    /* Enable the Peripheral */
    // __HAL_DMA_ENABLE(hqspi->hdma);
    DMA2_Channel7->CCR |=  DMA_CCR_EN;

    /* Enable the QSPI transfer error Interrupt */
    //  __QSPI_ENABLE_IT(QSPI_IT_TE);

    /* Enable the DMA transfer by setting the DMAEN bit in the QSPI CR register */
    SET_BIT(QUADSPI->CR, QUADSPI_CR_DMAEN);



}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA.
  * @param  hqspi: QSPI handle
  * @param  pData: pointer to data buffer.
  * @note   This function is used only in Indirect Read Mode
  * @note   If DMA peripheral access is configured as halfword, the number
  *         of data and the fifo threshold should be aligned on halfword
  * @note   If DMA peripheral access is configured as word, the number
  *         of data and the fifo threshold should be aligned on word
  * @retval HAL status
  */
void QSPI_Receive_DMA(uint8_t *pData)
{
    uint32_t *tmp;
    uint32_t addr_reg = READ_REG(QUADSPI->AR);
    uint32_t data_size = (READ_REG(QUADSPI->DLR) + 1);




    /* Clear interrupt */
    __QSPI_CLEAR_FLAG((QSPI_FLAG_TE | QSPI_FLAG_TC));


    /* Configure QSPI: CCR register with functional as indirect read */
    MODIFY_REG(QUADSPI->CCR, QUADSPI_CCR_FMODE, QSPI_FUNCTIONAL_MODE_INDIRECT_READ);
    /* Configure the direction of the DMA */


    /* Enable the DMA Channel */
    tmp = (uint32_t *)&pData;



    DMA2_Channel7->CCR &=  ~DMA_CCR_EN;

    MODIFY_REG(DMA2_Channel7->CCR, DMA_CCR_DIR, DMA_PERIPH_TO_MEMORY);
    /* Clear all flags */
    /* Clear all flags */
    DMA2->IFCR = (DMA_ISR_GIF1 << 0x00000018);

    /* Configure DMA Channel data length */
    DMA2_Channel7->CNDTR = data_size;

    /* Configure DMA Channel source address */
    DMA2_Channel7->CPAR = (uint32_t)&QUADSPI->DR;

    /* Configure DMA Channel destination address */
    DMA2_Channel7->CMAR = *(uint32_t *)tmp;

    /* Enable the Half transfer complete interrupt as well */
    //__HAL_DMA_ENABLE_IT(hdma, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));

    DMA2_Channel7->CCR |=  DMA_CCR_EN;


    /* Start the transfer by re-writing the address in AR register */
    WRITE_REG(QUADSPI->AR, addr_reg);

    /* Enable the QSPI transfer error Interrupt */
    // __QSPI_ENABLE_IT(hqspi, QSPI_IT_TE);

    /* Enable the DMA transfer by setting the DMAEN bit in the QSPI CR register */
    SET_BIT(QUADSPI->CR, QUADSPI_CR_DMAEN);

}

/**
  * @brief  Configure the QSPI Automatic Polling Mode in blocking mode.
  * @param  hqspi: QSPI handle
  * @param  cmd: structure that contains the command configuration information.
  * @param  cfg: structure that contains the polling configuration information.
  * @param  Timeout : Timeout duration
  * @note   This function is used only in Automatic Polling Mode
  * @retval HAL status
  */
void QSPI_AutoPolling(QSPI_CommandTypeDef *cmd, QSPI_AutoPollingTypeDef *cfg)
{


    /* Wait till BUSY flag reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET) {}

    /* Configure QSPI: PSMAR register with the status match value */
    WRITE_REG(QUADSPI->PSMAR, cfg->Match);

    /* Configure QSPI: PSMKR register with the status mask value */
    WRITE_REG(QUADSPI->PSMKR, cfg->Mask);

    /* Configure QSPI: PIR register with the interval value */
    WRITE_REG(QUADSPI->PIR, cfg->Interval);

    /* Configure QSPI: CR register with Match mode and Automatic stop enabled
    (otherwise there will be an infinite loop in blocking mode) */
    MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PMM | QUADSPI_CR_APMS),
               (cfg->MatchMode | QSPI_AUTOMATIC_STOP_ENABLE));

    /* Call the configuration function */
    cmd->NbData = cfg->StatusBytesSize;
    QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_AUTO_POLLING);

    /* Wait until SM flag is set to go back in idle state */
    while((__QSPI_GET_FLAG(QSPI_FLAG_SM)) != SET) {}

    __QSPI_CLEAR_FLAG(QSPI_FLAG_SM);

}

/**
  * @brief  Configure the QSPI Automatic Polling Mode in non-blocking mode.
  * @param  hqspi: QSPI handle
  * @param  cmd: structure that contains the command configuration information.
  * @param  cfg: structure that contains the polling configuration information.
  * @note   This function is used only in Automatic Polling Mode
  * @retval HAL status
  */
void QSPI_AutoPolling_IT(QSPI_CommandTypeDef *cmd, QSPI_AutoPollingTypeDef *cfg)
{


    /* Wait till BUSY flag reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET) {}

    /* Configure QSPI: PSMAR register with the status match value */
    WRITE_REG(QUADSPI->PSMAR, cfg->Match);

    /* Configure QSPI: PSMKR register with the status mask value */
    WRITE_REG(QUADSPI->PSMKR, cfg->Mask);

    /* Configure QSPI: PIR register with the interval value */
    WRITE_REG(QUADSPI->PIR, cfg->Interval);

    /* Configure QSPI: CR register with Match mode and Automatic stop mode */
    MODIFY_REG(QUADSPI->CR, (QUADSPI_CR_PMM | QUADSPI_CR_APMS),
               (cfg->MatchMode | cfg->AutomaticStop));

    /* Clear interrupt */
    __QSPI_CLEAR_FLAG(QSPI_FLAG_TE | QSPI_FLAG_SM);

    /* Call the configuration function */
    cmd->NbData = cfg->StatusBytesSize;
    QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_AUTO_POLLING);


    /* Enable the QSPI Transfer Error and status match Interrupt */
    __QSPI_ENABLE_IT((QSPI_IT_SM | QSPI_IT_TE));

}

/**
  * @brief  Configure the Memory Mapped mode.
  * @param  hqspi: QSPI handle
  * @param  cmd: structure that contains the command configuration information.
  * @param  cfg: structure that contains the memory mapped configuration information.
  * @note   This function is used only in Memory mapped Mode
  * @retval HAL status
  */
void QSPI_MemoryMapped(QSPI_CommandTypeDef *cmd, QSPI_MemoryMappedTypeDef *cfg)
{


    /* Wait till BUSY flag reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET) {}

    /* Configure QSPI: CR register with timeout counter enable */
    MODIFY_REG(QUADSPI->CR, QUADSPI_CR_TCEN, cfg->TimeOutActivation);

    if (cfg->TimeOutActivation == QSPI_TIMEOUT_COUNTER_ENABLE)
    {

        /* Configure QSPI: LPTR register with the low-power timeout value */
        WRITE_REG(QUADSPI->LPTR, cfg->TimeOutPeriod);

        /* Clear interrupt */
        __QSPI_CLEAR_FLAG(QSPI_FLAG_TO);

        /* Enable the QSPI TimeOut Interrupt */
        __QSPI_ENABLE_IT(QSPI_IT_TO);
    }

    /* Call the configuration function */
    QSPI_Config(cmd, QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED);




}


/**
  * @}
  */

/** @defgroup QSPI_Exported_Functions_Group3 Peripheral Control and State functions
  *  @brief   QSPI control and State functions
  *
@verbatim
 ===============================================================================
                  ##### Peripheral Control and State functions #####
 ===============================================================================
    [..]
    This subsection provides a set of functions allowing to :
      (+) Check in run-time the state of the driver.
      (+) Check the error code set during last operation.
      (+) Abort any operation.


@endverbatim
  * @{
  */


/**
* @brief  Abort the current transmission.
* @param  hqspi: QSPI handle
* @retval HAL status
*/
void QSPI_Abort(void)
{
    /* Configure QSPI: CR register with Abort request */
    SET_BIT(QUADSPI->CR, QUADSPI_CR_ABORT);

    /* Wait until TC flag is set to go back in idle state */
    /* Wait until TC flag is set to go back in idle state */
    while((__QSPI_GET_FLAG(QSPI_FLAG_TC)) != SET) {}

    /* Clear Transfer Complete bit */
    __QSPI_CLEAR_FLAG(QSPI_FLAG_TC);

    /* Wait until BUSY flag is reset */
    while((__QSPI_GET_FLAG(QSPI_FLAG_BUSY)) != RESET);

}




/**
  * @brief  Configure the communication registers.
  * @param  cmd: structure that contains the command configuration information
  * @param  FunctionalMode: functional mode to configured
  *           This parameter can be one of the following values:
  *            @arg QSPI_FUNCTIONAL_MODE_INDIRECT_WRITE: Indirect write mode
  *            @arg QSPI_FUNCTIONAL_MODE_INDIRECT_READ: Indirect read mode
  *            @arg QSPI_FUNCTIONAL_MODE_AUTO_POLLING: Automatic polling mode
  *            @arg QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED: Memory-mapped mode
  * @retval None
  */
static void QSPI_Config(QSPI_CommandTypeDef *cmd, uint32_t FunctionalMode)
{
    if ((cmd->DataMode != QSPI_DATA_NONE) && (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED))
    {
        /* Configure QSPI: DLR register with the number of data to read or write */
        WRITE_REG(QUADSPI->DLR, (cmd->NbData - 1));
    }

    if (cmd->InstructionMode != QSPI_INSTRUCTION_NONE)
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            WRITE_REG(QUADSPI->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction, address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressSize | cmd->AddressMode | cmd->InstructionMode |
                                         cmd->Instruction | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(QUADSPI->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with instruction and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressMode | cmd->InstructionMode |
                                         cmd->Instruction | FunctionalMode));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with instruction and address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateByteMode | cmd->AddressSize | cmd->AddressMode |
                                         cmd->InstructionMode | cmd->Instruction | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(QUADSPI->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only instruction ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateByteMode | cmd->AddressMode |
                                         cmd->InstructionMode | cmd->Instruction | FunctionalMode));
            }
        }
    }
    else
    {
        if (cmd->AlternateByteMode != QSPI_ALTERNATE_BYTES_NONE)
        {
            /* Configure QSPI: ABR register with alternate bytes value */
            WRITE_REG(QUADSPI->ABR, cmd->AlternateBytes);

            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with address and alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressSize | cmd->AddressMode |
                                         cmd->InstructionMode | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(QUADSPI->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only alternate bytes ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateBytesSize | cmd->AlternateByteMode |
                                         cmd->AddressMode | cmd->InstructionMode | FunctionalMode));
            }
        }
        else
        {
            if (cmd->AddressMode != QSPI_ADDRESS_NONE)
            {
                /*---- Command with only address ----*/
                /* Configure QSPI: CCR register with all communications parameters */
                WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                         cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                         cmd->AlternateByteMode | cmd->AddressSize |
                                         cmd->AddressMode | cmd->InstructionMode | FunctionalMode));

                if (FunctionalMode != QSPI_FUNCTIONAL_MODE_MEMORY_MAPPED)
                {
                    /* Configure QSPI: AR register with address value */
                    WRITE_REG(QUADSPI->AR, cmd->Address);
                }
            }
            else
            {
                /*---- Command with only data phase ----*/
                if (cmd->DataMode != QSPI_DATA_NONE)
                {
                    /* Configure QSPI: CCR register with all communications parameters */
                    WRITE_REG(QUADSPI->CCR, (cmd->DdrMode | cmd->DdrHoldHalfCycle | cmd->SIOOMode |
                                             cmd->DataMode | (cmd->DummyCycles << POSITION_VAL(QUADSPI_CCR_DCYC)) |
                                             cmd->AlternateByteMode | cmd->AddressMode |
                                             cmd->InstructionMode | FunctionalMode));
                }
            }
        }
    }
}
