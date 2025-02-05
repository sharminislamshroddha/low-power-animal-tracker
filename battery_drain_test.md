# Battery Drain Test  

This document shows the battery status at different time intervals during the test. The test was conducted while running a continuous file-writing process, where data was written to a file every **64 milliseconds**.  

## How Battery Information Was Collected
A **LiPo battery** with a voltage range of **3.0V (empty) to 4.2V (fully charged)** was monitored using an **analog pin (A4)** on the microcontroller. The battery voltage was measured using an **ADC (Analog-to-Digital Converter)** with a **voltage divider circuit**.

The following formula was used to convert the raw ADC reading into voltage:

$$
V_{\text{battery}} = \left(\frac{\text{ADC reading} \times V_{\text{reference}}}{4095}\right) \times \text{Voltage Divider Ratio}
$$

where:
- $V_{\text{reference}} = 3.3V$ (for the TinyZero board)
- **Voltage Divider Ratio** = **2.0** (depends on resistor values)
- **ADC resolution** = **12-bit (0-4095)**

### Battery Charge Estimation
Battery percentage was estimated using a **linear approximation** between 4.2V (100%) and 3.3V (0%):

$$
\text{Battery Percentage} = \frac{(V_{\text{battery}} - 3.3)}{(4.2 - 3.3)} \times 100
$$

The **remaining battery capacity (mAh)** was calculated as:

$$
\text{Remaining Capacity} = \frac{\text{Battery Percentage}}{100} \times \text{Total Capacity (mAh)}
$$


## Code Used for Battery Monitoring
The following Arduino code was used to track battery voltage and charge level:

```cpp
// Battery monitoring code
#include <RTCZero.h>

#define BATTERY_PIN A4
#define ADC_REFERENCE 3.3
#define VOLTAGE_DIVIDER_RATIO 2.0
#define SLEEP_BETWEEN_SAMPLES 5000

RTCZero rtc;
float totalCapacity = 2000.0; // Battery capacity in mAh

void setup() {
  SerialUSB.begin(9600);
  while (!SerialUSB);
  rtc.begin();
  rtc.setEpoch(0);
}

void loop() {
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

  delay(SLEEP_BETWEEN_SAMPLES);
}

// Function to read battery voltage
float readBatteryVoltage() {
  analogReadResolution(12);
  int rawValue = 0;
  for (int i = 0; i < 20; i++) rawValue += analogRead(BATTERY_PIN);
  rawValue /= 20;
  return (rawValue * ADC_REFERENCE / 4095.0) * VOLTAGE_DIVIDER_RATIO;
}

// Function to estimate battery percentage
float getBatteryPercentage(float voltage) {
  return constrain((voltage - 3.3) / (4.2 - 3.3) * 100.0, 0, 100);
}

// Function to calculate remaining capacity
float getRemainingCapacity(float voltage, float totalCapacity) {
  return (getBatteryPercentage(voltage) / 100.0) * totalCapacity;
}
```

## Initial State (0 Hours)  
- **Battery Voltage:** 4.16 V  
- **Battery Percentage:** 96.08%
- **Remaining Capacity: 1921.53 mAh**

![Battery at 0 Hours](images/battery_0h.png)  

## After 3 Hours  
- **Battery Voltage:** 4.12 V  
- **Battery Percentage:** 91.24%
- **Remaining Capacity: 1824.83 mAh**

![Battery at 0 Hours](images/battery_3h.png)  

## After 10 Hours  
- **Battery Voltage:** 4.06 V  
- **Battery Percentage:** 84.26%  
- **Remaining Capacity: 1685.14 mAh**

![Battery at 10 Hours](images/battery_10h.png)  

## Summary  
During the test, a **continuous file-writing process** was running, writing data every **64 milliseconds**. The images above show how the battery voltage and charge decreased over time under these conditions.    


