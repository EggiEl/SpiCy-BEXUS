/*manages the heating elements*/
#define PIN_H0 6
#define PIN_H1 7
#define PIN_H2 8
#define PIN_H3 9
#define PIN_H4 10
#define PIN_H5 11
#define PIN_H6 12
#define PIN_H7 13

#define HEAT_FREQ 10000
#define HEAT_HUNDERTPERCENT 1000 // value where the heaters are fully turned on
#define HEAT_CURRENT 250         // aproximation of the current of a single Heater in mA

/**
 * /updates the PWM Dutycicle of the Heaters. Will not change PWM values wich are set to 1.
 * @param unsigned int HeaterPWM[] the PWM in an array
 * @return nothing
 */
void updateAllHeater(uint16_t *  HeaterPWM)
{
  debug("-updateHeater:");
  analogWriteFreq(HEAT_FREQ); // 100Hz to 1MHz
  analogWriteRange(HEAT_HUNDERTPERCENT);
  // analogWriteResolution
  updateOneHeater(PIN_H0, HeaterPWM[0]);
  updateOneHeater(PIN_H1, HeaterPWM[1]);
  updateOneHeater(PIN_H2, HeaterPWM[2]);
  updateOneHeater(PIN_H3, HeaterPWM[3]);
  updateOneHeater(PIN_H4, HeaterPWM[4]);
  updateOneHeater(PIN_H5, HeaterPWM[5]);
  debug("success-");
}

void updateOneHeater(uint8_t PIN, uint16_t PWM)
{
  if (PWM != 1)
  {
    analogWrite(PIN, PWM);
  }
}

void heaterTest_manual()
{
  uint16_t buf[] = {100, 200, 300, 400, 500, 600};
  updateAllHeater(buf);
}

/*can evalue the functionality fo individual heaters via the currentdraw*/
uint8_t heaterTest_auto()
{
  uint16_t buf[] = {0, 0, 0, 0, 0, 0};
  updateAllHeater(buf);

  uint8_t results = 0;
  unsigned long current_start = get_current(); // current with all heater off
  unsigned long current_heat;

  for (uint8_t i = 0; i < 6; i++)
  {
    buf[i] = HEAT_HUNDERTPERCENT;
    current_heat = get_current() - current_start;
    if (HEAT_CURRENT - 30 < current_heat < HEAT_CURRENT + 30)
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