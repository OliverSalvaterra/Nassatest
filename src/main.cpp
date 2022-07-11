#include <Arduino.h>
#include <Adafruit_INA260.h>
#include <SPI.h>
#include <SD.h>

struct Readings {
  double millivolts;
  double amps;
  double watts;
};

int dataSets = 3;
const int dataPoints = 60;
int powerThreshhold = 0;
int maxIndex = -1;

Readings readings[dataPoints];
Adafruit_INA260 ina260 = Adafruit_INA260();
int readingNum = 0;

int pos = 0;

int relayPin = 7;

const int chipSelect = 4;

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

void logReadings(int maxIndex, File f)
{
  f.println("datapoint,power,current,voltage");
  
  f.print("Maximum power data,");
    f.print(readings[maxIndex].watts);
  f.print(",");
    f.print(readings[maxIndex].amps);
  f.print(",");
    f.println(readings[maxIndex].millivolts);

  for(int i = 0; i < dataPoints; i++)
  {
    f.print(i);
    f.print(",");
      f.print(readings[maxIndex].watts);
    f.print(",");
      f.print(readings[maxIndex].amps);
    f.print(",");
      f.println(readings[maxIndex].millivolts);
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

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");

  pinMode(relayPin, OUTPUT);
}

void loop() 
{
  if(dataSets > 0)
  {
    for (int i = 0; i <= dataPoints; i++) 
    { 
      readings[readingNum].millivolts = ina260.readPower()/ina260.readCurrent();
      readings[readingNum].amps = ina260.readCurrent();
      readings[readingNum].watts = ina260.readPower();
    }

    delay(15);
  
    maxIndex = maxReadings();
    //printReadings(maxIndex);

    char* fileName = "datalog0.txt";
    fileName[7] = (char)(dataSets + 48);

    File dataFile = SD.open(fileName, FILE_WRITE);
    
    if (dataFile) {
      logReadings(maxIndex, dataFile);
      dataFile.close();
    }
    else Serial.println("error opening datalog.txt");

    dataSets--;
  }

  digitalWrite(relayPin, HIGH);
}