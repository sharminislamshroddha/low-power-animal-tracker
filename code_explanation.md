## Libraries Used  

### 1. `#include <BMA250.h>`  
- This library is used to interact with the **BMA250 accelerometer sensor**, which measures motion, acceleration, and tilt.  
- It helps read data like acceleration values in different axes (X, Y, Z).  

### 2. `#include <RTCZero.h>`  
- This library is for managing the **Real-Time Clock (RTC)** on supported Arduino boards.  
- It allows keeping track of time (seconds, minutes, hours, etc.) and enables putting the device to sleep in second-based intervals.  

### 3. `#include <Adafruit_SleepyDog.h>`  
- This library provides **watchdog timer (WDT)** functionality, which can reset the device if it gets stuck.  
- It also supports power-saving features like sleeping in milliseconds, but the maximum sleep duration is around **18 seconds**.  

### 4. `#include <SdFat.h>`  
- This library helps manage **SD cards**, allowing the Arduino to read from and write data to an SD card.  
- It provides efficient storage management and file operations.  

### 5. `#include <SPI.h>`  
- This is the **Serial Peripheral Interface (SPI)** communication library, used for fast data exchange between the microcontroller and peripherals like SD cards, sensors, and displays.  

### 6. `#include <ArduinoLowPower.h>`  
- This library helps in **power management**, allowing the microcontroller to enter low-power (sleep) modes to save battery when it’s not actively performing tasks.  
