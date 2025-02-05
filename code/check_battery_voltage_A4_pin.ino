/*
It continuously tracks battery changes.  
A typical LiPo battery has a voltage range of 3.0V (empty) to 4.2V (fully charged).
*/

#include <RTCZero.h>

// Battery monitoring
#define BATTERY_PIN A4                // Use the analog pin connected to the battery divider
#define ADC_REFERENCE 3.3             // ADC reference voltage (3.3V for most TinyZero boards)
#define VOLTAGE_DIVIDER_RATIO 2.0     // Adjust based on your voltage divider (e.g., 1MΩ + 470kΩ ≈ 3.13 ratio)
#define SLEEP_BETWEEN_SAMPLES 5000    // 5 seconds in milliseconds

// Constants
#define EPOCH_OFFSET 946684800        // Convert Y2K to epoch time, allows timestamp to start at 0

// Objects
RTCZero rtc;                          // Create RTC object

// Variables
int wakeupTime;
int iteration = 1;
float totalCapacity = 2000.0; // Battery capacity in mAh

// Function prototypes
float readBatteryVoltage();
float getBatteryPercentage(float voltage);
float getRemainingCapacity(float voltage, float totalCapacity);

void setup() {
  // Initialize Serial for debugging
  SerialUSB.begin(9600);
  while (!SerialUSB); // Wait for SerialUSB to be ready

  // Initialize RTC
  rtc.begin();
  rtc.setEpoch(0);
}

void loop() {
  // Show iteration on the serial monitor
  SerialUSB.print("Iteration: ");
  SerialUSB.println(iteration);

  iteration = iteration + 1;

  // Read and display battery voltage and percentage
  float batteryVoltage = readBatteryVoltage();
  float batteryPercent = getBatteryPercentage(batteryVoltage);
  float remainingCapacity = getRemainingCapacity(batteryVoltage, totalCapacity);

  SerialUSB.print("Battery Voltage: ");
  SerialUSB.println(batteryVoltage);

  SerialUSB.print("Battery Percentage: ");
  SerialUSB.println(batteryPercent);

  SerialUSB.print("Remaining Capacity: ");
  SerialUSB.print(remainingCapacity);
  SerialUSB.println(" mAh\n");

  // Sleep for 5 seconds
  delay(SLEEP_BETWEEN_SAMPLES);
}

// Function to read battery voltage
float readBatteryVoltage() {
  analogReadResolution(12); // Set ADC to 12-bit resolution (0-4095)
  int rawValue = 0;
  for (int i = 0; i < 20; i++) {
    rawValue += analogRead(BATTERY_PIN);
  }
  rawValue /= 20;
  float voltage = (rawValue * ADC_REFERENCE) / 4095.0; // Convert to voltage
  voltage *= VOLTAGE_DIVIDER_RATIO; // Adjust for voltage divider
  return voltage;
}

// Function to estimate battery percentage
float getBatteryPercentage(float voltage) {
  // LiPo battery voltage range (adjust for your battery chemistry):
  float fullVoltage = 4.2; // 100% charge
  float emptyVoltage = 3.3; // 0% charge (safe cutoff)

  voltage = constrain(voltage, emptyVoltage, fullVoltage);
  return (voltage - emptyVoltage) / (fullVoltage - emptyVoltage) * 100.0;
}

// Function to calculate remaining capacity
float getRemainingCapacity(float voltage, float totalCapacity) {
  float percentage = getBatteryPercentage(voltage);
  return (percentage / 100.0) * totalCapacity;
}