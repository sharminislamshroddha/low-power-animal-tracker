// INA219 Battery Health Monitoring Code for TinyZero

#include <Wire.h>
#include <Adafruit_INA219.h>

// Create an instance of the INA219 sensor with the correct address
Adafruit_INA219 ina219(0x18);

// Constants for battery voltage (adjust these according to your battery type)
const float fullVoltage = 4.2;  // Full charge voltage for a Li-ion battery
const float emptyVoltage = 3.0;  // Empty voltage for a Li-ion battery
const float batteryCapacity = 2000;  // Full capacity of your battery in mAh (e.g., 2000mAh)

// Variables to track battery capacity usage over time
unsigned long lastTime = 0;
float totalCurrent = 0;  // To accumulate current over time for battery capacity estimation

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB);  // Wait for Serial Monitor to open

  SerialUSB.println("Adafruit INA219 Test");

  // Initialize the sensor
  if (!ina219.begin()) {
    SerialUSB.println("Failed to find INA219 sensor!");
    while (1);
  }
  SerialUSB.println("INA219 sensor found!");
}

void loop() {
  // Read data from INA219
  float shuntVoltage = ina219.getShuntVoltage_mV();
  float busVoltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  float power = ina219.getPower_mW();

  // Calculate the time difference since the last reading (in hours)
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 3600000.0;  // Convert milliseconds to hours

  // Update the total current consumption for battery capacity estimation
  totalCurrent += current * deltaTime;

  // Estimate remaining battery capacity (in mAh)
  float remainingCapacity = batteryCapacity - totalCurrent;

  // Estimate battery health (based on voltage)
  float batteryHealth = mapBatteryVoltageToHealth(busVoltage);

  // Print sensor data and battery status
  SerialUSB.print("Bus Voltage:   "); SerialUSB.print(busVoltage); SerialUSB.println(" V");
  SerialUSB.print("Shunt Voltage: "); SerialUSB.print(shuntVoltage); SerialUSB.println(" mV");
  SerialUSB.print("Current:       "); SerialUSB.print(current); SerialUSB.println(" mA");
  SerialUSB.print("Power:         "); SerialUSB.print(power); SerialUSB.println(" mW");
  
  // Print battery health and capacity
  SerialUSB.print("Remaining Capacity: "); SerialUSB.print(remainingCapacity); SerialUSB.println(" mAh");
  SerialUSB.print("Battery Health: "); SerialUSB.print(batteryHealth); SerialUSB.println(" %");

  // Update the last time
  lastTime = currentTime;

  delay(2000);  // Delay for 2 seconds
}

// Function to estimate battery health based on the bus voltage
float mapBatteryVoltageToHealth(float voltage) {
  if (voltage >= fullVoltage) {
    return 100;  // Fully charged, 100% healthy
  } else if (voltage <= emptyVoltage) {
    return 0;  // Completely discharged, 0% healthy
  } else {
    return (voltage - emptyVoltage) / (fullVoltage - emptyVoltage) * 100;
  }
}
