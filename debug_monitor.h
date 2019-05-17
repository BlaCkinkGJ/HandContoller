#ifndef __DEBUG_MONITOR_H__
#define __DEBUG_MONITOR_H__

#include <stdlib.h>
#include <stdio.h>
#include <lcd.h>

struct DebugContents {
    void *flexRatio;
    void *bluetoothStatus;
    void *gyroValue;
    void *axisAccel2;
    void *axisAccel3;
};

void drawTitle(const char*);
void drawHeader();
void drawContents(const struct DebugContents*);

#endif