#include <Arduino.h>
#include "hardware.h"
#include <TaskScheduler.h>

/*
For this project, we will use the Lilypad USB Plus board. And the TaskScheduler library 
to manage the tasks. This library was done by Anatoli Arkhipenko
*/

//----------- configuration -------------
// some macros
#define LIGHT_THRESHOLD      700U

#define BARGRAPH_PERIOD_MS   400U
#define RGB_BLINK_PERIOD_MS  500U

#define ALARM_TIMEOUT_MS     20000UL
#define COOLDOWN_TIME_MS     30000UL

#define BUTTON_PERIOD_MS     20U
#define SENSOR_PERIOD_MS     100U

//----------- application states -------------

typedef enum
{
    ST_IDLE = 0,
    ST_EXPOSURE,
    ST_HALF_EXPOSURE,
    ST_ALARM,
    ST_COOLDOWN

  } AppState_t;

AppState_t g_State = ST_IDLE;

//----------- application variables -------------

uint16_t g_LightRaw = 0;
uint8_t  g_LightLevel = 0;

bool g_ButtonPressed = false;

bool g_AlarmActive = false;
bool g_GreenBlinkState = false;

uint32_t g_ExposureStartTime = 0;


// Scheduler things
Scheduler runner; // Create a Scheduler object that will be used to manage the tasks

//----------- function prototypes -------------

void Task_ReadLight(void);
void Task_Button(void);
void Task_Bargraph(void);
void Task_RGBBlink(void);

//=========== Task callback functions ==============
// task to read light
void Task_ReadLight(void){
    g_LightRaw = LightSensor_Read();
    Serial.println(g_LightRaw);
}
// task to detect if the button is press
void Task_ReadButton(void){
    if(digitalRead(PIN_BUTTON) == LOW){
      g_ButtonPressed = true;
      Serial.println("button pressed");
    }
}

//Task structures
Task tReadLight(SENSOR_PERIOD_MS, TASK_FOREVER, &Task_ReadLight);
Task tReadButton(BUTTON_PERIOD_MS, TASK_FOREVER, &Task_ReadButton);

//----------- function definitions -----------


void setup() {
  // Initialize the hardware pins
  LedBarPin_Init();
  RGBLedPin_Init();
  InternalLedPin_Init();  
  ButtonPin_Init();
  BuzzerPin_Init();
  LightSensorPin_Init();

  // Initialize Serial communication for debugging
  Serial.begin(9600);

  // Initialize the scheduler
  runner.init(); // Initialize the scheduler

  runner.addTask(tReadLight); // Add the light reading task to the scheduler
  runner.addTask(tReadButton);

  tReadLight.enable();
  tReadButton.enable();

}

void loop(){

  runner.execute(); // Run the scheduler to execute tasks

}

