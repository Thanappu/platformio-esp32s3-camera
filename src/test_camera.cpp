// src/test_camera.cpp
#include <Arduino.h>
#include "camera_lib.h"

// Define GPIOs for built-in peripherals
#define RGB_BUILTIN 48
#define SWITCH_BUILTIN 0

CameraBase cam;

int lastButtonState = HIGH; 
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void signalSuccess() {
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    delay(100);
    neopixelWrite(RGB_BUILTIN, 0, 255, 0);   // Green
    delay(150);
    neopixelWrite(RGB_BUILTIN, 0, 0, 255);   // Blue
    delay(150);
    neopixelWrite(RGB_BUILTIN, 255, 255, 255); // White
    delay(150);
}
void setIdleStateLED() {
    neopixelWrite(RGB_BUILTIN, 0, 0, 30); // Dim Blue
}
void setup() {
    // Using INPUT_PULLUP as confirmed by the diagnostic test.
    pinMode(SWITCH_BUILTIN, INPUT_PULLUP);
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    Serial.begin(115200);
    delay(2000);
    bool init_success = cam.init(CameraModel::OV2640, PIXFORMAT_RGB565, true);
    if (init_success) {
        Serial.println("==============================================");
        Serial.println("  CAMERA INITIALIZED SUCCESSFULLY in RGB565!");
        Serial.println("  System is ready.");
        Serial.println("==============================================");
        setIdleStateLED();
    } else {
        Serial.println("!!! Camera initialization failed. Halting execution.");
        neopixelWrite(RGB_BUILTIN, 255, 0, 0);
        while (1) { delay(1000); }
    }
}

void loop() {
    int currentButtonState = digitalRead(SWITCH_BUILTIN);
    if (currentButtonState == LOW && lastButtonState == HIGH) {
        Serial.println("\nButton press detected! Capturing frame and printing HEX data...");
        neopixelWrite(RGB_BUILTIN, 200, 200, 0); // Yellowish-White: Processing
        Serial.println("--- BEGIN HEX DATA ---");
        cam.printFrameAsHex(); // This prints the "[0x..., ...]" string.
        Serial.println(); // Add a newline for clean formatting after the array.
        Serial.println("--- END HEX DATA ---");
        Serial.println("Capture and Print SUCCESS.");
        signalSuccess();
        Serial.println("\nSystem is ready. Press the button to capture again.");
        setIdleStateLED();
        delay(200); 
    }
    lastButtonState = currentButtonState;
}