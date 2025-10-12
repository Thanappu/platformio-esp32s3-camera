#ifndef CAMERA_LIB_H
#define CAMERA_LIB_H

#include "esp_camera.h"
enum class CameraModel {
    OV2640,
    OV5640
};

class CameraBase{
public:
    CameraBase();
    bool init(CameraModel model);
private:
    camera_config_t config;
    /* data */
};
#endif
