#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_INA260.h>

struct Readings {
  int millivolts;
  int amps;
  int watts;
};

Readings readings[60];
Adafruit_INA260 ina260 = Adafruit_INA260();
Servo servo;
int readingNum = 0;
int pos = 0;
int relayPin = 7;
int servoPin = 3;

void setup() 
{
  Serial.begin(115200);

  while (!Serial) { delay(10); }

  Serial.println("Adafruit INA260 Test");

  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");

  pinMode(relayPin, OUTPUT);
  servo.attach(servoPin);
}

void loop() 
{
  for (pos = 0; pos <= 180; pos += 1) 
  { 
    if(pos % 3 == 0)
    {
      readings[readingNum].millivolts = ina260.readBusVoltage();
      readings[readingNum].amps = ina260.readCurrent();
      readings[readingNum].watts = ina260.readPower();
    }

    servo.write(pos);
    delay(15);
  }
}