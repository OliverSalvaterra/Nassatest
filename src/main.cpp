#include <Arduino.h>
#include <Servo.h>
#include <Adafruit_INA260.h>

struct Readings {
  int millivolts;
  int amps;
  int watts;
};

int dataSets = 3;
const int dataPoints = 60;
int powerThreshhold = 0;

Readings readings[dataPoints];
Adafruit_INA260 ina260 = Adafruit_INA260();
Servo servo;
int readingNum = 0;

int pos = 0;

int relayPin = 7;
int servoPin = 3;

int maxReadings() // returns datapoint number for max power
{
  int maxPowerIndex;

  for(int i = 0; i < dataPoints; i++)
  {
    if(readings[i].watts > readings[maxPowerIndex].watts)
    {
      maxPowerIndex = i;
    }
  }

  return maxPowerIndex;
}

void printReadings(int maxIndex)
{
  Serial.println("datapoint,power,current,voltage");
  
  Serial.print("Maximum power data,");
    Serial.print(readings[maxIndex].watts);
  Serial.print(",");
    Serial.print(readings[maxIndex].amps);
  Serial.print(",");
    Serial.println(readings[maxIndex].millivolts);

  for(int i = 0; i < dataPoints; i++)
  {
    Serial.print(i);
    Serial.print(",");
      Serial.print(readings[maxIndex].watts);
    Serial.print(",");
      Serial.print(readings[maxIndex].amps);
    Serial.print(",");
      Serial.println(readings[maxIndex].millivolts);
  }
}


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
  servo.write(0);

  if(ina260.readPower() >= powerThreshhold && dataSets > 0)
  {
    for (pos = 0; pos <= 180; pos += 1) 
    { 
      if(pos % ((int)(180/dataPoints) + 1) == 0)
      {
        readings[readingNum].millivolts = ina260.readPower()/ina260.readCurrent();
        readings[readingNum].amps = ina260.readCurrent();
        readings[readingNum].watts = ina260.readPower();
      }

      servo.write(pos);
      delay(15);
    } 
  
    int maxIndex = maxReadings();
    int maxDegrees = (int)((180/dataPoints) + 1) * maxIndex;
    for (pos = 180; pos >= maxDegrees; pos -= 1) 
    {
      servo.write(pos);
      delay(15);
    }

    printReadings(maxIndex);

    dataSets--;
  }

  if(dataSets <= 0)
  {
    digitalWrite(relayPin, HIGH);
  }
}