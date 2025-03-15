# Clarifications  

## Changes made, compared to Brian's guide 

**Code-Related Changes (2 Approaches)**  
1. **No Motion, No Data Saving**  
   - If the device does not detect any motion, it will not save data to the SD card.  
   - Everything else follows Brian’s instructions.  

2. **Sleep Mode for Power Efficiency**  
   - If the device does not detect any motion, it will enter **sleep mode for 15 minutes**.  
   - After this period, the device will wake up and check the accelerometer:  
     - **If motion is detected:** The device will collect data.  
     - **If no motion is detected:** The device will return to sleep mode.  
   - The number of wake up cycles per day will depend on the turtle’s movement, not a fixed interval.
   - **Pros:** The device won’t follow a fixed interval. So it will **gather more data during movement periods**.
   - **Cons:** Since the turtle’s movement is unpredictable, it’s hard to know **how long the device will last on a single battery**.


## Data collection or switch back from low power only when motion sensed
- Our **TinyZero board does not have a built-in accelerometer**, and the battery is only connected to the TinyZero board.  
- As a result, when the device enters sleep mode, the accelerometer **stops detecting motion**, preventing the device from waking up again.  


## Low power mode or timed switch off/on in case of long time inactivity 
- The solution is already mentioned in point 2 of the first question’s answer.

