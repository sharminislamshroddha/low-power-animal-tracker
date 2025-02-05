// Libraries
#include <Wire.h>
#include <BMA250.h>
#include <RTCZero.h>
#include <Adafruit_SleepyDog.h>
#include <SdFat.h>
#include <SPI.h>

char fileName[] = "continuous_log.csv";

// Parameters to change:
#define SAVE_INTERVAL 5           // seconds; How long to delay between SD card saves
#define SLEEP_BETWEEN_SAMPLES 60  // milliseconds; Time between samples, determined by data rate of accelerometer
#define TIME_BETWEEN_SAMPLES 64   // milliseconds; Adjustment factor to keep timestamp accurate after sleep
#define DATA_COUNT 4              // Number of data fields you are collecting (x, y, z, temperature)
#define CHIP_SELECT 10

#define SECONDS_IN_MIN 60
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_DAY 86400
#define EPOCH_OFFSET 946684800  // Convert Y2K to epoch time, allows timestamp to start at 0
#define error(msg) sd.errorPrint(&SerialUSB)

RTCZero rtc;         // create RTC object
BMA250 accelSensor;  // create BMA250 accelerometer object
SdFat sd;            // create file system object
SdFile file;         // create file object

int lastSave;
int iteration = 0;

double prevMagnitude = 0.0;  // Previous magnitude

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

  file.sync();

  // Initialize RTC
  rtc.begin();
  rtc.setEpoch(0);

  // Initialize Sensor
  Wire.begin();
  accelSensor.begin(BMA250_range_2g, BMA250_update_time_64ms);

  SerialUSB.println("Setup Complete. Logging data...");
}

void loop() {
  // Data array
  double data[DATA_COUNT];

  // Get data from accelerometer
  getAccel(data);

  // Create timestamp and record data
  int prevTime = rtc.getEpoch() - EPOCH_OFFSET;
  String timeStampNow = getTimestamp(&prevTime);
  writeData(timeStampNow, data);

  // Save data if necessary
  int currentTime = rtc.getEpoch();
  if (currentTime - lastSave > SAVE_INTERVAL) {
    lastSave = currentTime;
    file.sync();
  }

  delay(SLEEP_BETWEEN_SAMPLES);
}

// Function to read accelerometer data
void getAccel(double pdata[]) {
  accelSensor.read();
  pdata[0] = accelSensor.X;
  pdata[1] = accelSensor.Y;
  pdata[2] = accelSensor.Z;
  pdata[3] = ((accelSensor.rawTemp * 0.5) + 24.0);
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
  
  // Print data to Serial Monitor
  SerialUSB.print("Timestamp: ");
  SerialUSB.print(timeStamp);
  SerialUSB.print(" | X: ");
  SerialUSB.print(pdata[0]);
  SerialUSB.print(" | Y: ");
  SerialUSB.print(pdata[1]);
  SerialUSB.print(" | Z: ");
  SerialUSB.print(pdata[2]);
  SerialUSB.print(" | Temp: ");
  SerialUSB.println(pdata[3]);

  file.print(timeStamp);
  for (byte i = 0; i < DATA_COUNT; i++) {
    file.write(",");
    file.print(pdata[i]);
  }
  file.println();
}

// Function to light LED (for debugging)
void lightLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  while (1) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
}
