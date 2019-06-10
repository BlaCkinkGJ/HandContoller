/**
 * @file debug_monitor.c
 * @author BlaCkinkGJ (ss5kijun@gmil.com)
 * @brief debug monitor implementation files
 * @version 0.1
 * @date 2019-06-06
 * 
 */
#include <debug_monitor.h>

static const char* headers[] = {
    "# Flex Ratio",
    "# Gyro Value",
    "# 3-axis accelerometers",
};
static const int numOfHeader = sizeof(headers) / sizeof(const char*);
static char flexString[256];
static char gyroString[256];
static char accelString[256];


/**
 * @brief To use something specific format.
 * this is the private function.
 * it can't be accessed by other location.
 * 
 * @param data
 * @return const char* 
 */
static const char* formatter(const char* format, const char* data)
{
    static char strBuf[MAX_LCD_CHAR_POSX];
    sprintf(strBuf, format, data);
    return strBuf;
}

/**
 * @brief draw the title
 * 
 * @param title 
 */
void drawTitle(const char* title)
{
    // LCD Screen: x, y,
    LCD_ShowString(1, 1,
        (CPU_INT08U*)formatter("[[ %s ]]", title),
        BLACK, // foreground color
        WHITE); // background color
}

/**
 * @brief draw the header
 * 
 */
void drawHeader()
{
    int i = 0;
    for (i = 0; i < numOfHeader; i++) {
        LCD_ShowString(1, FONT_SIZE * (i * 3 + 2),
            (CPU_INT08U*)formatter("%s", headers[i]),
            BLACK,
            WHITE);
    }
}

/**
 * @brief content formatting
 * 
 * @param id 
 * @param ptr 
 * @return const char* 
 */
static const char* contentFormatter(int id, const void* ptr)
{
    static char strContentBuf[MAX_LCD_CHAR_POSX];
    switch (id) {
    case 0:
    case 1:
    case 2:
        sprintf(strContentBuf, "%s", (char*)(ptr));
        break;
    default:
        sprintf(strContentBuf, "NOT IMPLEMENTED");
    }
    return strContentBuf;
}

/**
 * @brief draw the contents
 * 
 * @param contents 
 */
void drawContents(const struct DebugContents* contents)
{
    int i = 0;
    //@TODO: you have to change this section for debug contents
    void* const* structStartPtr = &(contents->flexRatio);
    for (i = 0; i < numOfHeader; i++) {
        LCD_ShowString(1, FONT_SIZE * (i * 3 + 3),
            (CPU_INT08U*)contentFormatter(i, *structStartPtr),
            BLACK,
            WHITE);
        structStartPtr++;
    }
}
/**
 * @brief Set the Contents object.
 * But sadly I think this will be changed
 * because this dependent on the value(int *)
 * in other words, THIS IS NOT GENERIC FUNCTION!
 * 
 * @param contents, flex, gyro
 * @param pack 
 */
void setContents(struct DebugContents* contents, CPU_INT16S *flex, CPU_INT16S *gyro)
{
    sprintf(flexString, "%-4d%-4d%-4d", flex[0], flex[1], flex[2]);
    sprintf(gyroString, "%-6d%-6d%-6d", gyro[0], gyro[1], gyro[2]);
    sprintf(accelString, "%-6d%-6d%-6d", gyro[3], gyro[4], gyro[5]);
    contents->flexRatio = flexString;
    contents->gyroValue = gyroString;
    contents->axisAccel3 = accelString;
}