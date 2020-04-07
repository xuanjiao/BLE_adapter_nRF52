#ifndef GLOBAL_H
#define GLOBAL_H

      
// log file stores in file system        
static const char* FILE_SYSTEM_NAME = "fs";

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