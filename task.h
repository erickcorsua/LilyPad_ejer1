#ifndef TASK_H
#define TASK_H

#include <stdint.h>

//MACROS
//macros for the light sensor
#define LIGHT_THRESHOLD      70U
#define LIGHT_MIN  0U
#define LIGHT_MAX  1003U

//periods for the tasks
#define LIGHT_SNSR_PERIOD_MS     100U
#define BARGRAPH_PERIOD_MS   400U
#define RGB_BLINK_PERIOD_MS  500U
#define BUZZER_PERIOD_MS     500U
#define BUTTON_PERIOD_MS     20U


// Variables globales compartidas
extern uint16_t g_LightRaw;
extern uint8_t  g_LightLevel;
extern uint8_t  g_BarLevel;
extern uint8_t  g_LedIndex;

extern bool gb_ButtonPressed;

extern bool gb_HalfExposureReached;
extern bool gb_AlarmTimeoutReached;
extern bool gb_CooldownFinished;

//----------- function prototypes -------------

void Task_ReadLight(void);
void Task_ReadButton(void);
void Task_Bargraph(void);
void Task_RGBBlink(void);
void Task_Buzzer(void);

void Task_HalfExposureTimeout(void);
void Task_AlarmTimeout(void);
void Task_CooldownTimeout(void);
#endif
