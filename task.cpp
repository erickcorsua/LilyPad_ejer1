#include "task.h"
#include <Arduino.h>
#include "hardware.h"
#include <stdint.h> 

//MACROS
//macros for the light sensor
#define LIGHT_THRESHOLD      700U
#define LIGHT_MIN  0U
#define LIGHT_MAX  1003U

//periods for the tasks
#define LIGHT_SNSR_PERIOD_MS     100U
#define BARGRAPH_PERIOD_MS   400U
#define RGB_BLINK_PERIOD_MS  500U
#define BUZZER_PERIOD_MS     500U
#define BUTTON_PERIOD_MS     20U

//==============global variables ==============
uint16_t g_LightRaw = 0;
uint8_t  g_LightLevel = 0;
uint8_t g_BarLevel = 0;

bool gb_ButtonPressed = false;

bool gb_HalfExposureReached = false;
bool gb_AlarmTimeoutReached = false;
bool gb_CooldownFinished = false;

//----------- function prototypes -------------

void Task_ReadLight(void);
void Task_ReadButton(void);
void Task_Bargraph(void);
void Task_RGBBlink(void);
void Task_Buzzer(void);


//=========== Task callback functions ==============
/**
 * This task will read the light sensor and update the global light level variable.
 */
void Task_ReadLight(void){

    g_LightRaw = LightSensor_Read();
    
    // Callibrate the light level to a percentage (0-100)
    g_LightLevel = map(g_LightRaw, LIGHT_MIN, LIGHT_MAX, 0, 100);

    //Avoid values outside the range of 0-100
    g_LightLevel = constrain(g_LightLevel, 0, 100);

    Serial.print(" LEVEL=");
    Serial.println(g_LightLevel);
}

/**
 * This task will read the button and update the global button press variable.
 */
void Task_ReadButton(void){

    static uint8_t stable_counter = 0;

    if(digitalRead(PIN_BUTTON) == LOW){
      
        if(stable_counter < 6){
            stable_counter++;
        }

        if(stable_counter == 6){
            gb_ButtonPressed = true;
        }
    }
    else{
        stable_counter = 0;
    }
}
/*
  This task will set the bargraph based on the light level. Only we have
  6 LEDs in the bargraph, so the max value is 6 and the min value is 0
*/
void Task_Bargraph(void){
    
    BarLed_Off();

    g_BarLevel = (g_LightLevel + 15) / 16;

    if(g_BarLevel > NUM_BAR_LEDS){
        g_BarLevel = NUM_BAR_LEDS;
    }

    BarLed_Set(g_BarLevel, true);
}
/*
 This task will make the RGB green LED blink with a 500 ms period
*/
void Task_RGBBlink(void){

  static bool ledState = false;

  ledState = !ledState;

  RGBLed_Set(false, ledState, false);
}
/*
 This task will make the Buzzer beep with a 500 ms period
*/
void Task_Buzzer(void){

  static bool buzzerState = false;

  buzzerState = !buzzerState;

  if(buzzerState){
    Buzzer_On(1000);
  }
  else{
    Buzzer_Off();
  }
}

// ============ One shot tasks ==============

/**
 * Triggered after 10 seconds of exposure.
 */
void Task_HalfExposureTimeout(void){
    gb_HalfExposureReached = true;

    Serial.println("HALF EXPOSURE TIMEOUT");
}
/**
 * Triggered after 20 seconds of exposure.
 */
void Task_AlarmTimeout(void){
    gb_AlarmTimeoutReached = true;

    Serial.println("ALARM TIMEOUT");
}
/**
 * Triggered after 30 seconds of cooldown.
 */
void Task_CooldownTimeout(void){
    gb_CooldownFinished = true;

    Serial.println("COOLDOWN TIMEOUT");
}