#include <Arduino.h>
#include "hardware.h"
#include "task.h"
#include <TaskScheduler.h>

/*
For this project, we will use the Lilypad USB Plus board. And the TaskScheduler library 
to manage the tasks. This library was done by Anatoli Arkhipenko
*/
//================global variables ==============
// Define the scheduler
Scheduler runner;

//Task structures
Task tReadLight(LIGHT_SNSR_PERIOD_MS, TASK_FOREVER, &Task_ReadLight);
Task tReadButton(BUTTON_PERIOD_MS, TASK_FOREVER, &Task_ReadButton);
Task tBargraph(BARGRAPH_PERIOD_MS, TASK_FOREVER, &Task_Bargraph);
Task tBuzzer(RGB_BLINK_PERIOD_MS, TASK_FOREVER, &Task_Buzzer);
Task tRGBBlink(RGB_BLINK_PERIOD_MS, TASK_FOREVER, &Task_RGBBlink);


typedef enum{
    ST_IDLE = 0,
    ST_HALF_EXPOSURE,
    ST_ALARM,
    ST_COOLDOWN

} AppState_t;

AppState_t g_AppState = ST_IDLE;

uint32_t g_ExposureStartTime = 0;
uint32_t g_CooldownStartTime = 0;

bool gb_HalfExposureIndicated = false;

//----------- function prototypes -------------
// FSM 
void FSM_Update(void);

//----------- function definitions -----------
void FSM_Update(void){

  switch(g_AppState){
    // In the idle state, we do measurements of the light, we exit when th light level is above the threshold
    // we lunch the half exposure timeout and the alarm timeout, very usefull for the next states
    case ST_IDLE:

    if(g_LightLevel > LIGHT_THRESHOLD)
    {
        g_AppState = ST_HALF_EXPOSURE;

        g_ExposureStartTime = millis();

        gb_HalfExposureIndicated = false;

        tRGBBlink.disable();
        RGBLed_Set(false, false, false);
    }

break;
    //verify if the light level is still above the threshold, if yes, we continue with the exposure, if not,
    //we go back to idle state and disable the timeouts and the RGB LED
    case ST_HALF_EXPOSURE:

    if(g_LightLevel <= LIGHT_THRESHOLD)
    {
        g_AppState = ST_IDLE;

        tRGBBlink.disable();
        RGBLed_Set(false, false, false);
    }
    else
    {
        uint32_t elapsedTime = millis() - g_ExposureStartTime;

        if((elapsedTime >= 10000) && (!gb_HalfExposureIndicated))
        {
            gb_HalfExposureIndicated = true;

            tRGBBlink.enable();
        }

        if(elapsedTime >= 20000)
        {
            tRGBBlink.disable();
            RGBLed_Set(false, false, false);

            tBuzzer.enable();

            g_AppState = ST_ALARM;
        }
    }

break;

case ST_ALARM:

    if(gb_ButtonPressed)
    {
        gb_ButtonPressed = false;

        tBuzzer.disable();
        Buzzer_Off();

        BarLed_Off();

        tReadLight.disable();
        tBargraph.disable();

        g_CooldownStartTime = millis();

        g_AppState = ST_COOLDOWN;
    }

break;
case ST_COOLDOWN:

    if((millis() - g_CooldownStartTime) >= 30000)
    {
        tReadLight.enable();
        tBargraph.enable();

        g_AppState = ST_IDLE;
    }

break;


    default:
      g_AppState = ST_IDLE;
      break;
  }
}



//----------------ARDUINO SETUP AND LOOP----------------
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

  // Add the tasks to the scheduler
  runner.addTask(tReadLight);
  runner.addTask(tReadButton);
  runner.addTask(tBargraph);
  runner.addTask(tBuzzer);
  runner.addTask(tRGBBlink);

  // Enable the tasks
  tReadLight.enable();
  tReadButton.enable();
  tBargraph.enable();

}

void loop(){

  // Execute the scheduler
  runner.execute();

  // Update the FSM
  FSM_Update();
}

