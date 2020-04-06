#ifndef SENSOR_TYPE_H
#define SENSOR_TYPE_H

#include "stdio.h"

enum Sensor_type{
    unknown = 0,
    light = 1,
    magnetometer = 2
};

static const int SENSOR_TYPE_NUM = 3;

static const char* sensor_types[SENSOR_TYPE_NUM] = {
    "unknow",
    "light",
    "magnetometer"
};



#endif