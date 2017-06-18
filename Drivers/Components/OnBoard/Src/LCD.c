#include "LCD.h"


/* Constant table for cap characters 'A' --> 'Z' */
const uint16_t CapLetterMap[26] =
{
    /* A      B      C      D      E      F      G      H      I  */
    0xFE00, 0x6714, 0x1D00, 0x4714, 0x9D00, 0x9C00, 0x3F00, 0xFA00, 0x0014,
    /* J      K      L      M      N      O      P      Q      R  */
    0x5300, 0x9841, 0x1900, 0x5A48, 0x5A09, 0x5F00, 0xFC00, 0x5F01, 0xFC01,
    /* S      T      U      V      W      X      Y      Z  */
    0xAF00, 0x0414, 0x5b00, 0x18C0, 0x5A81, 0x00C9, 0x0058, 0x05C0
};

/* Constant table for number '0' --> '9' */
const uint16_t NumberMap[10] =
{
    /* 0      1      2      3      4      5      6      7      8      9  */
    0x5F00, 0x4200, 0xF500, 0x6700, 0xEa00, 0xAF00, 0xBF00, 0x04600, 0xFF00, 0xEF00
};

uint32_t Digit[4];     /* Digit frame buffer */

void LCD_Init(void)
{

    uint8_t counter = 0;

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LCD);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);

    /**LCD GPIO Configuration
    PC3     ------> LCD_VLCD
    PA6     ------> LCD_SEG3
    PA7     ------> LCD_SEG4
    PC4     ------> LCD_SEG22
    PC5     ------> LCD_SEG23
    PB0     ------> LCD_SEG5
    PB1     ------> LCD_SEG6
    PB12     ------> LCD_SEG12
    PB13     ------> LCD_SEG13
    PB14     ------> LCD_SEG14
    PB15     ------> LCD_SEG15
    PD8     ------> LCD_SEG28
    PD9     ------> LCD_SEG29
    PD10     ------> LCD_SEG30
    PD11     ------> LCD_SEG31
    PD12     ------> LCD_SEG32
    PD13     ------> LCD_SEG33
    PD14     ------> LCD_SEG34
    PD15     ------> LCD_SEG35
    PC6     ------> LCD_SEG24
    PC7     ------> LCD_SEG25
    PC8     ------> LCD_SEG26
    PA8     ------> LCD_COM0
    PA9     ------> LCD_COM1
    PA10     ------> LCD_COM2
    PA15 (JTDI)     ------> LCD_SEG17
    PB4 (NJTRST)     ------> LCD_SEG8
    PB5     ------> LCD_SEG9
    PB9     ------> LCD_COM3
    */

    GPIO_InitStruct.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6
                          | LL_GPIO_PIN_7 | LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_11;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9
                          | LL_GPIO_PIN_10 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_11;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13
                          | LL_GPIO_PIN_14 | LL_GPIO_PIN_15 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5
                          | LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_11;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11
                          | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_11;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    CLEAR_BIT(LCD->CR, LCD_CR_LCDEN);
    for(counter = 0; counter <= 15; counter++)
    {
        LCD->RAM[counter] = 0;
    }
    /* Enable the display request */
    LCD->SR |= LCD_SR_UDR;
    LCD->FCR = 0x003C5440;
    while(!(LCD->SR & LCD_SR_FCRSR)) {}
    LCD->CR = 0x0000004C;
    /* Enable the peripheral */
    SET_BIT(LCD->CR, LCD_CR_LCDEN);

    /* Wait Until the LCD is enabled */
    while(!(LCD->SR & LCD_SR_ENS)) {}
    while(!(LCD->SR & LCD_SR_RDY)) {}
}

/**
  * @brief Display one or several bar in LCD frame buffer.
  * @param BarId: specifies the LCD GLASS Bar to display
  *     This parameter can be one of the following values:
  *     @arg BAR0: LCD GLASS Bar 0
  *     @arg BAR0: LCD GLASS Bar 1
  *     @arg BAR0: LCD GLASS Bar 2
  *     @arg BAR0: LCD GLASS Bar 3
  * @retval None
  */
void LCD_GLASS_DisplayBar(uint32_t BarId)
{
    uint32_t position = 0;

    /* Check which bar is selected */
    while ((BarId) >> position)
    {
        /* Check if current bar is selected */
        switch(BarId & (1 << position))
        {
            /* Bar 0 */
        case LCD_BAR_0:
            /* Set BAR0 */
            MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], LCD_BAR0_SEG, LCD_BAR0_SEG);
            break;

            /* Bar 1 */
        case LCD_BAR_1:
            /* Set BAR1 */
            MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], LCD_BAR1_SEG, LCD_BAR1_SEG);
            break;

            /* Bar 2 */
        case LCD_BAR_2:
            /* Set BAR2 */
            MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], LCD_BAR2_SEG, LCD_BAR2_SEG);
            break;

            /* Bar 3 */
        case LCD_BAR_3:
            /* Set BAR3 */
            MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], LCD_BAR3_SEG, LCD_BAR3_SEG);
            break;

        default:
            break;
        }
        position++;
    }

    /* Update the LCD display */
    LCD->SR |= LCD_SR_UDR;
}

void LCD_GLASS_ClearBar(uint32_t BarId)
{
    uint32_t position = 0;

    /* Check which bar is selected */
    while ((BarId) >> position)
    {
        /* Check if current bar is selected */
        switch(BarId & (1 << position))
        {
            /* Bar 0 */
        case LCD_BAR_0:
            /* Set BAR0 */
            MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], LCD_BAR0_SEG, 0);
            break;

            /* Bar 1 */
        case LCD_BAR_1:
            /* Set BAR1 */
            MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], LCD_BAR1_SEG, 0);
            break;

            /* Bar 2 */
        case LCD_BAR_2:
            /* Set BAR2 */
            MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], LCD_BAR2_SEG, 0);
            break;

            /* Bar 3 */
        case LCD_BAR_3:
            /* Set BAR3 */
            MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], LCD_BAR3_SEG, 0);
            break;

        default:
            break;
        }
        position++;
    }

    /* Update the LCD display */
    LCD->SR |= LCD_SR_UDR;
}


/**
  * @brief Configure the bar level on LCD by writing bar value in LCD frame buffer.
  * @param BarLevel: specifies the LCD GLASS Battery Level.
  *     This parameter can be one of the following values:
  *     @arg BATTERYLEVEL_OFF: LCD GLASS Battery Empty
  *     @arg BATTERYLEVEL_1_4: LCD GLASS Battery 1/4 Full
  *     @arg BATTERYLEVEL_1_2: LCD GLASS Battery 1/2 Full
  *     @arg BATTERYLEVEL_3_4: LCD GLASS Battery 3/4 Full
  *     @arg BATTERYLEVEL_FULL: LCD GLASS Battery Full
  * @retval None
  */
void LCD_GLASS_BarLevelConfig(uint8_t BarLevel)
{
    switch (BarLevel)
    {
        /* BATTERYLEVEL_OFF */
    case BATTERYLEVEL_OFF:
        /* Set BAR0 & BAR2 off */
        MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], (LCD_BAR0_SEG | LCD_BAR2_SEG), 0);
        /* Set BAR1 & BAR3 off */
        MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], (LCD_BAR1_SEG | LCD_BAR3_SEG), 0);
        break;

        /* BARLEVEL 1/4 */
    case BATTERYLEVEL_1_4:
        /* Set BAR0 on & BAR2 off */
        MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], (LCD_BAR0_SEG | LCD_BAR2_SEG), LCD_BAR0_SEG);
        /* Set BAR1 & BAR3 off */
        MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], (LCD_BAR1_SEG | LCD_BAR3_SEG), 0);
        break;

        /* BARLEVEL 1/2 */
    case BATTERYLEVEL_1_2:
        /* Set BAR0 on & BAR2 off */
        MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], (LCD_BAR0_SEG | LCD_BAR2_SEG), LCD_BAR0_SEG);
        /* Set BAR1 on & BAR3 off */
        MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], (LCD_BAR1_SEG | LCD_BAR3_SEG), LCD_BAR1_SEG);
        break;

        /* Battery Level 3/4 */
    case BATTERYLEVEL_3_4:
        /* Set BAR0 & BAR2 on */
        MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], (LCD_BAR0_SEG | LCD_BAR2_SEG), (LCD_BAR0_SEG | LCD_BAR2_SEG));
        /* Set BAR1 on & BAR3 off */
        MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], (LCD_BAR1_SEG | LCD_BAR3_SEG), LCD_BAR1_SEG);
        break;

        /* BATTERYLEVEL_FULL */
    case BATTERYLEVEL_FULL:
        /* Set BAR0 & BAR2 on */
        MODIFY_REG(LCD->RAM[LCD_BAR0_2_COM], (LCD_BAR0_SEG | LCD_BAR2_SEG), (LCD_BAR0_SEG | LCD_BAR2_SEG));
        /* Set BAR1 on & BAR3 on */
        MODIFY_REG(LCD->RAM[LCD_BAR1_3_COM], (LCD_BAR1_SEG | LCD_BAR3_SEG), (LCD_BAR1_SEG | LCD_BAR3_SEG));
        break;

    default:
        break;
    }


    /* Update the LCD display */
    LCD->SR |= LCD_SR_UDR;
}



/**
  * @brief  Convert an ascii char to the a LCD digit.
  * @param  Char: a char to display.
  * @param  Point: a point to add in front of char
  *         This parameter can be: POINT_OFF or POINT_ON
  * @param  Colon : flag indicating if a colon character has to be added in front
  *         of displayed character.
  *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
  * @retval None
  */
void LCD_Convert(uint8_t *Char, Point_Typedef Point, DoublePoint_Typedef Colon)
{
    uint16_t ch = 0 ;
    uint8_t loop = 0, index = 0;

    switch (*Char)
    {
    case ' ' :
        ch = 0x00;
        break;

    case '*':
        ch = C_STAR;
        break;

    case '(' :
        ch = C_OPENPARMAP;
        break;

    case ')' :
        ch = C_CLOSEPARMAP;
        break;

    case 'd' :
        ch = C_DMAP;
        break;

    case 'm' :
        ch = C_MMAP;
        break;

    case 'n' :
        ch = C_NMAP;
        break;

    case 'u' :
        ch = C_UMAP;
        break;

    case '-' :
        ch = C_MINUS;
        break;
    case '+' :
        ch = C_PLUS;
        break;

    case '/' :
        ch = C_SLATCH;
        break;

    case '%' :
        ch = C_PERCENT_2;
        break;
    case 255 :
        ch = C_FULL;
        break ;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        ch = NumberMap[*Char - ASCII_CHAR_0];
        break;

    default:
        /* The character Char is one letter in upper case*/
        if ( (*Char < ASCII_CHAR_LEFT_OPEN_BRACKET) && (*Char > ASCII_CHAR_AT_SYMBOL) )
        {
            ch = CapLetterMap[*Char - 'A'];
        }
        /* The character Char is one letter in lower case*/
        if ( (*Char < ASCII_CHAR_LEFT_OPEN_BRACE) && ( *Char > ASCII_CHAR_APOSTROPHE) )
        {
            ch = CapLetterMap[*Char - 'a'];
        }
        break;
    }

    /* Set the digital point can be displayed if the point is on */
    if (Point == POINT_ON)
    {
        ch |= 0x0002;
    }

    /* Set the "COL" segment in the character that can be displayed if the colon is on */
    if (Colon == DOUBLEPOINT_ON)
    {
        ch |= 0x0020;
    }

    for (loop = 12, index = 0 ; index < 4; loop -= 4, index++)
    {
        Digit[index] = (ch >> loop) & 0x0f; /*To isolate the less significant digit */
    }
}

/**
  * @brief  Write a character in the LCD frame buffer.If display one ch,please manual run LCD->SR |= LCD_SR_UDR;
  * @param  ch: the character to display.
  * @param  Point: a point to add in front of char
  *         This parameter can be: POINT_OFF or POINT_ON
  * @param  Colon: flag indicating if a colon character has to be added in front
  *         of displayed character.
  *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
  * @param  Position: position in the LCD of the character to write [1:6]
  * @retval None
  */
void LCD_WriteChar(uint8_t *ch, Point_Typedef Point, DoublePoint_Typedef Colon, DigitPosition_Typedef Position)
{
    uint32_t data = 0x00;
    /* To convert displayed character in segment in array digit */
    LCD_Convert(ch, (Point_Typedef)Point, (DoublePoint_Typedef)Colon);

    switch (Position)
    {
        /* Position 1 on LCD (Digit1)*/
    case LCD_DIGIT_POSITION_1:
        data = ((Digit[0] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG1_SHIFT)
               | (((Digit[0] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG23_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT1_COM0], ~LCD_DIGIT1_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[1] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG1_SHIFT)
               | (((Digit[1] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG23_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT1_COM1], ~LCD_DIGIT1_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[2] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG1_SHIFT)
               | (((Digit[2] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG23_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT1_COM2], ~LCD_DIGIT1_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[3] & 0x1) << LCD_SEG0_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG1_SHIFT)
               | (((Digit[3] & 0x4) >> 2) << LCD_SEG22_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG23_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT1_COM3], ~LCD_DIGIT1_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

        /* Position 2 on LCD (Digit2)*/
    case LCD_DIGIT_POSITION_2:
        data = ((Digit[0] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG3_SHIFT)
               | (((Digit[0] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG21_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT2_COM0], ~LCD_DIGIT2_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[1] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG3_SHIFT)
               | (((Digit[1] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG21_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT2_COM1], ~LCD_DIGIT2_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[2] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG3_SHIFT)
               | (((Digit[2] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG21_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT2_COM2], ~LCD_DIGIT2_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[3] & 0x1) << LCD_SEG2_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG3_SHIFT)
               | (((Digit[3] & 0x4) >> 2) << LCD_SEG20_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG21_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT2_COM3], ~LCD_DIGIT2_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

        /* Position 3 on LCD (Digit3)*/
    case LCD_DIGIT_POSITION_3:
        data = ((Digit[0] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG5_SHIFT)
               | (((Digit[0] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG19_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT3_COM0], ~LCD_DIGIT3_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[1] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG5_SHIFT)
               | (((Digit[1] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG19_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT3_COM1], ~LCD_DIGIT3_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[2] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG5_SHIFT)
               | (((Digit[2] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG19_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT3_COM2], ~LCD_DIGIT3_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[3] & 0x1) << LCD_SEG4_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG5_SHIFT)
               | (((Digit[3] & 0x4) >> 2) << LCD_SEG18_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG19_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT3_COM3], ~LCD_DIGIT3_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

        /* Position 4 on LCD (Digit4)*/
    case LCD_DIGIT_POSITION_4:
        data = ((Digit[0] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG17_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM0], ~LCD_DIGIT4_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = (((Digit[0] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG16_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM0_1], ~LCD_DIGIT4_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[1] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG17_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM1], ~LCD_DIGIT4_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = (((Digit[1] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG16_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM1_1], ~LCD_DIGIT4_COM1_1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[2] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG17_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM2], ~LCD_DIGIT4_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = (((Digit[2] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG16_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM2_1], ~LCD_DIGIT4_COM2_1_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[3] & 0x1) << LCD_SEG6_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG17_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM3], ~LCD_DIGIT4_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */

        data = (((Digit[3] & 0x2) >> 1) << LCD_SEG7_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG16_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT4_COM3_1], ~LCD_DIGIT4_COM3_1_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

        /* Position 5 on LCD (Digit5)*/
    case LCD_DIGIT_POSITION_5:
        data = (((Digit[0] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[0] & 0x4) >> 2) << LCD_SEG14_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM0], ~LCD_DIGIT5_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[0] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG15_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM0_1], ~LCD_DIGIT5_COM0_1_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = (((Digit[1] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[1] & 0x4) >> 2) << LCD_SEG14_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM1], ~LCD_DIGIT5_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[1] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG15_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM1_1], ~LCD_DIGIT5_COM1_1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = (((Digit[2] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[2] & 0x4) >> 2) << LCD_SEG14_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM2], ~LCD_DIGIT5_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[2] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG15_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM2_1], ~LCD_DIGIT5_COM2_1_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = (((Digit[3] & 0x2) >> 1) << LCD_SEG9_SHIFT) | (((Digit[3] & 0x4) >> 2) << LCD_SEG14_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM3], ~LCD_DIGIT5_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */

        data = ((Digit[3] & 0x1) << LCD_SEG8_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG15_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT5_COM3_1], ~LCD_DIGIT5_COM3_1_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

        /* Position 6 on LCD (Digit6)*/
    case LCD_DIGIT_POSITION_6:
        data = ((Digit[0] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[0] & 0x2) >> 1) << LCD_SEG11_SHIFT)
               | (((Digit[0] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[0] & 0x8) >> 3) << LCD_SEG13_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT6_COM0], ~LCD_DIGIT6_COM0_SEG_MASK, data); /* 1G 1B 1M 1E */

        data = ((Digit[1] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[1] & 0x2) >> 1) << LCD_SEG11_SHIFT)
               | (((Digit[1] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[1] & 0x8) >> 3) << LCD_SEG13_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT6_COM1], ~LCD_DIGIT6_COM1_SEG_MASK, data) ; /* 1F 1A 1C 1D  */

        data = ((Digit[2] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[2] & 0x2) >> 1) << LCD_SEG11_SHIFT)
               | (((Digit[2] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[2] & 0x8) >> 3) << LCD_SEG13_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT6_COM2], ~LCD_DIGIT6_COM2_SEG_MASK, data) ; /* 1Q 1K 1Col 1P  */

        data = ((Digit[3] & 0x1) << LCD_SEG10_SHIFT) | (((Digit[3] & 0x2) >> 1) << LCD_SEG11_SHIFT)
               | (((Digit[3] & 0x4) >> 2) << LCD_SEG12_SHIFT) | (((Digit[3] & 0x8) >> 3) << LCD_SEG13_SHIFT);
        MODIFY_REG(LCD->RAM[LCD_DIGIT6_COM3], ~LCD_DIGIT6_COM3_SEG_MASK, data) ; /* 1H 1J 1DP 1N  */
        break;

    default:
        break;
    }


}

void LCD_GLASS_DisplayString(uint8_t *ptr)
{
    DigitPosition_Typedef position = LCD_DIGIT_POSITION_1;

    /* Send the string character by character on lCD */
    while ((*ptr != 0) & (position <= LCD_DIGIT_POSITION_6))
    {
        /* Write one character on LCD */
        LCD_WriteChar(ptr, POINT_OFF, DOUBLEPOINT_OFF, position);

        /* Point on the next character */
        ptr++;

        /* Increment the character counter */
        position++;
    }

    /* Update the LCD display */
    LCD->SR |= LCD_SR_UDR;
}

/**
  * @brief  Write a character string with decimal point in the LCD RAM buffer.
  * @param  ptr: Pointer to string to display on the LCD Glass.
  * @retval None
  * @note Required preconditions: Char is ASCCI value "ORed" with decimal point or Colon flag
  */
void LCD_GLASS_DisplayStrDeci(uint16_t *ptr)
{
    DigitPosition_Typedef index = LCD_DIGIT_POSITION_1;
    uint8_t tmpchar = 0;

    /* Send the string character by character on lCD */
    while((*ptr != 0) & (index <= LCD_DIGIT_POSITION_6))
    {
        tmpchar = (*ptr) & 0x00FF;

        switch((*ptr) & 0xF000)
        {
        case DOT:
            /* Write one character on LCD with decimal point */
            LCD_WriteChar(&tmpchar, POINT_ON, DOUBLEPOINT_OFF, index);
            break;
        case DOUBLE_DOT:
            /* Write one character on LCD with decimal point */
            LCD_WriteChar(&tmpchar, POINT_OFF, DOUBLEPOINT_ON, index);
            break;
        default:
            LCD_WriteChar(&tmpchar, POINT_OFF, DOUBLEPOINT_OFF, index);
            break;
        }/* Point on the next character */
        ptr++;

        /* Increment the character counter */
        index++;
    }
    /* Update the LCD display */
    LCD->SR |= LCD_SR_UDR;
}

void LCD_GLASS_Clear(void)
{

    uint8_t counter = 0;
    for(counter = 0; counter <= 15; counter++)
    {
        LCD->RAM[counter] = 0;
    }
}

