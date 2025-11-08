// src/main.cpp
#include <Arduino.h>
#include "camera_lib.h"
#define RGB_BUILTIN 48
#define SWITCH_BUILTIN 0
CameraBase cam;
bool switch_state = false;
void setup() {
  pinMode(SWITCH_BUILTIN, INPUT);
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
switch_state = digitalRead(SWITCH_BUILTIN);
  if (switch_state == false){
    /* code */
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);   // แดง
    delay(100);
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);   // เขียว
    delay(100);
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);   // น้ำเงิน
    delay(100);
  }
  else{
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);   // แดง
    delay(1000);
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);   // เขียว
    delay(1000);
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);   // น้ำเงิน
    delay(1000);
  }
}