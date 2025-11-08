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

bool CameraBase::init(CameraModel model, pixformat_t format, bool vflip, bool hmirror) {
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
    config.pixel_format = format;

    if (format == PIXFORMAT_JPEG) {
        config.jpeg_quality = 12;
        config.fb_count = 1; // For JPEG, 1 buffer is often enough
    } else {
        config.fb_count = 2; // For other formats like RGB565, 2 buffers are recommended
    }

    // Configure based on the camera sensor model
    switch (model) {
        case CameraModel::OV2640:
            if (format == PIXFORMAT_RGB565) {
                config.frame_size = FRAMESIZE_QCIF;
            } else {
                // Otherwise, use a default like VGA for JPEG
                config.frame_size = FRAMESIZE_VGA;
            }
            break;
        case CameraModel::OV5640:
            // For this example, we'll keep a high resolution for JPEG on OV5640
            config.frame_size = FRAMESIZE_QSXGA;
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
// --- NEW: Set Vertical Flip and Horizontal Mirror ---
    sensor_t *s = esp_camera_sensor_get();
    if (s) {
        s->set_vflip(s, (int)vflip);
        s->set_hmirror(s, (int)hmirror);
    } else {
        Serial.println("Warning: Could not get sensor object to set flip/mirror.");
    }
    // --- END NEW ---

    Serial.println("Camera initialized successfully");
    return true;
}
/**
 * @brief Gets the frame buffer from the camera.
 * @return A pointer to the camera_fb_t struct, or nullptr on failure.
 * @note Remember to return the buffer using returnFrameBuffer()!
 */
camera_fb_t* CameraBase::getFrameBuffer() {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Failed to get frame buffer");
    }
    return fb;
}

/**
 * @brief Returns the frame buffer to the camera driver.
 * @param fb A pointer to the camera_fb_t struct to be returned.
 */
void CameraBase::returnFrameBuffer(camera_fb_t* fb) {
    if (fb) {
        esp_camera_fb_return(fb);
    }
}
/**
 * @brief Captures an RGB565 frame and prints it to Serial as a HEX array,
 *        swapping the byte order to match the TinyML example.
 *        Format: "0x[Byte2][Byte1], 0x[Byte4][Byte3], ..."
 */
void CameraBase::printFrameAsHex() {
    camera_fb_t *fb = getFrameBuffer();
    if (!fb) {
        Serial.println("Failed to get frame buffer.");
        return;
    }

    if (fb->format != PIXFORMAT_RGB565) {
        Serial.println("\nError: Pixel format is not RGB565. Cannot print HEX array.");
        returnFrameBuffer(fb);
        return;
    }
    
    // fb->len is the total number of bytes
    Serial.print("HEXADECIMAL_BYTES = [\n    ");
    for (size_t i = 0; i < fb->len; i += 2) {
        // Get the two bytes for one pixel
        byte msb = fb->buf[i];   // Most Significant Byte
        byte lsb = fb->buf[i+1]; // Least Significant Byte

        Serial.print("0x");
        
        // Print the bytes in swapped order (LSB then MSB) and ensure two digits with leading zero if needed
        if (lsb < 0x10) {
            Serial.print("0");
        }
        Serial.print(lsb, HEX);

        if (msb < 0x10) {
            Serial.print("0");
        }
        Serial.print(msb, HEX);
        
        // Print a comma and space, unless it's the very last pixel
        if (i < fb->len - 2) {
            Serial.print(", ");
        }

        // Add a newline every 8 pixels for readability
        if ((i/2 + 1) % 8 == 0) {
            Serial.println();
            Serial.print("    ");
        }
    }
    Serial.println("\n]");
    
    returnFrameBuffer(fb); 
}