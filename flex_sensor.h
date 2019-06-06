#ifndef FLEX_SENSOR_H
#define FLEX_SENSOR_H

/**
 * @file flex_sensor.h
 * @author BlaCkinkGJ (ss5kijun@gmail.com)
 * @brief flex sensor header file
 * @version 0.1
 * @date 2019-06-06
 * 
 */
#include <stm32f10x_lib.h>

void flexInit();
int getFlexValue();

#endif