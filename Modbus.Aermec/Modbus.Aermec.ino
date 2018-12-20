#include "cmAermecHeatPumpModbus.h"

cmAermecHeatPumpModbus coolingHeatPump(1, 8);
cmAermecHeatPumpModbus coolingHeatPump2(1, 10);

void setup()
{
  Serial.begin(115200);
  
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial2.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  Serial3.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  
  Serial.println("Starting...");
  coolingHeatPump.begin(Serial2);
  coolingHeatPump2.begin(Serial3);
  Serial.println("Started.");
}

void loop()
{
  Serial.println("Looping");
  long stopwatch = millis();
  
  coolingHeatPump.proccess();
  coolingHeatPump2.proccess();

  Serial.println(millis()-stopwatch);
}
