// VIBRATION MOLE REPELLENT ...thingy
// Created by daninet - 2024
// More info and guide on the github page: https://github.com/daninet/vibration_mole_repellent


//                       ██████████████                                                
//                   ████░░░░░░░░░░░░░░██                                              
//                 ██░░░░░░░░░░░░░░░░░░░░████                                          
//             ████░░░░░░░░░░░░░░░░░░░░░░░░░░████████████████████                      
//       ██████░░░░░░░░░░░░████░░░░░░░░░░░░░░██░░░░░░░░░░░░░░░░░░██                    
//   ██████░░░░░░░░░░░░░░██    ██░░░░░░░░░░░░░░██░░░░░░░░░░░░░░░░░░██                  
// ██▓▓▓▓▓▓██░░░░░░░░░░░░██    ██░░░░░░░░░░░░░░░░██░░░░░░░░░░░░░░░░░░██                
// ██▓▓▓▓▓▓▓▓██░░░░░░░░░░░░████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██              
//   ██▓▓▓▓██░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██            
//     ████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██            
//       ██░░░░░░░░░░░░░░████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██            
//         ████░░░░░░████░░░░░░░░░░░░██░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██████        
//       ██░░░░██████░░░░░░░░░░░░░░░░░░██░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░██    ██      
//     ██░░░░██░░░░░░░░░░░░░░░░░░░░░░░░██░░░░░░░░████████████████░░░░░░░░████    ██    
//     ██████░░░░░░░░░░░░░░░░░░░░░░░░░░██░░░░░░██░░░░░░░░░░░░░░░░██░░░░░░██  ████      
//           ████  ██░░░░██░░░░░░░░░░░░██░░░░██░░░░░░░░░░░░░░░░░░░░██████              
//               ██░░░░██░░░░░░██░░░░██████████░░░░██░░░░░░██░░░░░░██                  
//             ██░░░░██░░░░░░██░░░░██        ██░░██░░░░░░██░░░░░░██                    
//           ██░░░░██░░░░████░░░░██            ██░░░░████░░░░░░██                      
//             ██████████  ██████                ████  ████████                        

//                      REPEL THE MOLES, DON'T KILL THEM                                                                                        

#include <driver/rtc_io.h>
#include <esp_sleep.h>
#include "driver/ledc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc.h"

//==========================================
// SETTINGS

// SETUP YOUR PINS

// Define the GPIO pin for the mosfets switching the motor
const int mosfetPin = 3; 

// Define the Analog pin for the voltage sensor
const int voltagePin = 0; 

// Define the voltage threshold where the device will save a bit of power by vibrating less.
// For when you have long cloudy autumns and the solar panel is not making enough power.
const float voltageThreshold = 10.8;

// Short buzz length
const int shortBuzz = 1000; //ms

// Long buzz length
const int longBuzz = 3000; //ms

// Define the voltage divider resistor values (R1 and R2 in ohms)
// If you have used just two resistors you already know.
// If you purchased a sensor module chances are the below is correct
const float R1 = 30000.0;  // 30k ohms
const float R2 = 7500.0;  // 7.5k ohms

//ESPs and Arduinos measure voltage rather inaccurate, with up-to 10% error.
//Check the voltage in test mode and compensate here if it is off.
float actualVoltage1 = 10.0; 
float measuredVoltage1 = 10.86;  // Replace with your measured voltage
float actualVoltage2 = 12.6;
float measuredVoltage2 = 13.83;  // Replace with your measured voltage

// Boolean to enable/disable serial debug
bool enableDebug = true;

// Test mode - in testmode the ESP will not go into deep sleep so you can read the serial messages.
// In test mode the timeout is 1/10th of the real timeout, for example instead of 36sec it is 3.6sec
bool testMode = false;

// Motor max speed - Define the max speed of the motor on the range of 0-8191
// 8191 is 100% duty cycle for 13-bit resolution
// With some motors slower vibration is more effective as high speed makes it smoother

const int maxSpeed = 6000;

// END OF SETTINGS
//=========================================

// Motor PWM settings, ideally don't change it
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          mosfetPin // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               maxSpeed // Set duty cycle
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

void setup() {
  if (enableDebug) {
    Serial.begin(9600);  // Initialize serial communication for debugging
  }
  
  // Disable deep sleep hold on the MOSFET pin if previously enabled
  gpio_hold_dis(static_cast<gpio_num_t>(mosfetPin));
  gpio_deep_sleep_hold_dis();

  // Initialize the MOSFET pin
  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, LOW);  // Ensure the MOSFET is off initially

  // Initialize the voltage pin
  pinMode(voltagePin, INPUT);

  // Seed the random number generator with a unique value
  randomSeed(esp_random());
}

// LEDC config to create PWM
void ledc_init() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void setMosfet(bool state) {
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, state ? LEDC_DUTY : 0));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

//Setup voltage read compensation
float calculateCalibrationFactor(float actualVoltage1, float measuredVoltage1, float actualVoltage2, float measuredVoltage2) {
  float factor1 = actualVoltage1 / measuredVoltage1;
  float factor2 = actualVoltage2 / measuredVoltage2;
  float calibrationFactor = (factor1 + factor2) / 2;
  return calibrationFactor;
}

float readVoltage(float calibrationFactor) {
  int analogValue = analogRead(voltagePin);
  float voltage = analogValue * (3.3 / 4095.0);  // Convert analog reading to voltage
  float inputVoltage = voltage * ((R1 + R2) / R2);  // Calculate the input voltage
  float calibratedVoltage = inputVoltage * calibrationFactor;  // Apply calibration factor
  return calibratedVoltage;
}

// Generate a random time between 30 and 60 seconds
uint32_t sleepTime = 30 + esp_random() % 31;  // Random time between 30 and 60 seconds

//Main sleep fuction
void goToSleep() {
  if (enableDebug) {
    Serial.println("Going to sleep for: ");
    Serial.print(sleepTime);
    Serial.print(" seconds.");
    }

  // Ensure mosfet is off
  setMosfet(false);
  // Enable deep sleep hold on the MOSFET pin
  gpio_hold_en(static_cast<gpio_num_t>(mosfetPin));
  gpio_deep_sleep_hold_en();
  

  if (testMode == true) {
    //  In sleep mode we will just wait for a shorter timeout
    delay(sleepTime * 100);
  } else {
    // Convert sleep time to microseconds and set up deep sleep
    esp_sleep_enable_timer_wakeup(sleepTime * 1000000);
    esp_deep_sleep_start();
    }
}

//Main loop
void loop() {
  //Calibrate voltage read based on preset
  float calibrationFactor = calculateCalibrationFactor(actualVoltage1, measuredVoltage1, actualVoltage2, measuredVoltage2);
  float voltage = readVoltage(calibrationFactor);
    if (enableDebug) {
  Serial.println("Calibration Factor: " + String(calibrationFactor));
  Serial.println("Voltage: " + String(voltage));
    }

  //Initialize ledc for PWM
  ledc_init();

  //Patterns here, change to your liking
  if (voltage < voltageThreshold) {
    //LOW VOLTAGE PATTERN
      if (enableDebug) {
        Serial.println("Voltage low - Short buzz");
      }
    for (int i = 0; i < 2; i++) {
      setMosfet(true);
      delay(shortBuzz);
      setMosfet(false);
      delay(shortBuzz);
    }
  } else {
    // NORMAL VOLTAGE PATTERN
      if (enableDebug) {
        Serial.println("Voltage OK - Normal buzz sequence.");
      }
    for (int i = 0; i < 3; i++) {
      setMosfet(true);
      delay(shortBuzz);
      setMosfet(false);
      delay(shortBuzz);
    }

    setMosfet(true);
    delay(longBuzz);
    setMosfet(false);
  }
  // Initiate deep sleep
  goToSleep();
}