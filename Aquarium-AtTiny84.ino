/* 
 * Alauddin Ansari
 * 2019-06-01
 * ATtiny84 Low-Powered Aquarium Automation
 */

#include <TimeLib.h>
#include <DS1307RTC.h>
#include "WatchdogAtTiny84.h"

// Watchdog Settings
const int watchdog_mode = 9;  // 6=1sec (0.5sec), 7=2sec (1sec), 8=4sec (3.2sec), 9=8sec (7.4sec)
const int wtd_cnt_reset = 40; // Actual task will be executed after this counter.

// General variables
int wtd_cnt = 0;              // Counter for task execution comparision.
int last_hour = -1;           // Last hour to compare current hour change, used in sync time drift.
int last_date = 0;            // Last date to compare date change, used to drop food multiple time a day.
boolean initialized = false;  // Flag to check whether running first time after powering up, used in sync time drift.

/* 
 * Pins Setting
 * -------------------------
 * Pin6 - RTC SDA (Phy Pin5) - occupied
 * Pin4 - RTC SCL (Phy Pin7) - occupied 
 */
const int FoodPin = 0;            // Food Pin

const int FilterPin = 1;          // Filter Pin
const int LightPin = 2;           // Light Pin
const int ThirdSwitchPin = 3;     // Third Pin

const int PowerIndicPin = 5;      // Power Indicator Pin
const int MainPowerPin = 7;       // Main Power Switch Pin

// Time schedule (in Hour) -- Same on and off value means disable
const int Filter_on = 5;
const int Filter_off = 23;

const int Light_on = 8;
const int Light_off = 19;

const int ThirdSwitch_on = 0;
const int ThirdSwitch_off = 0;

const int FoodDose_at[] = {8, 18};        // Array: Drop food at multiple times
const int FoodDropQty[] = {1000, 600};    // Microseconds till what DC motor will run to drop food. More value, more food.
const int FoodDose_count = 2;             // Size of array count
int FoodDropped_at = -1;                   // Hour in which food is dropped.

void setup() {
  pinMode(FoodPin, OUTPUT);
  pinMode(FilterPin, OUTPUT);
  pinMode(LightPin, OUTPUT);
  pinMode(ThirdSwitchPin, OUTPUT);
  pinMode(PowerIndicPin, OUTPUT);
  pinMode(MainPowerPin, OUTPUT);

  digitalWrite(FoodPin, LOW);
  digitalWrite(FilterPin, LOW);
  digitalWrite(LightPin, LOW);
  digitalWrite(ThirdSwitchPin, LOW);
  digitalWrite(PowerIndicPin, HIGH);
  digitalWrite(MainPowerPin, LOW);

  delay(100);
  connectDS1307();
  delay(500);

  setup_watchdog(watchdog_mode);
}

void loop() {
  if (wtd_cnt == 0) {
    // Watchdog sleep mode also stops Timer.
    // Hence, get current time from RTC module
    setTime(RTC.get());
    delay(100);
  
    if (validateTime()) {
      int _date = day();
      int _hour = hour();
      int _min = minute();
  
      // Dropping food
      for(int i = 0; i < FoodDose_count; i++)
      {
        int food_hour = FoodDose_at[i];
        if (food_hour == _hour && (last_date != _date || FoodDropped_at != _hour))
        {
          FoodDropped_at = _hour;
          last_date = _date;
          
          dropFood(FoodPin, FoodDropQty[i]); // in functions.ino file
          delay(500);
        }
      }
  
      // Filter program
      if (FoodDropped_at == _hour && _min < 30) {
        // Switch off filter during food hour.
        // It helps fish to eat food without disturbance by water flow.
        digitalWrite(FilterPin, LOW);
      } else {
        int filterStatus = getOnOffStatus(_hour, Filter_on, Filter_off);
        digitalWrite(FilterPin, filterStatus);
      }
      delay(100);
  
      // Light program
      if (FoodDropped_at == _hour && _min < 30) {
        // Switch on light during food hour.
        digitalWrite(LightPin, HIGH);
      } else {
        int lightStatus = getOnOffStatus(_hour, Light_on, Light_off);
        digitalWrite(LightPin, lightStatus);
      }
      delay(100);

      // Third switch program
      int thirdSwitchStatus = getOnOffStatus(_hour, ThirdSwitch_on, ThirdSwitch_off);
      if (thirdSwitchStatus == 1) {
        digitalWrite(ThirdSwitchPin, thirdSwitchStatus);
        delay(100);
      }

      // Check whether main power supply should switch on.
      if (digitalRead(FilterPin) == LOW && digitalRead(LightPin) == LOW && digitalRead(ThirdSwitchPin) == LOW) {
        digitalWrite(MainPowerPin, LOW);
      } else {
        digitalWrite(MainPowerPin, HIGH);
      }
      
  
      // Additional settings
      // Delay time to sync drifted time in RTC module
      /*
      if (_hour != last_hour) {
        if (initialized) { // Ignore initial call.
          
          // Sync drifted time
          // Param: drifted second(s) in an hour.
          // Must be less than 60 seconds.
          // In TimeFunctions.ino file
          int driftedSec = getDriftedTime();
          if (driftedSec > 0) {
            syncDriftedTime(driftedSec);
            // Update RTC module time with adjusted time
            RTC.set(now());
          }
        }
        last_hour = _hour;
      }
      */
  
      initialized = true;
    }
  }
  
  wtd_cnt++;
  
  if (wtd_cnt >= wtd_cnt_reset) {
    wtd_cnt = 0;
  }
  
  system_sleep();
}
