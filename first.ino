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
static bool schedulerStarted = false;

//Task structures
Task tReadLight(LIGHT_SNSR_PERIOD_MS, TASK_FOREVER, &Task_ReadLight);
Task tReadButton(BUTTON_PERIOD_MS, TASK_FOREVER, &Task_ReadButton);
Task tBargraph(BARGRAPH_PERIOD_MS, TASK_FOREVER, &Task_Bargraph);
Task tBuzzer(RGB_BLINK_PERIOD_MS, TASK_FOREVER, &Task_Buzzer);
Task tRGBBlink(RGB_BLINK_PERIOD_MS, TASK_FOREVER, &Task_RGBBlink);

Task tHalfExposureTimeout(10000, 1, &Task_HalfExposureTimeout);
Task tAlarmTimeout(20000, 1, &Task_AlarmTimeout);
Task tCooldownTimeout(30000, 1, &Task_CooldownTimeout);


typedef enum{
    ST_IDLE = 0,
    ST_EXPOSURE,
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

            if(g_LightLevel > LIGHT_THRESHOLD){
                
                g_AppState = ST_EXPOSURE;

                //start the half exposure timeout
                tHalfExposureTimeout.restartDelayed();

                //start the alarm timeout
                tAlarmTimeout.restartDelayed();

                //reset the flags for the one shot tasks
                gb_HalfExposureReached = false;
                gb_AlarmTimeoutReached = false;
                gb_CooldownFinished = false;
                
            }

        break;

        //verify if the light level is still above the threshold, if yes, we continue with the exposure, if not,
        //we go back to idle state and disable the timeouts and the RGB LED
        //Also we check if the one shot tasks have been triggered, and we execute the corresponding actions
        case ST_EXPOSURE:
            if(g_LightLevel > LIGHT_THRESHOLD){

                if(gb_HalfExposureReached){
                    gb_HalfExposureReached = false;

                    // Activate the RGB LED task to blink the green LED
                    tRGBBlink.enable();
                    
                }

                if(gb_AlarmTimeoutReached){
                    gb_AlarmTimeoutReached = false;

                    // Activate the Buzzer task to beep the buzzer
                    tBuzzer.enable();
                    g_AppState = ST_ALARM;
                }
            }
            else{

                g_AppState = ST_IDLE;

                // Deactivate the RGB LED and the Buzzer
                tRGBBlink.disable();
                tBuzzer.disable();
                Buzzer_Off();
                RGBLed_Set(false, false, false);


                // Disable the timeouts
                tHalfExposureTimeout.disable();
                tAlarmTimeout.disable();
                tCooldownTimeout.disable();
            }
        // In the alarm state, we wait for the the button to be pressed, when it is pressed, we 
        //turn off the buzzer and the RGB LED, and we go to the cooldown state, where we wait for 30 seconds 
        //before going back to the idle state
        case ST_ALARM:
            if(gb_ButtonPressed){
                gb_ButtonPressed = false;

                // Deactivate the Buzzer and the task that controls it
                tBuzzer.disable();
                Buzzer_Off();

                // Deactivate the RGB LED and the task that controls it
                tRGBBlink.disable();
                RGBLed_Set(false, false, false);

                //Deactivate the measurement tasks, we don't need to measure the light level during the cooldown
                //and the bargraph will be turned off, so we don't need to measure the light level during the cooldown
                tReadLight.disable();
                tBargraph.disable();
                tReadButton.disable();

                BarLed_Off();

                g_AppState = ST_COOLDOWN;

                //start the cooldown timeout
                tCooldownTimeout.restartDelayed();
            }
        // In the cooldown state, we wait for the cooldown timeout to be triggered, when it is triggered, we go
        //back to the idle state
        case ST_COOLDOWN:

            if(gb_CooldownFinished){
                gb_CooldownFinished = false;        
                g_AppState = ST_IDLE;

                //reactivate the measurement tasks
                tReadLight.enable();
                tBargraph.enable();
                tReadButton.enable();
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

  runner.addTask(tHalfExposureTimeout);
  runner.addTask(tAlarmTimeout);
  runner.addTask(tCooldownTimeout);
  
  // Enable the tasks
  tReadLight.enable();
  tReadButton.enable();
  tBargraph.enable();

  tAlarmTimeout.enable();
  tHalfExposureTimeout.enable();
  tCooldownTimeout.enable();

}

void loop(){

  // Execute the scheduler
  runner.execute();

  if(!schedulerStarted){
    schedulerStarted = true;

    //initialize in false the flags for the one shot tasks, so they can be triggered when needed
    gb_HalfExposureReached = false;
    gb_AlarmTimeoutReached = false;
    gb_CooldownFinished = false;
    gb_ButtonPressed = false;

  }
  
  // Update the FSM
  FSM_Update();
}

