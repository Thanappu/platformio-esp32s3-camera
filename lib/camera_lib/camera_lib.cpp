// lib/camera_lib/camera_lib.cpp

#include <Arduino.h>
#include "camera_lib.h"

// =================================================================
// ==      BOARD PINOUT CONFIGURATION - SELECT IN platformio.ini  ==
// =================================================================

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  // --- CORRECTED Pinout for your specific ESP32S3_EYE board ---
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM -1
  #define XCLK_GPIO_NUM 15
  #define SIOD_GPIO_NUM 4
  #define SIOC_GPIO_NUM 5

  #define Y9_GPIO_NUM 16
  #define Y8_GPIO_NUM 17
  #define Y7_GPIO_NUM 18
  #define Y6_GPIO_NUM 12
  #define Y5_GPIO_NUM 10
  #define Y4_GPIO_NUM 8
  #define Y3_GPIO_NUM 9
  #define Y2_GPIO_NUM 11

  #define VSYNC_GPIO_NUM 6
  #define HREF_GPIO_NUM 7
  #define PCLK_GPIO_NUM 13

#elif defined(CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3) || defined(CAMERA_MODEL_DFRobot_Romeo_ESP32S3)
  // --- CORRECTED Pinout for DFRobot boards ---
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM -1
  #define XCLK_GPIO_NUM 45
  #define SIOD_GPIO_NUM 1
  #define SIOC_GPIO_NUM 2

  #define Y9_GPIO_NUM 48
  #define Y8_GPIO_NUM 46
  #define Y7_GPIO_NUM 8
  #define Y6_GPIO_NUM 7
  #define Y5_GPIO_NUM 4
  #define Y4_GPIO_NUM 41
  #define Y3_GPIO_NUM 40
  #define Y2_GPIO_NUM 39

  #define VSYNC_GPIO_NUM 6
  #define HREF_GPIO_NUM 42
  #define PCLK_GPIO_NUM 5

#else
  #error "Camera model not selected or is not supported, please define one in platformio.ini"
#endif


// --- CameraBase Class Implementation ---

CameraBase::CameraBase() {}

bool CameraBase::init(CameraModel model) {
    // Pin configuration is set by the defines above
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM; // <-- Updated from pin_sscb_sda
    config.pin_sccb_scl = SIOC_GPIO_NUM; // <-- Updated from pin_sscb_scl
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;

    // Core configuration
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.xclk_freq_hz = 20000000;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    // Configure based on the camera sensor model
    switch (model) {
        case CameraModel::OV2640:
            config.pixel_format = PIXFORMAT_JPEG;
            // Using a smaller frame size for initial testing to reduce memory usage
            config.frame_size = FRAMESIZE_VGA; // <-- Changed from FRAMESIZE_UXGA
            break;
        case CameraModel::OV5640:
            config.pixel_format = PIXFORMAT_JPEG;
            config.frame_size = FRAMESIZE_QSXGA; // 2560x1920
            break;
        default:
            Serial.println("Unknown camera sensor model");
            return false;
    }

    // Initialize the camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    Serial.println("Camera initialized successfully");
    return true;
}