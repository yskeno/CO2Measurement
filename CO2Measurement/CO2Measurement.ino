/* 
 CO2 Measurement.ino
 Use mh-z19b as CO2 sensor.
 https://www.winsen-sensor.com/d/files/infrared-gas-sensor/mh-z19b-co2-ver1_0.pdf
 */

#define PIN_PWM 3

// 0 is as LOW, other values are as HIGH
int curValue = LOW;
int prevValue = LOW;

long highLevelStart = 0L;
long highLevelEnd = 0L;
long highLevelLength = 0L;
long lowLevelStart = 0L;
long lowLevelEnd = 0L;
long lowLevelLength = 0L;

// Get current time as milli seconds
long curMilliSec = 0L;

long CO2ppm = 0L;
long CO2Max = 0L;
long CO2Mean = 0L;

void setup()
{
  Serial.begin(9600);
  pinMode(PIN_PWM, INPUT);
  prevValue = digitalRead(PIN_PWM);
}

void loop()
{
  curMilliSec = millis();
  curValue = digitalRead(PIN_PWM);

  // CO2 level: Cppm = 2000 × (TH - 2ms) / (TH + TL - 4ms)
  // Cppm: CO2 level which calculated by PWM output
  // TH: high level output time during cycle
  // TL: low level output time during cycle
  // 1 cycle = 1004ms +-5%
  // Timing of "LOW to HIGH" divide cycles.
  if (curValue == HIGH)
  {
    if (prevValue == LOW) // prevValue:LOW, curValue:HIGH => LOW to HIGH
    {
      highLevelLength = highLevelEnd - highLevelStart;
      lowLevelLength = lowLevelEnd - lowLevelStart;
      CO2ppm = 2000 * (highLevelLength - 2) / (highLevelLength + lowLevelLength - 4);

      // Keep max CO2ppm value
      if (CO2ppm > CO2Max)
      {
        CO2Max = CO2ppm;
      }

      // Calculate CO2ppm mean.
      CO2Mean = (CO2Mean + CO2ppm) / 2;

      Serial.println("CO2: " + String(CO2ppm) + " ppm\n" + "Max: " + String(CO2Max) + " Ave: " + String(CO2Mean));

      highLevelStart = curMilliSec;
      prevValue = HIGH;
    }
    else // prevValue:HIGH, curValue:HIGH
    {
      highLevelEnd = curMilliSec;
    }
  }
  else
  {
    if (prevValue == HIGH) // prevValue:HIGH, curValue:LOW => HIGH to LOW
    {
      lowLevelStart = curMilliSec;
      prevValue = LOW;
    }
    else // prevValue:LOW, curValue:LOW
    {
      lowLevelEnd = curMilliSec;
    }
  }
}