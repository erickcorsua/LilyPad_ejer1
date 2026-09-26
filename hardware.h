//titulo: hardware.h
//autor: Monica Carpio Erick Correa
//descripcion: Definicion de pines de hardware para el proyecto usando 
//             Lilypad USB Plus y Arduino IDE

// hardware.h
// Autor: Monica Carpio, Erick Correa

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

/*****************************
 * RGB LED
 *****************************/

#define PIN_LED_RED    12
#define PIN_LED_GREEN  13
#define PIN_LED_BLUE   14

/*****************************
 * Barra de LEDs blancos
 ***************************/

#define PIN_LED_0      15
#define PIN_LED_1      16
#define PIN_LED_2      17
#define PIN_LED_3      18
#define PIN_LED_4      19
#define PIN_LED_5      20

#define NUM_BAR_LEDS   6

static const uint8_t LED_BAR_PINS[NUM_BAR_LEDS] =
{
    PIN_LED_0,
    PIN_LED_1,
    PIN_LED_2,
    PIN_LED_3,
    PIN_LED_4,
    PIN_LED_5
};

/*****************************
 * Comunicación
 *****************************/

#define PIN_SCL        10
#define PIN_SDA        11

#define PIN_SPI_SCK    21
#define PIN_SPI_MOSI   22
#define PIN_SPI_MISO   23

/*****************************
 * LEDs internos USB
 *****************************/

#define PIN_RX_LED     0
#define PIN_TX_LED     1

/*****************************
 * Sensores y actuadores
 *****************************/

// Botón
#define PIN_BUTTON     A4

// Zumbador (buzzer)
#define PIN_BUZZER     A3

// Posibles periféricos de la práctica
#define PIN_LIGHT_SENSOR   A2

// Si existe interruptor físico en el diseño
// #define PIN_SWITCH     A9

#endif // HARDWARE_H