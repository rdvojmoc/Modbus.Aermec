/*

  Basic.pde - example using ModbusMaster library

  Library:: ModbusMaster
  Author:: Doc Walker <4-20ma@wvfans.net>

  Copyright:: 2009-2016 Doc Walker

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/

#include <ModbusMaster.h>
#include "cmAermecHeatPumpModbus.h";

// instantiate ModbusMaster object
ModbusMaster node;
cmAermecHeatPumpModbus coolingHeatPump(1, Serial2, 8);
#define MAX485_DE      8


void preTransmission2()
{
  //digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission2()
{
  //digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_DE, 0);
  
  Serial.begin(115200);
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial2.begin(9600, SERIAL_8N2); //SERIAL_8N2 BAUD_RATE=9600, PARITY = NONE, DATA_BITS = 8, STOP_BITS = 2 as defined in MOD485K.pdf => Modaer protocol

  Serial.println("Starting...");
  // communicate with Modbus slave ID 2 over Serial (port 0)
  node.begin(1, Serial2);
  node.preTransmission(preTransmission2);
  node.postTransmission(postTransmission2);
  Serial.println("Started.");

  coolingHeatPump.begin();
}


bool state = true;

void loop()
{
  static uint32_t i;
  uint8_t j, result;
  uint16_t data[6];
  
  i = i+2;
  
    Serial.println("Looping");
  // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
  node.setTransmitBuffer(0, lowWord(i));
  
  // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
  node.setTransmitBuffer(1, highWord(i));
  
  // slave: write TX buffer to (2) 16-bit registers starting at register 0
  result = node.writeMultipleRegisters(0, 2);
  
  // Toggle the coil at address 0x0002 (Manual Load Control)
  result = node.writeSingleCoil(0x0000, state);
  state = !state;
  
  // slave: read (6) 16-bit registers starting at register 2 to RX buffer
  result = node.readHoldingRegisters(2, 6);
  Serial.println(result);
  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (j = 0; j < 6; j++)
    {
      data[j] = node.getResponseBuffer(j);
      Serial.print(data[j]);
    }
    Serial.println("------");
  }
  delay(1000);
}
