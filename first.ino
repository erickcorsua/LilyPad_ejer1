#include "hardware.h"
//#include <Arduino.h>
#include <stdint.h>

void setup() {
  // put your setup code here, to run once:


  //Configurar barra de leds blancos salida y led green
  for(int i = 0; i<7; i++){

    pinMode(LED_BAR_PINS[i],OUTPUT);
    digitalWrite(LED_BAR_PINS[i], LOW);

  }

  pinMode(PIN_LED_GREEN,OUTPUT);
  digitalWrite(PIN_LED_GREEN, LOW);

  //Configuracion button
  pinMode(PIN_BUTTON,INPUT_PULLUP);
  

  //Configuracion buzzer
  pinMode(PIN_BUZZER,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<7; i++){
    digitalWrite(LED_BAR_PINS[i],HIGH);    
    delay(1000);
    digitalWrite(LED_BAR_PINS[i],LOW);    
  }

}
