#include <lcd.h>
#include <font.h>

/* Private variable ---------------------------------------------------------*/
CPU_INT16U DeviceCode;

/* Private typedef -----------------------------------------------------------*/

/* private function---------------------------------------------------------- */

void LCD_RD(int n)
{
    // LCD_Read
    //@TODO
    switch (n) {
    case 0:
        GPIO_ResetBits(GPIOD, GPIO_Pin_15);
        break;
    case 1:
        GPIO_SetBits(GPIOD, GPIO_Pin_15);
        break;
    }
}

void LCD_WR(int n)
{
    // LCD_Write
    //@TODO
    switch (n) {
    case 0:
        GPIO_ResetBits(GPIOD, GPIO_Pin_14);
        break;
    case 1:
        GPIO_SetBits(GPIOD, GPIO_Pin_14);
        break;
    }
}

void LCD_CS(int n)
{
    // LCD_ChipSelect
    //@TODO
    switch (n) {
    case 0:
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
        break;
    case 1:
        GPIO_SetBits(GPIOC, GPIO_Pin_6);
        break;
    }
}

void LCD_RS(int n)
{
    // LCD_RegisterSelect
    //@TODO
    switch (n) {
    case 0:
        GPIO_ResetBits(GPIOD, GPIO_Pin_13);
        break;
    case 1:
        GPIO_SetBits(GPIOD, GPIO_Pin_13);
        break;
    }
}

void LCD_WR_REG(CPU_INT16U LCD_Reg)
{
    //@TODO
    LCD_RD(1);
    LCD_RS(0);
    LCD_CS(0);
    LCD_WR(0);
    DataToWrite(LCD_Reg);
    LCD_WR(1);
    LCD_CS(1);
    LCD_RS(1);
}

void LCD_WR_DATA(CPU_INT16U LCD_Data)
{
    //@TODO
    LCD_RD(1);
    LCD_RS(1);
    LCD_CS(0);
    LCD_WR(0);
    DataToWrite(LCD_Data);
    LCD_WR(1);
    LCD_CS(1);
}

void LCD_WriteReg(CPU_INT16U LCD_Reg, CPU_INT16U LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(0x22);
}

void LCD_WriteRAM(CPU_INT16U RGB_Code)
{
    LCD_WR_DATA(RGB_Code);
}

/**
	LCD_SetCursor(CPU_INT16U Xpos, CPU_INT16U Ypos)

 **/
void LCD_SetCursor(CPU_INT16U Xpos, CPU_INT16U Ypos)
{
    LCD_WriteReg(0x004E, Xpos);
    LCD_WriteReg(0X004F, Ypos);
}

/**
	LCD_Clear(CPU_INT16U Colour)
 **/
void LCD_Clear(CPU_INT16U Color)
{
    CPU_INT32U index = 0;
    LCD_SetCursor(0x00, 0x0000);
    LCD_WriteRAM_Prepare();
    for (index = 0; index < 76800; index++) {
        LCD_WR_DATA(Color);
    }
}

/****************************************************************************
 * LCD_GetPoint(CPU_INT16U x,CPU_INT16U y)

 * CurrentColor = LCD_GetPoint(10,10);

 ****************************************************************************/
//CPU_INT16U LCD_GetPoint(CPU_INT16U x,CPU_INT16U y)
//{
//  LCD_SetCursor(x,y);
//  if(DeviceCode=0X8999)
//    return (LCD_ReadRAM());
//  else
//    return (LCD_BGRtoRGB(LCD_ReadRAM()));
//}

/**
 * LCD_DrawPoint(void)


 **/
void LCD_DrawPoint(CPU_INT16U xsta, CPU_INT16U ysta)
{
    LCD_SetCursor(xsta, ysta);
    LCD_WR_REG(0x22);
    LCD_WR_DATA(POINT_COLOR);
}

/**					
	LCD_WindowMax()

 **/
void LCD_WindowMax(unsigned int x, unsigned int y, unsigned int x_end, unsigned int y_end)
{
    LCD_WriteReg(0x44, x | ((x_end - 1) << 8));
    LCD_WriteReg(0x45, y);
    LCD_WriteReg(0x46, y_end - 1);
}

/**
 * LCD_Fill(CPU_INT08U xsta, CPU_INT16U ysta, CPU_INT08U xend, CPU_INT16U yend, CPU_INT16U colour)

 **/
void LCD_Fill(CPU_INT08U xsta, CPU_INT16U ysta, CPU_INT08U xend, CPU_INT16U yend, CPU_INT16U colour)
{
    CPU_INT32U n;

    LCD_WindowMax(xsta, ysta, xend, yend);
    LCD_SetCursor(xsta, ysta);
    LCD_WriteRAM_Prepare();
    n = (CPU_INT32U)(yend - ysta + 1) * (xend - xsta + 1);
    while (n--) {
        LCD_WR_DATA(colour);
    }

    LCD_WindowMax(0, 0, 240, 320);
}

/**
 * LCD_DrawLine(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend)

 **/
void LCD_DrawLine(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend)
{
    CPU_INT16U x, y, t;
    if ((xsta == xend) && (ysta == yend))
        LCD_DrawPoint(xsta, ysta);
    else if (abs(yend - ysta) > abs(xend - xsta)) {
        if (ysta > yend) {
            t = ysta;
            ysta = yend;
            yend = t;
            t = xsta;
            xsta = xend;
            xend = t;
        }
        for (y = ysta; y < yend; y++) {
            x = (CPU_INT32U)(y - ysta) * (xend - xsta) / (yend - ysta) + xsta;
            LCD_DrawPoint(x, y);
        }
    } else {
        if (xsta > xend) {
            t = ysta;
            ysta = yend;
            yend = t;
            t = xsta;
            xsta = xend;
            xend = t;
        }
        for (x = xsta; x <= xend; x++) {
            y = (CPU_INT32U)(x - xsta) * (yend - ysta) / (xend - xsta) + ysta;
            LCD_DrawPoint(x, y);
        }
    }
}

/**
 * Draw_Circle(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U r)

 **/
void Draw_Circle(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);
    while (a <= b) {
        LCD_DrawPoint(x0 - b, y0 - a); //3
        LCD_DrawPoint(x0 + b, y0 - a); //0
        LCD_DrawPoint(x0 - a, y0 + b); //1
        LCD_DrawPoint(x0 - b, y0 - a); //7
        LCD_DrawPoint(x0 - a, y0 - b); //2
        LCD_DrawPoint(x0 + b, y0 + a); //4
        LCD_DrawPoint(x0 + a, y0 - b); //5
        LCD_DrawPoint(x0 + a, y0 + b); //6
        LCD_DrawPoint(x0 - b, y0 + a);
        a++;

        if (di < 0)
            di += 4 * a + 6;
        else {
            di += 10 + 4 * (a - b);
            b--;
        }
        LCD_DrawPoint(x0 + a, y0 + b);
    }
}

/**
 * LCD_DrawRectangle(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend)


 **/
void LCD_DrawRectangle(CPU_INT16U xsta, CPU_INT16U ysta, CPU_INT16U xend, CPU_INT16U yend)
{
    LCD_DrawLine(xsta, ysta, xend, ysta);
    LCD_DrawLine(xsta, ysta, xsta, yend);
    LCD_DrawLine(xsta, yend, xend, yend);
    LCD_DrawLine(xend, ysta, xend, yend);
}

/**
 * LCD_ShowChar(CPU_INT08U x, CPU_INT16U y, CPU_INT08U num, CPU_INT08U size, CPU_INT16U PenColor, CPU_INT16U BackColor)

 **/
void LCD_ShowChar(CPU_INT08U x, CPU_INT16U y, CPU_INT08U num, CPU_INT08U size, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
#define MAX_CHAR_POSX 232
#define MAX_CHAR_POSY 304
    CPU_INT08U temp;
    CPU_INT08U pos, t;
    if (x > MAX_CHAR_POSX || y > MAX_CHAR_POSY)
        return;
    LCD_WindowMax(x, y, x + size / 2, y + size);
    LCD_SetCursor(x, y);

    LCD_WriteRAM_Prepare();
    num = num - ' ';
    for (pos = 0; pos < size; pos++) {
        if (size == 12)
            temp = asc2_1206[num][pos];
        else
            temp = asc2_1608[num][pos];
        for (t = 0; t < size / 2; t++) {
            if (temp & 0x01) {
                LCD_WR_DATA(PenColor);
            } else
                LCD_WR_DATA(BackColor);
            temp >>= 1;
        }
    }
    LCD_WindowMax(0x0000, 0x0000, 240, 320);
}

CPU_INT32U mypow(CPU_INT08U m, CPU_INT08U n)
{
    CPU_INT32U result = 1;
    while (n--)
        result *= m;
    return result;
}

void LCD_ShowNum(CPU_INT08U x, CPU_INT08U y, CPU_INT32U num, CPU_INT08U len, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
    CPU_INT08U size = 16;
    CPU_INT08U t, temp;
    CPU_INT08U enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, PenColor, BackColor);
                continue;
            } else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, PenColor, BackColor);
    }
}

/**
	 LCD_ShowCharString(CPU_INT16U x, CPU_INT16U y, const CPU_INT08U *p, CPU_INT16U PenColor, CPU_INT16U BackColor)

 **/
void LCD_ShowCharString(CPU_INT16U x, CPU_INT16U y, const CPU_INT08U* p, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
    CPU_INT08U size = 16;

    if (x > MAX_CHAR_POSX) {
        x = 0;
        y += size;
    }
    if (y > MAX_CHAR_POSY) {
        y = x = 0;
        LCD_Clear(WHITE);
    }
    LCD_ShowChar(x, y, *p, size, PenColor, BackColor);
}

/**
 * findHzIndex(CPU_INT08U *hz)

 **/
CPU_INT16U findHzIndex(CPU_INT08U* hz)
{
    CPU_INT16U i = 0;
    FNT_GB16* ptGb16 = (FNT_GB16*)GBHZ_16;
    while (ptGb16[i].Index[0] > 0x80) {
        if ((*hz == ptGb16[i].Index[0]) && (*(hz + 1) == ptGb16[i].Index[1])) {
            return i;
        }
        i++;
        if (i > (sizeof((FNT_GB16*)GBHZ_16) / sizeof(FNT_GB16) - 1)) {
            break;
        }
    }
    return 0;
}

/**
 * WriteOneHz(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U *pucMsk, CPU_INT16U PenColor, CPU_INT16U BackColor)

 **/
void WriteOneHz(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U* pucMsk, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
    CPU_INT16U i, j;
    CPU_INT16U mod[16];
    CPU_INT16U* pusMsk;
    CPU_INT16U y;

    CPU_INT16U size = 16;

    pusMsk = (CPU_INT16U*)pucMsk;

    for (i = 0; i < 16; i++) {
        mod[i] = *pusMsk;
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);
        pusMsk = pusMsk + 1;
    }
    y = y0;
    LCD_WindowMax(x0, y0, x0 + size, y0 + size);
    LCD_SetCursor(x0, y0);
    LCD_WriteRAM_Prepare();
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            if ((mod[i] << j) & 0x8000) {
                LCD_WriteRAM(PenColor);
            } else {
                LCD_WriteRAM(BackColor);
            }
        }
        y++;
    }
    LCD_WindowMax(0x0000, 0x0000, 240, 320);
}

/**
 * LCD_ShowHzString(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U *pcStr, CPU_INT16U PenColor, CPU_INT16U BackColor)

 **/
void LCD_ShowHzString(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U* pcStr, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
#define MAX_HZ_POSX 224
#define MAX_HZ_POSY 304
    CPU_INT16U usIndex;
    CPU_INT08U size = 16;
    FNT_GB16* ptGb16 = 0;
    ptGb16 = (FNT_GB16*)GBHZ_16;

    if (x0 > MAX_HZ_POSX) {
        x0 = 0;
        y0 += size;
    }
    if (y0 > MAX_HZ_POSY) {
        y0 = x0 = 0;
        LCD_Clear(WHITE);
    }

    usIndex = findHzIndex(pcStr);
    WriteOneHz(x0, y0, (CPU_INT08U*)&(ptGb16[usIndex].Msk[0]), PenColor, BackColor);
}

/**
 * LCD_ShowString(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U *pcstr, CPU_INT16U PenColor, CPU_INT16U BackColor)

 **/
void LCD_ShowString(CPU_INT16U x0, CPU_INT16U y0, CPU_INT08U* pcStr, CPU_INT16U PenColor, CPU_INT16U BackColor)
{
    while (*pcStr != '\0') {
        if (*pcStr > 0x80) {
            LCD_ShowHzString(x0, y0, pcStr, PenColor, BackColor);
            pcStr += 2;
            x0 += 16;
        } else {
            LCD_ShowCharString(x0, y0, pcStr, PenColor, BackColor);
            pcStr += 1;
            x0 += 8;
        }
    }
}

CPU_INT16U LCD_RGBtoBGR(CPU_INT16U Color)
{
    CPU_INT16U r, g, b, bgr;

    b = (Color >> 0) & 0x1f;
    g = (Color >> 5) & 0x3f;
    r = (Color >> 11) & 0x1f;

    bgr = (b << 11) + (g << 5) + (r << 0);

    return (bgr);
}

void LCD_DrawPicture(CPU_INT16U StartX, CPU_INT16U StartY, CPU_INT16U Xend, CPU_INT16U Yend, CPU_INT08U* pic)
{
    static CPU_INT16U i = 0, j = 0;
    CPU_INT16U* bitmap = (CPU_INT16U*)pic;

    LCD_WindowMax(StartX, StartY, Xend, Yend);
    LCD_SetCursor(StartX, StartY);
    LCD_WriteRAM_Prepare();
    for (j = 0; j < Yend - StartY; j++) {
        for (i = 0; i < Xend - StartX; i++)
            LCD_WriteRAM(*bitmap++);
    }

    LCD_WindowMax(0, 0, 240, 320);
}

void LCD_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
            | RCC_APB2Periph_GPIOE,
        ENABLE);

    /*DB00~DB16*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
        | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
        | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11
        | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* LCD_RS  LCD_WR  LCD_RD*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* LCD_CS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
 * LCD_Init()

 **/

void Delay_10ms(int time)
{
    int i, j;
    for (i = 0; i < time; i++) {
        for (j = 0; j < 1140; j++) {
        }
    }
}

void LCD_Init(void)
{
    LCD_Configuration();

    /* Delay_10ms() �Լ� ����*/
    Delay_10ms(10); /* delay 100 ms */
    Delay_10ms(10); /* delay 100 ms */
    //DeviceCode = LCD_ReadReg(0x0000);
    Delay_10ms(10); /* delay 100 ms */

    {
        /*-----   Start Initial Sequence ------*/
        LCD_WriteReg(0x00, 0x0001); /*Start internal OSC */
        LCD_WriteReg(0x01, 0x3B3F); /*Driver output control */
        LCD_WriteReg(0x02, 0x0600); /* set 1 line inversion	*/
        /*-------- Power control setup --------*/
        LCD_WriteReg(0x0C, 0x0007); /* Adjust VCIX2 output voltage */
        LCD_WriteReg(0x0D, 0x0006); /* Set amplitude magnification of VLCD63 */
        LCD_WriteReg(0x0E, 0x3200); /* Set alternating amplitude of VCOM */
        LCD_WriteReg(0x1E, 0x00BB); /* Set VcomH voltage */
        LCD_WriteReg(0x03, 0x6A64); /* Step-up factor/cycle setting  */
        /*-------- RAM position control --------*/
        LCD_WriteReg(0x0F, 0x0000); /* Gate scan position start at G0 */
        LCD_WriteReg(0x44, 0xEF00); /* Horizontal RAM address position */
        LCD_WriteReg(0x45, 0x0000); /* Vertical RAM address start position*/
        LCD_WriteReg(0x46, 0x013F); /* Vertical RAM address end position */
        /* ------ Adjust the Gamma Curve -------*/
        LCD_WriteReg(0x30, 0x0000);
        LCD_WriteReg(0x31, 0x0706);
        LCD_WriteReg(0x32, 0x0206);
        LCD_WriteReg(0x33, 0x0300);
        LCD_WriteReg(0x34, 0x0002);
        LCD_WriteReg(0x35, 0x0000);
        LCD_WriteReg(0x36, 0x0707);
        LCD_WriteReg(0x37, 0x0200);
        LCD_WriteReg(0x3A, 0x0908);
        LCD_WriteReg(0x3B, 0x0F0D);
        /*--------- Special command -----------*/
        LCD_WriteReg(0x28, 0x0006); /* Enable test command	*/
        LCD_WriteReg(0x2F, 0x12EB); /* RAM speed tuning	 */
        LCD_WriteReg(0x26, 0x7000); /* Internal Bandgap strength */
        LCD_WriteReg(0x20, 0xB0E3); /* Internal Vcom strength */
        LCD_WriteReg(0x27, 0x0044); /* Internal Vcomh/VcomL timing */
        LCD_WriteReg(0x2E, 0x7E45); /* VCOM charge sharing time */
        /*--------- Turn On display ------------*/
        LCD_WriteReg(0x10, 0x0000); /* Sleep mode off */
        Delay_10ms(3); /* Wait 30mS  */
        LCD_WriteReg(0x11, 0x6870); /* Entry mode setup. 262K type B, take care on the data bus with 16it only */
        LCD_WriteReg(0x07, 0x0033); /* Display ON	*/
    }
    Delay_10ms(5);
    LCD_Clear(BLACK);
}
