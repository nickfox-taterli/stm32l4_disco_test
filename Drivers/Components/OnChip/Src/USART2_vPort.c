#include "USART2_vPort.h"

static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/* USART2 init function */
void xSerialPortInitMinimal(unsigned portBASE_TYPE uxQueueLength )
{
    /* Create the queues used to hold Rx/Tx characters. */
    xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
    xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

    LL_USART_InitTypeDef USART_InitStruct;

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);

    /**USART2 GPIO Configuration
    PD5   ------> USART2_TX
    PD6   ------> USART2_RX
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    NVIC_SetPriority(USART2_IRQn, 0x0F);
    NVIC_EnableIRQ(USART2_IRQn);

    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART2, &USART_InitStruct);

    LL_USART_ConfigAsyncMode(USART2);

    LL_USART_Enable(USART2);

    LL_USART_EnableIT_RXNE( USART2 );

}

signed portBASE_TYPE xSerialGetChar(signed char *pcRxedChar, TickType_t xBlockTime )
{
    /* Get the next character from the buffer.  Return false if no characters
    are available, or arrive before xBlockTime expires. */
    if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
    {
        return pdTRUE;
    }
    else
    {
        return pdFALSE;
    }
}

void vSerialPutString(const signed char *const pcString, unsigned short usStringLength )
{
    signed char *pxNext;

    /* Send each character in the string, one at a time. */
    pxNext = ( signed char * ) pcString;
    while( *pxNext )
    {
        xSerialPutChar(*pxNext, ( ( TickType_t ) 0 ) );
        pxNext++;
    }
}

signed portBASE_TYPE xSerialPutChar(signed char cOutChar, TickType_t xBlockTime )
{
    signed portBASE_TYPE xReturn;

    if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) == pdPASS )
    {
        xReturn = pdPASS;
        LL_USART_EnableIT_TXE( USART2 );
    }
    else
    {
        xReturn = pdFAIL;
    }

    return xReturn;
}

void vUARTInterruptHandler( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    char cChar;

    if(LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
    {
        /* The interrupt was caused by the THR becoming empty.  Are there any
        more characters to transmit? */
        if( xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
        {
            /* A character was retrieved from the queue so can be sent to the
            THR now. */
            LL_USART_TransmitData8( USART2, cChar );
        }
        else
        {
            LL_USART_DisableIT_TXE( USART2 );
        }
    }

    if(LL_USART_IsEnabledIT_RXNE(USART2) && LL_USART_IsActiveFlag_RXNE(USART2))
    {
        cChar = LL_USART_ReceiveData8( USART2 );
        xQueueSendFromISR( xRxedChars, &cChar, &xHigherPriorityTaskWoken );
    }

    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
