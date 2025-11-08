// src/main.cpp
#include <Arduino.h>
#include "camera_lib.h"
#define LED_BUILTIN 48
CameraBase cam;
void setup() {
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  Serial.begin(115200);
  delay(2000);
  Serial.println("--- Starting Camera Initialization (Final Attempt) ---");
  bool init_success = cam.init(CameraModel::OV2640); 
  if (init_success) {
    Serial.println("==============================================");
    Serial.println("      CAMERA INITIALIZED SUCCESSFULLY!");
    Serial.println("==============================================");
  } else {
    Serial.println("!!! Camera initialization failed. Halting execution.");
    while (1) { delay(1000); }
  }
}
void loop() {
  Serial.println("HELLO TinyML!");
  delay(2000);
}