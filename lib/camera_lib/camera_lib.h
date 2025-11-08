#ifndef CAMERA_LIB_H
#define CAMERA_LIB_H

#include "esp_camera.h"
#include "Arduino.h" // Add this line for String object

enum class CameraModel {
    OV2640,
    OV5640
};

class CameraBase {
public:
    CameraBase();
    // Modified to accept a pixel format parameter
    bool init(CameraModel model, pixformat_t format = PIXFORMAT_JPEG, bool vflip = false, bool hmirror = false);
    // New public methods for frame buffer handling
    camera_fb_t* getFrameBuffer();
    void returnFrameBuffer(camera_fb_t* fb);
    void printFrameAsHex();

private:
    camera_config_t config;
    /* data */
};

#endif