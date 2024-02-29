#define PIN_H0 6
#define PIN_H1 7
#define PIN_H2 8
#define PIN_H3 9
#define PIN_H4 10
#define PIN_H5 11
#define PIN_H6 12
#define PIN_H7 13



/**
 * /updates the PWM Dutycicle of the Heaters
 * @param unsigned int HeaterPWM[] the PWM in an array
 * @return nothing
 */
void updateHeater(uint16_t HeaterPWM[]) {
  analogWriteFreq(10000);  //100Hz to 1MHz
  analogWriteRange(1000);
  //analogWriteResolution
  for (int i = PIN_H0; i < PIN_H7; i++) {
    analogWrite(i, HeaterPWM[i - PIN_H0]);
  }
}

void testHEater() {
  uint16_t a[] = { 100, 200, 300, 400, 500, 600, 700, 800};
  updateHeater(a);
}

void updatePID(){
  // float p,i,d;
}