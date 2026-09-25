//hardware.h
//autor: Monica Carpio Erick Correa
#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

// Leds RGB
//#define PIN_LED_RED   12
#define PIN_LED_GREEN 13
//#define PIN_LED_BLUE  14

// Tira de leds blancos
#define PIN_LED_1   15
#define PIN_LED_2   16
#define PIN_LED_3   17
#define PIN_LED_4   18
#define PIN_LED_5   19
#define PIN_LED_6   20

#define BUS_BAR_LEDS 6
const uint8_t LED_BAR_PINS[BUS_BAR_LEDS] = {PIN_LED_1,PIN_LED_2,PIN_LED_3,PIN_LED_4,PIN_LED_5,PIN_LED_6};


// Actuadores
#define PIN_BUTTON A4
#define PIN_BUZZER A3 
//#define PIN_SWITCH A9 
//#define PIN_LIGHT_SENSOR A2 




#endif //HARDWARE_H