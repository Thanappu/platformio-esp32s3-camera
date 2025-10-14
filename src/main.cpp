// src/main.cpp
#include <Arduino.h>
#include "camera_lib.h"
CameraBase cam;
void setup() {
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
void loop() { delay(5000); }