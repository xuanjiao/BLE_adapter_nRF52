#ifndef SENSOR_TYPE_H
#define SENSOR_TYPE_H

#include "stdio.h"

enum Sensor_type{
    unknown = 0,
    light = 1,
    magnetometer = 2
};

static const char* sensor_types[] = {
    "unknow",
    "light",
    "magnetometer"
};

#endif