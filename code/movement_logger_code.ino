// Movement-Based Data Logger for Low-Power Applications

// Libraries
#include <Wire.h>
#include <BMA250.h>
#include <RTCZero.h>
#include <Adafruit_SleepyDog.h>
#include <SdFat.h>
#include <SPI.h>
#include <ArduinoLowPower.h>

char fileName[] = "datalog_final212.csv";

// Parameters to change:
#define INITIAL_SLEEP_TIME 2      // seconds; How long to sleep on power
#define RECORD_INTERVAL 50        // seconds; How long to collect data for during each recording interval
#define SLEEP_BETWEEN_INTERVAL 5  // seconds; How long to sleep between recording intervals
#define NO_MOVEMENT_SLEEP 8       // seconds;
#define SAVE_INTERVAL 5           // seconds; How long to delay between SD card saves
#define SLEEP_BETWEEN_SAMPLES 60  // milliseconds; Time between samples, determined by data rate of accelerometer
#define TIME_BETWEEN_SAMPLES 64   // milliseconds; Adjustment factor to keep timestamp accurate after sleep
#define DATA_COUNT 4              // Number of data fields you are collecting (x, y, z, temperature)
#define CHIP_SELECT 10

#define SECONDS_IN_MIN 60
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_DAY 86400
#define EPOCH_OFFSET 946684800  //Convert Y2K to epoch time, allows timestamp to start at 0
#define error(msg) sd.errorPrint(&SerialUSB)

RTCZero rtc;         // create RTC object
BMA250 accelSensor;  // create BMA250 accelerometer object
SdFat sd;            // create file system object
SdFile file;         // create file object

int wakeupTime;
int prevTime;
int lastSave;
int iteration = 0;

double prevMagnitude = 0.0;  // Previous magnitude
double threshold = 2.0;      // Sensitivity threshold

void setup() {

  // Initialize SD card
  if (!sd.begin(CHIP_SELECT, SD_SCK_MHZ(50))) {
    SPI.end();
    lightLED();
  }

  // Open the log file
  if (!file.open(fileName, O_WRONLY | O_CREAT | O_APPEND)) {
    SPI.end();
    lightLED();
  }

  detectReset();  // Debugging reset reason
  file.sync();

  // Initialize RTC
  rtc.begin();
  rtc.setEpoch(0);

  // Initialize Sensor
  Wire.begin();
  accelSensor.begin(BMA250_range_2g, BMA250_update_time_64ms);

  // Set alarm for initial sleep - allows delay before recording
  rtc.setAlarmEpoch(rtc.getEpoch() + INITIAL_SLEEP_TIME);
  rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
  LowPower.sleep();

  // Record time at wakeup, record initial timestamp time
  wakeupTime = rtc.getEpoch();
  prevTime = rtc.getEpoch() - EPOCH_OFFSET;
}

void loop() {

  // Data array
  double data[DATA_COUNT];

  // Get data from accelerometer
  getAccel(data);

  // Check if movement is detected
  if (checkMovement()) {

    // Create timestamp and record data if movement is detected
    String timeStampNow = getTimestamp(&prevTime);
    writeData(timeStampNow, data);

    // Sleep between samples
    Watchdog.sleep(SLEEP_BETWEEN_SAMPLES);

    int currentTime = rtc.getEpoch();

    // If recording time interval is over, go to sleep for the next interval
    if (currentTime - wakeupTime > RECORD_INTERVAL) {
      // Save data if necessary
      if (currentTime - lastSave > SAVE_INTERVAL) {
        lastSave = currentTime;
        file.sync();
      }

      rtc.setAlarmEpoch(currentTime + SLEEP_BETWEEN_INTERVAL);  // Set alarm for next sleep interval
      rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
      LowPower.sleep();
      wakeupTime = rtc.getEpoch();
    }
  } else {
    //Blink LED 2 times if no movement is detected
    // blinkLED(2);

    // If no movement, put the device into deep sleep to save power
    // int currentTime = rtc.getEpoch();
    // rtc.setAlarmEpoch(currentTime + NO_MOVEMENT_SLEEP); // Set alarm for next sleep interval
    // rtc.enableAlarm(rtc.MATCH_YYMMDDHHMMSS);
    // LowPower.sleep();
    // wakeupTime = rtc.getEpoch();
  }
}

// Function to blink the LED a specified number of times
// void blinkLED(int blinkCount) {
//   pinMode(LED_BUILTIN, OUTPUT);
//   for (int i = 0; i < blinkCount; i++) {
//     digitalWrite(LED_BUILTIN, HIGH);  // Turn LED on
//     delay(200);                       // Wait 200ms
//     digitalWrite(LED_BUILTIN, LOW);   // Turn LED off
//     delay(200);                       // Wait 200ms
//   }
// }

// Function to read accelerometer data
void getAccel(double pdata[]) {
  accelSensor.read();
  pdata[0] = accelSensor.X;
  pdata[1] = accelSensor.Y;
  pdata[2] = accelSensor.Z;
  pdata[3] = ((accelSensor.rawTemp * 0.5) + 24.0);
}

// Function to check movement based on accelerometer data
bool checkMovement() {
  accelSensor.read();
  double ax = accelSensor.X;
  double ay = accelSensor.Y;
  double az = accelSensor.Z;

  double magnitude = sqrt(ax * ax + ay * ay + az * az);

  if (abs(magnitude - prevMagnitude) > threshold) {
    prevMagnitude = magnitude;
    return true;
  }
  prevMagnitude = magnitude;
  return false;
}

// Function to create timestamp string
String getTimestamp(int *pprevTime) {
  int timeStamp = rtc.getEpoch() - EPOCH_OFFSET;

  if (timeStamp != *pprevTime) {
    iteration = 0;
    *pprevTime = timeStamp;
  } else {
    iteration += 1;
  }

  int t_ms = iteration * TIME_BETWEEN_SAMPLES;
  int t_sec = timeStamp % SECONDS_IN_MIN;
  int t_min = (timeStamp / SECONDS_IN_MIN) % SECONDS_IN_MIN;
  int t_hr = (timeStamp % SECONDS_IN_DAY) / SECONDS_IN_HOUR;
  int t_day = (timeStamp / SECONDS_IN_DAY);

  String colon = " : ";
  String dot = ".";
  String timeStampString = t_day + colon + t_hr + colon + t_min + colon + t_sec + dot + t_ms;
  return timeStampString;
}

// Function to write data to SD card
void writeData(String timeStamp, double pdata[]) {
  file.print(timeStamp);

  for (byte i = 0; i < DATA_COUNT; i++) {
    file.write(",");
    file.print(pdata[i]);
  }

  file.println();

  // Show success message on the serial monitor
  //  SerialUSB.print("Data successfully written to file. Timestamp: ");
  //  SerialUSB.println(timeStamp);
}

// Function to detect reset reason
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

// Function to light LED (for debugging)
void lightLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  while (1) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
