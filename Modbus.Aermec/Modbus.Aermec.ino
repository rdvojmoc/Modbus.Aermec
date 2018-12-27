#include "cmAermecHeatPumpModbus.h"

//cmAermecHeatPumpModbus coolingHeatPump(1, 8);
cmAermecHeatPumpModbus coolingHeatPump2(1, 10);

char incomingByte;
int regime = 1;

void setup()
{
  Serial.begin(115200);
  
  // use Serial (port 0); initialize Modbus communication baud rate
  //Serial2.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  Serial3.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  
  Serial.println("Starting...");
  //coolingHeatPump.begin(Serial2);
  coolingHeatPump2.MinWinterTempSP = -5;
  coolingHeatPump2.MaxWinterTempSP = 18;
  coolingHeatPump2.MinSummerTempSP = 25;
  coolingHeatPump2.MaxSummerTempSP = 45;
  coolingHeatPump2.begin(Serial3);
  Serial.println("Started.");
}

void loop()
{
  long stopwatch = millis();
  while (Serial.available()) {
    incomingByte = Serial.read();
    Serial.print("Command read: ");
    Serial.println(incomingByte);
    //Manual ON/OFF command
    switch(incomingByte)
    {
      case '1': coolingHeatPump2.ManualOn = true; break;
      case '2': coolingHeatPump2.ManualOn = false; break;
      case '3': coolingHeatPump2.ManualModeOn = true; break;
      case '4': coolingHeatPump2.ManualModeOn = false; break;
      case '5': coolingHeatPump2.ManualTempSP = 25; break;
      case '6': coolingHeatPump2.ManualTempSP = -1000; break;
      case '7': coolingHeatPump2.ManualTempSP = 32.2; break;
      case '8': coolingHeatPump2.ManualTempSP = -4.2; break;
      case '9': coolingHeatPump2.ManualTempSP = 10; break;
      
    }
    
  }
  //coolingHeatPump.process(regime);
  coolingHeatPump2.process(regime);

  //Serial.println(millis()-stopwatch);
}
