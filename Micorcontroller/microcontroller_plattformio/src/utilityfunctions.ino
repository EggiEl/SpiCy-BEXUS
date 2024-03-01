void printIO(uint8_t anzahl_Ios) {
  Serial.println();
  Serial.println("printio: ");
  Serial.print("Pinnumb: ");
  char *d = (char *)malloc(anzahl_Ios);
  char *a = (char *)malloc(anzahl_Ios);
  if (d == NULL) { Serial.println("Initializing of dynamic array d failed"); }
  if (a == NULL) { Serial.println("Initializing of dynamic array a failed"); }
  for (int i = 0; i < anzahl_Ios; i++) {
    pinMode(i, INPUT);
    d[i] = digitalRead(i);
    a[i] = analogRead(i);
  }
  for (int i = 0; i < anzahl_Ios; i++) {
    Serial.print(i);
    if (i < 10) {
      Serial.print(" ");
    }
    Serial.print(" |");
  }
  Serial.println();
  Serial.print("Digital: ");
  for (int i = 0; i < anzahl_Ios; i++) {
    Serial.print(d[i], BIN);
    Serial.print(" ");
    Serial.print(" |");
  }
  Serial.println();
  Serial.print("Analog:  ");
  for (int i = 0; i < anzahl_Ios; i++) {
    Serial.print(a[i], DEC);
    if (a[i] < 100) {
      Serial.print(" ");
    }
    Serial.print("|");
  }
}

void blinkLed() {
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
}

void fadeLED(){
  analogWriteRange(10000);
    for (int brightness = 0; brightness < 5000; brightness++) {
    analogWrite(LED_BUILTIN, brightness);
    delay(1);
  }
  
  // Fade out
  for (int brightness = 2500; brightness >= 0; brightness--) {
    analogWrite(LED_BUILTIN, brightness);
    delay(1);
  }
}

void heartbeat(){
    for (int i = 100; i >= 10; i -= 10) {
    analogWrite(LED_BUILTIN, 255); // Turn the LED on
    delay(i^2);                 // Wait
    analogWrite(LED_BUILTIN, 0);   // Turn the LED off
    delay(i^2);                 // Wait
  }

  // Slowly decrease the heartbeat rate
  for (int i = 10; i <= 100; i += 10) {
    analogWrite(LED_BUILTIN, 255); // Turn the LED on
    delay(i^2);                 // Wait
    analogWrite(LED_BUILTIN, 0);   // Turn the LED off
    delay(i^2);                 // Wait
  }
}