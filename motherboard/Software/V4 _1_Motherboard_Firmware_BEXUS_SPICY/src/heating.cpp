#include "header.h"
#include "debug_in_color.h"

volatile char heat_init = 0;

/*initializes the pins, freq and range of the heating elements*/
void heat_setup()
{
  pinMode(PIN_H0, OUTPUT);
  pinMode(PIN_H1, OUTPUT);
  pinMode(PIN_H2, OUTPUT);
  pinMode(PIN_H3, OUTPUT);
  pinMode(PIN_H4, OUTPUT);
  pinMode(PIN_H5, OUTPUT);
  pinMode(PIN_H6, OUTPUT);
  pinMode(PIN_H7, OUTPUT);
  digitalWrite(PIN_H0, 0);
  digitalWrite(PIN_H1, 0);
  digitalWrite(PIN_H2, 0);
  digitalWrite(PIN_H3, 0);
  digitalWrite(PIN_H4, 0);
  digitalWrite(PIN_H5, 0);
  digitalWrite(PIN_H6, 0);
  digitalWrite(PIN_H7, 0);
  debugf_status("<heat_setup>\n");
  analogWriteFreq(ADC_FREQ_WRITE);
  analogWriteRange(ADC_MAX_WRITE);
  heat_init = 1;
  debugf_sucess("heat_setup success\n");
}

/**
 * /updates the PWM Dutycicle of the Heaters. Will not change PWM values wich are set to 1.
 * @param unsigned int HeaterPWM[] the PWM in an array size of 8
 */
void heat_updateall(const float HeaterPWM[])
{
  if (!heat_init)
  {
     heat_setup();
  }
  // analogWriteResolution
  heat_updateone(PIN_H0, HeaterPWM[0]);
  heat_updateone(PIN_H1, HeaterPWM[1]);
  heat_updateone(PIN_H2, HeaterPWM[2]);
  heat_updateone(PIN_H3, HeaterPWM[3]);
  heat_updateone(PIN_H4, HeaterPWM[4]);
  heat_updateone(PIN_H5, HeaterPWM[5]);
  heat_updateone(PIN_H6, HeaterPWM[6]);
  heat_updateone(PIN_H7, HeaterPWM[7]);
}

/**
 * Updates one Heater
 * @param duty is the duty cicyle in percent 
 */
void heat_updateone(const uint8_t PIN, const float duty)
{
  if (!heat_init)
  {
    heat_setup();
  }
  analogWrite(PIN, duty*0.01*ADC_MAX_WRITE);
}

/**
 * Sets all heaters on different PWM duticycles
 */
void heat_testmanual()
{
  debugf_status("<Manual Heater Test>");
  debugln("sets heater 1 to 10%, 2 to 20% and so on");
  debugf_info("first Heater Pin: %i\n", PIN_H0);
  if (!heat_init)
  {
     heat_setup();
  }
  float buf[] = {10, 20, 30, 40, 50, 60, 70, 80};
  heat_updateall(buf);
}

