//title: hardware.cpp
// Autor: Monica Carpio, Erick Correa
// Date: 2026-10-4
// Description: This file contains the implementation of the hardware functions used in the project. 

#include <stdint.h>
#include <Arduino.h>
#include "hardware.h"

//================variable definitions ================
bool gb_LedBarPin_Init       =false;
bool gb_RGBLedPin_Init       =false;
bool gb_CommunicationPin_Init=false;
bool gb_InternalLedPin_Init  =false;
bool gb_ButtonPin_Init       =false;
bool gb_BuzzerPin_Init       =false;
bool gb_LightSensorPin_Init  =false;
bool gb_AccelerometerPin_Init=false;
//================ function prototypes ================
// initialize the hardware pins
void LedBarPin_Init(void);
void RGBLedPin_Init(void);
void CommunicationPin_Init(void);
void InternalLedPin_Init(void);
void ButtonPin_Init(void);
void BuzzerPin_Init(void);
void LightSensorPin_Init(void);
void AccelerometerPin_Init(void);

//------LedBar functions------
void BarLed_Set(uint8_t ledIndex, bool continuous);
void BarLed_Off(void);
//------RGB functions---------
void RGBLed_Set(bool red, bool green, bool blue);
//---LightSensor functions----
uint16_t LightSensor_Read(void);
//---Buzzer functions---------
void Buzzer_On(uint16_t freq);
void Buzzer_Off(void);
//---Accelerometer functions---
void Accelerometer_Read(int16_t* x, int16_t* y, int16_t* z);



//================ function definitions ================

void LedBarPin_Init(void){

    if(!gb_LedBarPin_Init){
        for(uint8_t i=0; i<NUM_BAR_LEDS; i++){
            pinMode(LED_BAR_PINS[i], OUTPUT);
        }
        gb_LedBarPin_Init = true;
    }
    
}
//-------------------------------
void RGBLedPin_Init(void){

    if(!gb_RGBLedPin_Init){
        pinMode(PIN_LED_RED, OUTPUT);
        pinMode(PIN_LED_GREEN, OUTPUT);
        pinMode(PIN_LED_BLUE, OUTPUT);
        gb_RGBLedPin_Init = true;
    }
    
}
//-------------------------------
void CommunicationPin_Init(void){ // only for I2C and SPI communication manually, not for the Arduino Wire library

    if(!gb_CommunicationPin_Init){
        pinMode(PIN_SCL, OUTPUT);
        pinMode(PIN_SDA, OUTPUT);
        pinMode(PIN_SPI_SCK, OUTPUT);
        pinMode(PIN_SPI_MOSI, OUTPUT);
        pinMode(PIN_SPI_MISO, INPUT);
        gb_CommunicationPin_Init = true;
    }
    
}    
//-------------------------------
void InternalLedPin_Init(void){

    if(!gb_InternalLedPin_Init){
        pinMode(PIN_RX_LED, OUTPUT);
        pinMode(PIN_TX_LED, OUTPUT);
        gb_InternalLedPin_Init = true;
    }
    
}
//-------------------------------
void ButtonPin_Init(void){

    if(!gb_ButtonPin_Init){
        pinMode(PIN_BUTTON, INPUT_PULLUP);
        gb_ButtonPin_Init = true;
    }
    
}
//-------------------------------
void BuzzerPin_Init(void){

    if(!gb_BuzzerPin_Init){
        pinMode(PIN_BUZZER, OUTPUT);
        gb_BuzzerPin_Init = true;
    }
    
}
//-------------------------------
void LightSensorPin_Init(void){

    if(!gb_LightSensorPin_Init){
        pinMode(PIN_LIGHT_SENSOR, INPUT);
        gb_LightSensorPin_Init = true;
    }

}
//-------------------------------
void AccelerometerPin_Init(void){

    if(!gb_AccelerometerPin_Init){

        pinMode(PIN_ACCELEROMETER_X, INPUT);
        pinMode(PIN_ACCELEROMETER_Y, INPUT);
        pinMode(PIN_ACCELEROMETER_Z, INPUT);
        
        gb_AccelerometerPin_Init = true;
    }

}

//some other functions can be added here for the hardware,
//like reading the button state, reading the light sensor value, etc.

/*
This fucntions turn on a specific LED in the LED bar, it could be only one LED or 
all the bar until the ledIndex, depending on the continuous parameter. If continuous is true, 
all the LEDs from 0 to ledIndex will be turned on, if continuous is false, only the LED at ledIndex will be turned on.
And if ledIndex is 0, all the LEDs will be turned off.
*/
void BarLed_Set(uint8_t level, bool continuous)
{
    BarLed_Off();

    if(level == 0){ 
        return;
        BarLed_Off();
    }

    if(level > NUM_BAR_LEDS){
        level = NUM_BAR_LEDS;
    }

    if(continuous){
        for(uint8_t i = 0; i < level; i++){
            digitalWrite(LED_BAR_PINS[i], HIGH);
        }
    }
    else{
        digitalWrite(LED_BAR_PINS[level - 1], HIGH);
    }
}
/*
 * This function turns off all the LEDs in the LED bar.
 */
void BarLed_Off(void){

    for(uint8_t i=0; i<NUM_BAR_LEDS; i++){
        digitalWrite(LED_BAR_PINS[i], LOW);
    }
}

/*
This function sets the color of the RGB LED.
*/
void RGBLed_Set(bool red, bool green, bool blue){
    digitalWrite(PIN_LED_RED, red ? HIGH : LOW);
    digitalWrite(PIN_LED_GREEN, green ? HIGH : LOW);
    digitalWrite(PIN_LED_BLUE, blue ? HIGH : LOW);
}
/*
This function reads the value from the light sensor and returns it as an integer.
*/    
uint16_t LightSensor_Read(void){
    return analogRead(PIN_LIGHT_SENSOR);
}
/*
This function turns on the buzzer at a specific frequency.
*/
void Buzzer_On(uint16_t freq){
    tone(PIN_BUZZER, freq);
}
/**
  This function turns off the buzzer.
*/
void Buzzer_Off(void){
    noTone(PIN_BUZZER);
}
/*
    This functions reads the values from the accelerometer and prints them to the Serial monitor.
    The return is a uint16_t array with the values of the X, Y and Z axes.
*/
void Accelerometer_Read(int16_t* x, int16_t* y, int16_t* z){
    *x = analogRead(PIN_ACCELEROMETER_X);
    *y = analogRead(PIN_ACCELEROMETER_Y);
    *z = analogRead(PIN_ACCELEROMETER_Z);
}

/*
       _                        
       \`*-.                    
        )  _`-.                 
       .  : `. .                
       : _   '  \               
       ; *` _.   `*-._          
       `-.-'          `-.       
         ;       `       `.     
         :.       .        \    
         . \  .   :   .-'   .   
         '  `+.;  ;  '      :   
         :  '  |    ;       ;-. 
         ; '   : :`-:     _.`* ;
[ideas].*' /  .*' ; .*`- +'  `*' 
      `*-*   `*-*  `*-*'

*/