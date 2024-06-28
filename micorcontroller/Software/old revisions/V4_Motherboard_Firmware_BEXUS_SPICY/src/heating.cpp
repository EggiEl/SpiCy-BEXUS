#include "header.h"

char heat_init = 0;

/*initializes the pins, freq and range of the heating elements*/
void heat_setup()
{
  pinMode(PIN_H0u1, OUTPUT);
  pinMode(PIN_H2u3, OUTPUT);
  pinMode(PIN_H4, OUTPUT);
  pinMode(PIN_H5, OUTPUT);
  pinMode(PIN_H6, OUTPUT);
  pinMode(PIN_H7, OUTPUT);
  digitalWrite(PIN_H0u1, 0);
  digitalWrite(PIN_H2u3, 0);
  digitalWrite(PIN_H4, 0);
  digitalWrite(PIN_H5, 0);
  digitalWrite(PIN_H6, 0);
  digitalWrite(PIN_H7, 0);
  debugf_status("<heat_setup>\n");
  analogWriteFreq(ADC_FREQ_WRITE); // 100Hz to 1MHz
  analogWriteRange(ADC_MAX_WRITE);
  heat_init = 1;
  debugf_sucess("heat_setup success\n");
}

/**
 * /updates the PWM Dutycicle of the Heaters. Will not change PWM values wich are set to 1.
 * @param unsigned int HeaterPWM[] the PWM in an array
 */
void heat_updateall(uint16_t *HeaterPWM)
{
  if (!heat_init)
  {
    void thermal_setup();
  }
  // analogWriteResolution
  heat_updateone(PIN_H0u1, HeaterPWM[0]);
  heat_updateone(PIN_H2u3, HeaterPWM[2]);
  heat_updateone(PIN_H4, HeaterPWM[4]);
  heat_updateone(PIN_H5, HeaterPWM[5]);
  heat_updateone(PIN_H6, HeaterPWM[6]);
  heat_updateone(PIN_H7, HeaterPWM[7]);
}

/**
 * Updates one Heater if the PWM value is not set to 1
 */
void heat_updateone(uint8_t PIN, uint16_t PWM)
{
  if (!heat_init)
  {
    void thermal_setup();
  }
  analogWrite(PIN, PWM);
}

/**
 * Sets all heaters on different PWM duticycles
 */
void heat_testmanual()
{
  debugf_status("<Manual Heater Test>");
  debugln("sets heater 1 to 10%, 2 to 20% and so on");
  debugf_info("first Heater Pin: %i\n", PIN_H0u1);
  if (!heat_init)
  {
    void thermal_setup();
  }
  uint16_t buf[] = {10, 20, 30, 40, 50, 60, 70, 80};
  heat_updateall(buf);
}

/*can evalue the functionality fo individual heaters via the currentdraw
@return:*/
uint8_t heat_testauto()
{
  uint16_t buf[] = {0, 0, 0, 0, 0, 0};
  heat_updateall(buf);

  uint8_t results = 0;
  unsigned long cur_alloff = get_current(); // current with all heater off

  unsigned long cur_one;
  for (uint8_t i = 0; i < 6; i++)
  {
    buf[i] = ADC_FREQ_WRITE;
    heat_updateall(buf);
    cur_one = get_current() - cur_alloff;
    if (((HEAT_CURRENT - 30) < cur_one) && (cur_one <( HEAT_CURRENT + 30)))
    {
      results |= (1 << i);
    }
    buf[i] = 0;
  }
  return results;
}
