#include <debug_monitor.h>

static const char *headers[5] = {
    "# Flex Ratio",
    "# Bluetooth Status",
    "# Gyro Value",
    "# 3-axis accelerometers",
    "# 1-axis accelerometers"
};
static const int numOfHeader = sizeof(headers)/sizeof(const char *);

/**
 * @brief To use something specific format.
 * this is the private function.
 * it can't be accessed by other location.
 * 
 * @param data
 * @return const char* 
 */
static const char* formatter(const char* format, const char* data) {
    static char strBuf[MAX_LCD_CHAR_POSX];
    sprintf(strBuf, format, data);
    return strBuf;
}

/**
 * @brief draw the title
 * 
 * @param title 
 */
void drawTitle(const char* title) {
    // LCD Screen: x, y,
    LCD_ShowString(1, 1,
        (CPU_INT08U*)formatter("[[ %s ]]",title),
        BLACK,  // foreground color
        WHITE); // background color
}

/**
 * @brief draw the header
 * 
 */
void drawHeader() {
    int i = 0;
    for(i = 0; i < numOfHeader; i++) {
        LCD_ShowString(1, FONT_SIZE*(i*3+2),
            (CPU_INT08U*)formatter("%s", headers[i]),
            BLACK,
            WHITE);
    }
}

static const char* contentFormatter(int id, void * const * ptr) {
    static char strContentBuf[MAX_LCD_CHAR_POSX];
    switch(id) {
        case 1:
        sprintf(strContentBuf, "==> %d",*(int *)(*ptr));
        break;
        case 2:
        sprintf(strContentBuf, "==)) %d",*(int *)(*ptr));
        break;
        default:
        sprintf(strContentBuf, "error ==> %d %d",id, *(int *)(*ptr));
    }
    return strContentBuf;
}

/**
 * @brief draw the contents
 * 
 * @param contents 
 */
void drawContents(const struct DebugContents *contents) {
    int i = 0;
    //@TODO: you have to change this section for debug contents
    void * const* ptr = &(contents->flexRatio);
    for(i = 0; i < numOfHeader; i++) {
        LCD_ShowString(1, FONT_SIZE*(i*3+3),
            (CPU_INT08U*)contentFormatter(i, ptr),
            BLACK,
            WHITE);
        ptr++;
    }
}