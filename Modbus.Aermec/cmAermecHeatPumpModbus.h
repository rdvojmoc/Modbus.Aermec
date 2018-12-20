#ifndef cmAermecHeatPumpModbus_h
#define cmAermecHeatPumpModbus_h

#include "src/ModbusMaster/ModbusMaster.h"
#include "cmAermecHeatPump.h"

class cmAermecHeatPumpModbus : public cmAermecHeatPump{
    private:
      ModbusMaster _node;
      int i = 0;
      bool state = true;
      bool _oldXS;
      bool _oldModeXS;
    protected:
    
      uint8_t _slaveId;
      
      /**
      Driver Enable/Receiver Enable
      */
      uint8_t _dere;
      
      /**
      Set DE/RE state before communication is started
      */
      void setDERE(){
        pinMode(_dere, OUTPUT);
        digitalWrite(_dere, 0);
      }
      
      /**
      */
      void preTransmission(){
          digitalWrite(_dere, 1);
      };

      /**
      */
      void postTransmission(){
          digitalWrite(_dere, 0);
      };
      
    public:
      /**
      Code returned by Modbus library which indicates transaction success or failiure - consult Modbus library documentation to understand codes meaning
      */
      uint8_t TransactionResultCode;
      
      cmAermecHeatPumpModbus(uint8_t slaveId, uint8_t dere){
        _slaveId = slaveId;
        _dere = dere;
      };  

      /**
      */
      void begin(Stream &stream){
        //set selected pin to control DE/RE
        setDERE();
        
        _node.preTransmission(MakeDelegate(this, &cmAermecHeatPumpModbus::preTransmission));
        _node.postTransmission(MakeDelegate(this, &cmAermecHeatPumpModbus::postTransmission));
        
        //start communication with heat pump
        _node.begin(_slaveId, stream);

      }

      /** Override base class process method to add MODBUS functions
      @param current regime of the process unit  0 = Automatic, 1 = Manual, 2 = Service, ...
      */
      virtual void process(int regime){
        //To optimize writing over MODBUS rembmer old state to compere it with new state
        _oldXS = XS;
        _oldModeXS = ModeXS;
        
        //Call base method before writing to ouputs
        cmAermecHeatPump::process(regime);

        //Detect state change and write it to the registers
        if(_oldXS != XS) {
           Serial.println("XS Changed!");
           TransactionResultCode = _node.writeSingleCoil(0x0000, XS);
        }

        if(_oldModeXS != ModeXS) {
           TransactionResultCode = _node.writeSingleCoil(0x0001, ModeXS);
        }
//        i = i+1;
//        int result = 0;
//        // set word 0 of TX buffer to least-significant word of counter (bits 15..0)
//        _node.setTransmitBuffer(0, lowWord(i));
//        
//        // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
//        _node.setTransmitBuffer(1, highWord(i));
//        
//        //slave: write TX buffer to (2) 16-bit registers starting at register 0
//        result = _node.writeMultipleRegisters(0, 2);
//
//        // Toggle the coil at address 0x0002 (Manual Load Control)
//        result = _node.writeSingleCoil(0x0000, state);
//        state = !state;
//        
//        // slave: read (6) 16-bit registers starting at register 2 to RX buffer
//        result = _node.readHoldingRegisters(2, 6);
//        
//        // do something with data if read is successful
//        if (result == _node.ku8MBSuccess)
//        {
//          uint8_t j = 0;
//          uint16_t data[6];
//          for (j = 0; j < 6; j++)
//          {
//            data[j] = _node.getResponseBuffer(j);
//            Serial.print(data[j]);
//          }
//        }
//        Serial.println(result);
      }
};
#endif
