/*
    .-_---...._____
 .-((_.>      _____::::::--------------
 )   '--\--"""'
(        )
 '--.   /
  _  \ (
 (   /  '.
  '-'     \
           |
           )
  .-----'`
 (
  '-.
     \
      |
    _.'

╗   ╗                ╦
║ o ║                ║
║ ╦ ║ ╦ ╦  ╔═╗ ╔═╗ ╔═╣
║ ║ ║ ║ ║  ║ ║ ╔═╣ ║ ║
╩ ╩ ╩ ╚═╣  ╠═╝ ╚═╚ ╚═╝  ejer1
        ║  ║          
      ╚═╝  ╩          

Title: first.ino
Author: Monica Carpio Erick CS 
Date: 2026-10-4
Description: This is the main file of the project, it contains the setup and loop functions,
              and the FSM that controls the state of the system. We will use the TaskScheduler library to manage the tasks,
              and the hardware.h and task.h files to manage the hardware and the tasks respectively. Always trying to keep 
              the code as modular as possible, so we can reuse it in other projects. The FSM will have 4 states: IDLE, EXPOSURE
              ALARM and COOLDOWN. In the IDLE state, we will read the light sensor and the button, and we will update the bargraph.    
              Also we try to keep the efficiency of the code, disabling the tasks that are not needed in each state, and enabling them
              when they are needed. 
*/
#include <Arduino.h>
#include "hardware.h"
#include "task.h"
#include <TaskScheduler.h>
#include <stdint.h>
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
Task tAccelerometer(ACCELEROMETER_PERIOD_MS, TASK_FOREVER, &Task_Accelerometer);

// One shot tasks for the timeouts
Task tHalfExposureTimeout(HALF_EXPOSURE_TIMEOUT_MS, 1, &Task_HalfExposureTimeout);
Task tAlarmTimeout(ALARM_TIMEOUT_MS, 1, &Task_AlarmTimeout);
Task tCooldownTimeout(COOLDOWN_TIMEOUT_MS, 1, &Task_CooldownTimeout);

//the FSM state variable
typedef enum{
    ST_IDLE = 0, // The system is idle, waiting for the light level to be above the threshold
    ST_EXPOSURE, // The system is in exposure, the light level is above the threshold, we are measuring the light level and updating the bargraph
    ST_ALARM,    // The system is in alarm, the light level is above the threshold, we are alerting the user
    ST_COOLDOWN  // The system is in cooldown, we are waiting for the light level to return to normal

} AppState_t;

AppState_t g_AppState = ST_IDLE;

//----------- function prototypes -------------
// FSM 
void FSM_Update(void);

//----------- function definitions -----------
void FSM_Update(void){  
  // check the accelerometer X axis value, if it is above 590, and the Y axis is above 440, and the Z axis is above 610,  
  // we activate the barled task
  if(g_AccelerometerX > 460 && g_AccelerometerY > 364 && g_AccelerometerZ > 600){
    if(tBargraph.isEnabled()==false){

      tBargraph.enable();
      Serial.println("Bargraph task enabled");
    }
  }
  else{
    if(tBargraph.isEnabled()==true){

      tBargraph.disable();
      BarLed_Off();
      Serial.println("Bargraph task disabled");

    }
  }

  switch(g_AppState){

        // In the idle state, we do measurements of the light, we exit when th light level is above the threshold
        // we lunch the half exposure timeout and the alarm timeout, very usefull for the next states
        case ST_IDLE:

            //Button task is not needed in this state, we will use it only in the alarm state
            tReadButton.disable();

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
        break;
        // In the alarm state, we wait for the the button to be pressed, when it is pressed, we 
        //turn off the buzzer and the RGB LED, and we go to the cooldown state, where we wait for 30 seconds 
        //before going back to the idle state
        case ST_ALARM:
        //we need the button task enabled in this state, to detect when the button is pressed
        tReadButton.enable();

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
                //and the button task will be disabled.
                tReadLight.disable();
                tBargraph.disable();
                tReadButton.disable();

                BarLed_Off();

                g_AppState = ST_COOLDOWN;

                //start the cooldown timeout
                tCooldownTimeout.restartDelayed();
            }
        break;
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
  AccelerometerPin_Init();

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
  runner.addTask(tAccelerometer);

  runner.addTask(tHalfExposureTimeout);
  runner.addTask(tAlarmTimeout);
  runner.addTask(tCooldownTimeout);
  
  // Enable the tasks  
  tReadButton.disable();
  tBargraph.disable(); // Start with the bargraph task disabled
  tReadLight.enable();
  tAccelerometer.enable();

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

