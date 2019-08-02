#include "cmAermecHeatPumpModbus.h"

cmAermecHeatPumpModbus heatPump(1, 10);

char incomingByte;
int regime = 1; // 0 = Auto, 1 = Manual, 2 = Service
long lastPrintTime = 0;

void setup()
{
  Serial.begin(115200);
  
  Serial3.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  
  Serial.println("Starting...");
  heatPump.MinWinterTempSP = -5;
  heatPump.MaxWinterTempSP = 18;
  heatPump.MinSummerTempSP = 25;
  heatPump.MaxSummerTempSP = 45;
  heatPump.begin(Serial3);
  
  Serial.println("Started.");
}

void loop()
{
  long stopwatch = millis();
  while (Serial.available()) {
    incomingByte = Serial.read();
    Serial.print("Command read: ");
    Serial.println(incomingByte);
    switch(incomingByte)
    {
      case '1': heatPump.ManualOn = true; break;
      case '2': heatPump.ManualOn = false; break;
      case '3': heatPump.ManualModeOn = true; break;
      case '4': heatPump.ManualModeOn = false; break;
      case '5': heatPump.ManualTempSP = 25; break;
      case '6': heatPump.ManualTempSP = -1000; break;
      case '7': heatPump.ManualTempSP = 32.2; break;
      case '8': heatPump.ManualTempSP = -4.2; break;
      case '9': heatPump.ManualTempSP = 10; break;
    }
  }
  
  heatPump.process(regime);

  // ***** PRINT heat pump state every 1s ***** 
  if(stopwatch - lastPrintTime > 1000){
      lastPrintTime = stopwatch;
      Serial.println("########################################");
      Serial.print("Mode: ");
      Serial.print(heatPump.ModeXS);
      Serial.print("Alarm: ");
      Serial.println(heatPump.Alarm);
      Serial.print("Alarm Remote: ");
      Serial.println(heatPump.AlarmRemote);
      Serial.print("Outside Temperature: ");
      Serial.print(heatPump.OutsideTemperature);
      Serial.println(" °C");
      Serial.print("XS: ");
      Serial.print(heatPump.XS);
      Serial.println("########################################");
  }
  
  //Serial.println(millis()-stopwatch);
}
