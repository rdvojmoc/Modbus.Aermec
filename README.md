# Modbus.Aermec
Modbus MOD485K (Modaer) implementation for Aermec heat pumps on Arduino. Dependant on ModbusMaster library.

Implementation of Modbus communication with Aermec ANKI 025 Heat Pump on Arduino Mega over Serial3.
What is supported:
- Heat pump On/Off
- Select mode Winter/Summer
- Set temperature setpoint 
- Read alarm status indication

In case of a Modbus communication failure periodic (60s) retry logic is implemented for read and write.

Use:
``` cpp
#include "cmAermecHeatPumpModbus.h"

uint8_t slaveId = 1;
uint8_t derePin = 10;
uint8_t regime = 1; // 0 = Auto, 1 = Manual, 2 = Service => In Manual regime control module can be controled by user

cmAermecHeatPumpModbus coolingHeatPump2(slaveId, derePin);

void setup()
{
  Serial3.begin(19200, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=19200, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol
  //Define temperature setpoint limits as is specified for your device
  coolingHeatPump2.MinWinterTempSP = -5;
  coolingHeatPump2.MaxWinterTempSP = 18;
  coolingHeatPump2.MinSummerTempSP = 25;
  coolingHeatPump2.MaxSummerTempSP = 45;
  
  coolingHeatPump2.begin(Serial3);
}

void loop()
{
  coolingHeatPump2.process(regime);
}
```

In case that you need to change what is read form or written to the device extend cmAermecHeatPumpModbus class and override methods `readState` and `writeState`
