#include "header.h"
char heat_init = 0;
/*initializes the pins, freq and range of the heating elements*/
void heat_initialize()
{
  analogWriteFreq(HEAT_FREQ); // 100Hz to 1MHz
  analogWriteRange(HEAT_HUNDERTPERCENT);
  pinMode(PIN_H0, OUTPUT);
  pinMode(PIN_H1, OUTPUT);
  pinMode(PIN_H2, OUTPUT);
  pinMode(PIN_H3, OUTPUT);
  pinMode(PIN_H4, OUTPUT);
  pinMode(PIN_H5, OUTPUT);
  heat_init = 1;
}

/**
 * /updates the PWM Dutycicle of the Heaters. Will not change PWM values wich are set to 1.
 * @param unsigned int HeaterPWM[] the PWM in an array
 */
void heat_updateall(uint16_t *HeaterPWM)
{
  if (!heat_init)
  {
    heat_initialize();
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
 * Updates one Heater if the PWM value is not set to 1
 */
void heat_updateone(uint8_t PIN, uint16_t PWM)
{
  if (!heat_init)
  {
    heat_initialize();
  }

  if (PWM != 1)
  {
    analogWrite(PIN, PWM);
  }
}

/**
 * Sets all heaters on different PWM duticycles
 */
void heat_testmanual()
{
  debugln("<Manual Heater Test>");
  debugln("sets heater 1 to 10%, 2 to 20% and so on");
  debug("first Heater Pin: ");
  debugln(PIN_H0);
  if (!heat_init)
  {
    heat_initialize();
  }
  uint16_t buf[] = {10, 20, 30, 40, 50, 60,70,80};
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
    buf[i] = HEAT_HUNDERTPERCENT;
    heat_updateall(buf);
    cur_one = get_current() - cur_alloff;
    if (HEAT_CURRENT - 30 < cur_one < HEAT_CURRENT + 30)
    {
      results |= (1 << i);
    }
    buf[i] = 0;
  }
  return results;
}

void updatePID()
{
  // float p,i,d;
}