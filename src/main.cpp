//#include <Arduino.h>
#include "TestInsole.h"

TestInsole testinsole;
void setup() {
  Serial.begin(112500);
  testinsole.begin();
  testinsole.setup();
  testinsole.screenIntro();
}
void loop() {
  switch (testinsole.stateMH) {
    case 0:
      testinsole.display1();
      break;

    case 1:
      testinsole.display2();
      break;

    case 2:
      testinsole.display3();
      break;

    default:
      break;
  }
  //Serial.println(tt);
}