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

<br>

## Parameters Configuration  

The following parameters control various aspects of the device's operation.  

### File Name  
```cpp  
char fileName[] = "logfile.csv";  
```
- The name of the file where collected data will be stored on the SD card.

###  Timing Parameters
```cpp  
 #define INITIAL_SLEEP_TIME 240  // hours 
```
- The device will sleep for 240 hours (10 days) after powering on before it starts recording data, helping to save battery if immediate data collection is not needed.

```cpp  
 #define RECORD_INTERVAL 15  // seconds 
```
- The duration for which the device will collect data in each recording session, set to 15 seconds.

```cpp  
 #define SLEEP_BETWEEN_INTERVAL 900  // seconds 
```
- The device will sleep for 900 seconds (15 minutes) between recording sessions to save power.

```cpp  
 #define SAVE_INTERVAL 14400  // seconds 
```
- Data is saved to the SD card every 14400 seconds (4 hours) to reduce power consumption since writing to the SD card consumes significant energy.

```cpp  
#define SLEEP_BETWEEN_SAMPLES 60  // milliseconds 
```
- The device will wait 60 milliseconds between each data sample, which is set according to the accelerometer’s data rate.

```cpp  
 #define TIME_BETWEEN_SAMPLES 64  // milliseconds 
```
- Adjustment factor to ensure timestamps remain accurate after sleep periods.
 
### Data Collection Parameters

```cpp  
 #define DATA_COUNT 4
```
- The number of data values collected per sample (e.g., for X, Y, Z accelerations + temperature, DATA_COUNT=4).

### Hardware Settings 
```cpp  
 #define CHIP_SELECT 10
```
- Defines the chip select (CS) pin for the SD card module, typically required for SPI communication.

### Time Calculations
```cpp  
 #define SECONDS_IN_MIN 60
 #define SECONDS_IN_HOUR 3600
 #define SECONDS_IN_DAY 86400
```
- Constants used for time calculations to convert minutes, hours, and days to seconds.
 
```cpp  
 #define EPOCH_OFFSET 946684800  // Y2K (Year 2000) epoch offset
```
- This value represents the number of seconds from the Unix epoch (1970) to January 1, 2000, ensuring timestamps start from year 2000 instead of 1970.
- This offset (January 1, 2000, at 00:00:00 UTC) is likely used to make timestamps smaller and easier to work with, as subtracting 30 years (1970 to 2000) reduces the numbers recorded.
- **Why Epoch Format?**
<br>Epoch format is compact, efficient for storage and calculations, and timezone-independent, making it ideal for resource-constrained devices. It can easily be converted to human-readable formats during data analysis.
- [Unix Timestamp Converter](https://www.unixtimestamp.com/)

### Error Handling
```cpp  
 #define error(msg) sd.errorPrint(&SerialUSB)
```
- A macro that prints error messages to the serial monitor via SerialUSB whenever an SD card error occurs.

### Integer Variables (int) 
```cpp  
 int wakeupTime;  
```
- **Purpose:** Stores the real-time clock (RTC) value when the processor wakes up from sleep mode.
- **Use case:** Helps track when the microcontroller wakes up for data collection.
 
```cpp  
 int prevTime;  
```
- **Purpose:** Stores the RTC time at each sample, likely in seconds.
- **Use case:** Used to track time intervals and ensure accurate timestamping for collected data.
 
```cpp  
 int lastSave;  
```
- **Purpose:** Records the last time data was saved to the SD card, in seconds.
- **Use case:** Helps in managing the SD card write operations efficiently by saving data periodically rather than too frequently.
 
```cpp  
 int iteration = 0;
```
- **Purpose:** Tracks the number of times data has been collected or the current loop iteration.
- **Use case:** Used for counting and debugging purposes to monitor the number of data collection cycles.
 

### Object Declarations
```cpp  
 RTCZero rtc;
```
- **Purpose:** Creates an instance of the RTCZero library, which allows the microcontroller to keep track of time (hours, minutes, seconds, etc.), even when in sleep mode.
- **Use case:** Used to schedule wake-ups, timestamp data, and control sleep intervals accurately.

```cpp  
 BMA250 accelSensor;  
```
- **Purpose:** Creates an instance of the BMA250 accelerometer sensor object.
- **Use case:** Allows interaction with the accelerometer to collect motion data such as X, Y, Z acceleration and temperature.

```cpp  
 SdFat sd;  
```
- **Purpose:** Creates an instance of the SdFat library to manage SD card operations like opening, reading, and writing files.
- **Use case:** Handles SD card initialization and file system management.
 
```cpp  
 SdFile file;  
```
- **Purpose:** Creates an instance of an SdFile object to represent an individual file on the SD card.
- **Use case:** Used to open, write, and close data log files (logfile.csv) to store collected sensor data.
<br>

## Setup Function Overview
### SD Card Initialization and Error Handling 
```cpp  
 if (!sd.begin(CHIP_SELECT, SD_SCK_MHZ(50))) {
      SPI.end();
      lightLED();
  }
```
- **Purpose:** Initializes the SD card with a clock speed of 50 MHz.
- **Error Handling:** If the SD card fails to initialize, the SPI communication is ended, and an LED is lit as an error indicator.
  
```cpp  
 if (!file.open(fileName, O_WRONLY | O_CREAT | O_APPEND)) {
      SPI.end();
      lightLED();
  }
```
- **Purpose:** Opens the file (logfile.csv) on the SD card in write mode, creating it if it doesn’t exist and appending new data to it.
- **Error Handling:** If file creation/opening fails, the SPI is disabled, and the LED is turned on to indicate the issue.

### Debugging: Detecting Reset Reason
```cpp  
 detectReset();
 file.sync();
```
- **Purpose:** detectReset() checks why the system restarted (e.g., power failure, watchdog timeout), useful for debugging.
- **Syncing:** file.sync() ensures any pending data in memory is written to the SD card.

### RTC (Real-Time Clock) Initialization
```cpp  
 rtc.begin();
 rtc.setEpoch(0);
```
- **Purpose:** Initializes the real-time clock (RTC) and sets the epoch (starting time) to 0, corresponding to the Y2K offset defined in the code.

### Sensor Initialization
```cpp  
 Wire.begin();
 accelSensor.begin(BMA250_range_2g, BMA250_update_time_64ms);
```
- **Purpose:** Initializes I2C communication using Wire.begin(), and sets up the BMA250 accelerometer with:<br>
	•	BMA250_range_2g: Configures the sensor to measure acceleration within ±2g range.<br>
	•	BMA250_update_time_64ms: Sets the data update rate to every 64 milliseconds.

### Initial Sleep Setup (Delayed Recording Start)
```cpp  
 rtc.setAlarmEpoch(rtc.getEpoch() + INITIAL_SLEEP_TIME * SECONDS_IN_HOUR);
 rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
 LowPower.sleep();
```
- **Purpose:** Sets up an RTC alarm to delay the start of data recording for a predefined period (INITIAL_SLEEP_TIME, e.g., 240 hours).
- **Operation:**
  - `rtc.getEpoch()` gets the current time.
  - `+ INITIAL_SLEEP_TIME * SECONDS_IN_HOUR` adds the desired delay (e.g., 10 days).
  - `rtc.enableAlarm()` activates the alarm.
  - `LowPower.sleep()` puts the microcontroller into sleep mode to save power until the alarm triggers wake-up.

### Wake-Up Time Recording
```cpp  
 wakeupTime = rtc.getEpoch();
 prevTime = rtc.getEpoch() - EPOCH_OFFSET;
```
- **Purpose:** After waking up, these lines:
  - Store the current epoch time in wakeupTime to track when the system resumed operation.
  - Adjust the recorded time by subtracting the Y2K offset (EPOCH_OFFSET) to keep timestamps relative to the year 2000.
<br>

## Explanation of detectReset() Function
- This function checks the reason why the system was reset and logs it to the SD card file.

```cpp  
 void detectReset() {
   if (REG_PM_RCAUSE == PM_RCAUSE_SYST) {
     file.println(F("Reset requested by system"));
   }
   if (REG_PM_RCAUSE == PM_RCAUSE_WDT) {
     file.println(F("Reset requested by Watchdog"));
   }
   if (REG_PM_RCAUSE == PM_RCAUSE_EXT) {
     file.println(F("External reset requested"));
   }
   if (REG_PM_RCAUSE == PM_RCAUSE_BOD33) {
     file.println(F("Reset brown out 3.3V"));
   }
   if (REG_PM_RCAUSE == PM_RCAUSE_BOD12) {
     file.println(F("Reset brown out 1.2v"));
   }
   if (REG_PM_RCAUSE == PM_RCAUSE_POR) {
     file.println(F("Normal power on reset"));
   }
 }
```

### Reset Causes
1. **PM_RCAUSE_SYST**  
   - **Reason:** Reset requested by the system (software-triggered reset).  

2. **PM_RCAUSE_WDT**  
   - **Reason:** Reset triggered by the Watchdog Timer (used to recover from system hangs).  

3. **PM_RCAUSE_EXT**  
   - **Reason:** External reset (e.g., a reset button was pressed).  

4. **PM_RCAUSE_BOD33**  
   - **Reason:** Brown-Out Detection at 3.3V (voltage dropped below a safe level for operation). 

5. **PM_RCAUSE_BOD12**  
   - **Reason:** Brown-Out Detection at 1.2V (core voltage dropped below the safe operating level). 

6. **PM_RCAUSE_POR**  
   - **Reason:** Power-On Reset (normal reset when powering up the device).

<br>

## Explanation of lightLED() Function
- This function lights up the built-in LED to indicate a critical error (e.g., SD card failure) and halts the system.
- Halts the system to prevent it from continuing with faulty or incomplete initialization.

```cpp  
 void lightLED() {
   pinMode(LED_BUILTIN, OUTPUT);
   while (1) {
     digitalWrite(LED_BUILTIN, HIGH);
   }
 }
```
<br>

## Explanation of getAccel() Function
- The function reads data from the BMA250 accelerometer and stores the values of X, Y, Z accelerations(integer numbers) and temperature in an array pdata.
- These values represent the force applied to the sensor along each axis, measured in relation to the gravitational force.
  
```cpp  
 void getAccel(double pdata[]) {
   accelSensor.read();
   pdata[0] = accelSensor.X;
   pdata[1] = accelSensor.Y;
   pdata[2] = accelSensor.Z;
   pdata[3] = ((accelSensor.rawTemp * 0.5) + 24.0);
 }
```

### Understanding Accelerometer Data
1. **Raw Data:**  
   - The accelerometer outputs unscaled integer data.  

2. **Conversion to g-force:**  
   - Raw values are divided by the scaling factor (e.g., 256 for a 2g range) to get g-force.  

3. **Conversion to m/s²:**  
   - Multiply g-force by 9.8 m/s² to get acceleration in meters per second squared.  

4. **Stored Values:**  
   - **X, Y, Z:** Accelerations in each axis:  
     - **X:** Left/Right  
     - **Y:** Forward/Backward  
     - **Z:** Up/Down (close to 1g due to gravity).  
   - Values depend on sensor orientation and external forces. If the sensor is still, Z will be close to 1g because of gravity, and X, Y will be close to 0g.
   - **Temperature (°C):** The sensor's raw temperature value is scaled by multiplying it by 0.5 and offset by adding 24.0 to get the temperature in Celsius.

<br>

## Explanation of getTimestamp() Function 
- The getTimestamp function generates a precise timestamp for each data sample by combining the RTC (Real-Time Clock) data and an estimated millisecond component. Since the RTC only provides whole seconds, the function uses the accelerometer’s sampling rate to estimate milliseconds.
```cpp  
String getTimestamp(int *pprevTime) {
  int timeStamp = rtc.getEpoch() - EPOCH_OFFSET;

  //RTC only returns whole seconds. Therefore to get a timestamp at the millisecond
  //level, we estimate it. We know the sampling rate of the accelerometer. Therefore, each time
  //the RTC changes, for example from second 11 to second 12, we start a counter at 0.
  //This counter increments each time the accelerometer collects data. The millisecond
  //portion of the time stamp is then estimated as (iteration * time_between_samples)
  
  //If the RTC has changed, set prevTime to new second, and reset the counter to 0
  if (timeStamp != *pprevTime) {
    iteration = 0;
    *pprevTime = timeStamp;
  }

  //If RTC is stil on the same second, increment the counter
  else {
    iteration += 1;
  }

  //Calculate timestamp values and concatenate
  int t_ms = iteration * TIME_BETWEEN_SAMPLES;
  int t_sec = timeStamp % SECONDS_IN_MIN;
  int t_min = (timeStamp / SECONDS_IN_MIN) % SECONDS_IN_MIN;
  int t_hr =  (timeStamp % SECONDS_IN_DAY) / SECONDS_IN_HOUR;
  int t_day = (timeStamp / SECONDS_IN_DAY);

  String colon = " : ";
  String dot = ".";

  String timeStampString = t_day + colon + t_hr + colon +
                           t_min + colon + t_sec + dot + t_ms;
  return timeStampString;
}
```
- **RTC Epoch Time:** rtc.getEpoch() gives the current time in seconds since a defined epoch (e.g., the year 2000).
- **Millisecond Estimation:** Tracks the number of samples collected within the current second using a counter (iteration). Multiplies it by TIME_BETWEEN_SAMPLES to estimate milliseconds.
- **Timestamp Components:** Breaks the time into days, hours, minutes, seconds, and milliseconds.
- **Format:** Combines these into a readable format: Day : Hour : Minute : Second.Millisecond.
- **Purpose:** Provides precise timestamps for accelerometer data, essential for time-sensitive applications. Example format: 1 : 12 : 34 : 56.128 (Day 1, 12:34:56 and 128 ms).

<br>

## Explanation of writeData() Function
- The writeData function saves a timestamp and accelerometer data to a file on the SD card in CSV format.
```cpp  
 void writeData(String timeStamp, double pdata[]) {
   file.print(timeStamp);
   for (byte i = 0; i < DATA_COUNT; i = i + 1) {
     file.write(",");
     file.print(pdata[i]);
   }
   file.println();
 }
```
| TimeStamp          | X     | Y    | Z    | Temperature (°C) |
|--------------------|-------|------|------|------------------|
| 1 : 12 : 34 : 56.128 | -114  | 72   | 221  | 29.5             |
| 1 : 12 : 34 : 56.192 | -114  | 73   | 221  | 29.5             |
| 1 : 12 : 34 : 56.256 | -115  | 73   | 221  | 29.5             |

<br>

## Explanation of loop() Function 
- After the setup() function (which runs only once at the start) finishes, the loop() function is executed repeatedly in an infinite cycle.
```cpp  
void loop() {

  // Data array
  double data[DATA_COUNT];

  //Get data
  getAccel(data);

  //Create timestamp
  String timeStampNow = getTimestamp(&prevTime);

  //Record timestamp and data
  writeData(timeStampNow, data);

  // Sleep bewteen samples
  Watchdog.sleep(SLEEP_BETWEEN_SAMPLES);

  int currentTime = rtc.getEpoch();
  //If current recording time interval is over, go to sleep for sleep period
  if (currentTime - wakeupTime > RECORD_INTERVAL) {

    //If data has not been saved to SD card recently, save the data
    if (currentTime - lastSave > SAVE_INTERVAL) {

      lastSave = currentTime;
      file.sync();
    }
    
    rtc.setAlarmEpoch(currentTime + SLEEP_BETWEEN_INTERVAL); //Set alarm SLEEP_BETWEEN_INTERVAL seconds from now
    rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
    LowPower.sleep();
    wakeupTime = rtc.getEpoch();
  }
}
```

### 1. Collect Accelerometer Data
- The function calls `getAccel(data)` to read accelerometer values (X, Y, Z, and temperature) and stores them in a `data` array.

### 2. Create Timestamp
- A timestamp is generated using `getTimestamp(&prevTime)`, which estimates the current time down to milliseconds.

### 3. Write Data to SD Card
- The function writes the timestamp and collected accelerometer data to the SD card using `writeData(timeStampNow, data)`.

### 4. Sleep Between Samples
- The system enters a short sleep (defined by `SLEEP_BETWEEN_SAMPLES`) using `Watchdog.sleep()` to reduce power consumption.

### 5. Check Recording Interval
- It calculates the time elapsed since the last wakeup (`currentTime - wakeupTime`).
- **If the recording interval (`RECORD_INTERVAL`) is over:**
  - **Save Data to SD Card:**
    - If enough time has passed since the last save (`SAVE_INTERVAL`), it synchronizes the data on the SD card with `file.sync()`.
  - **Enter Deep Sleep:**
    - Sets an alarm for `SLEEP_BETWEEN_INTERVAL` seconds from the current time.
    - Puts the system into deep sleep using `LowPower.sleep()`.
    - Updates the `wakeupTime` after waking up.
