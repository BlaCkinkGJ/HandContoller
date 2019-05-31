#ifndef DEBUG_MONITOR_H
#define DEBUG_MONITOR_H

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
void setContents(struct DebugContents *contents, int *pack);

#endif