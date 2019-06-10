#ifndef DEBUG_MONITOR_H
#define DEBUG_MONITOR_H

/**
 * @file debug_monitor.h
 * @author BlaCkinkGJ (ss5kijun@gmail.com)
 * @brief debug monitor header
 * @version 0.1
 * @date 2019-06-06
 * 
 */
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>

struct DebugContents {
    void* flexRatio;
    void* gyroValue;
    void* axisAccel3;
};

void drawTitle(const char*);
void drawHeader();
void drawContents(const struct DebugContents*);
void setContents(struct DebugContents* contents, CPU_INT16S *flex, CPU_INT16S *gyro);

#endif