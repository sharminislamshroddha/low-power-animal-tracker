/*
  Accelerometer Detection Code for TinyZero
  -----------------------------------------
  This code checks for the presence of the BMA250 accelerometer. 
  If the accelerometer is detected, the onboard LED blinks rapidly 3 times. 
  Otherwise, the LED blinks slowly 3 times to indicate failure.
*/

#include <Wire.h>

#define BMA250_I2C_ADDR 0x18  // Accelerometer I2C address
#define LED_PIN 13            // Onboard LED pin (TinyZero)

void setup() {
    Wire.begin();
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);  // Set LED pin as output

    Wire.beginTransmission(BMA250_I2C_ADDR);
    if (Wire.endTransmission() == 0) {
        Serial.println("Accelerometer detected!");
        blinkLED(3, 200);  // Blink LED rapidly 3 times (200ms interval)
    } else {
        Serial.println("No accelerometer found.");
        blinkLED(3, 2000);  // Blink LED slowly 3 times (1 sec interval)
    }
}

void loop() {
    // No need for repeated actions in loop
}

// Function to blink the LED a given number of times with a delay
void blinkLED(int times, int delayTime) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayTime);
        digitalWrite(LED_PIN, LOW);
        delay(delayTime);
    }
}
