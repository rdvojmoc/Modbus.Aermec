#ifndef cmAermecHeatPumpModbus_h;
#define cmAermecHeatPumpModbus_h;

#include <ModbusMaster.h>
#include "cmAermecHeatPump.h"

class cmAermecHeatPumpModbus : public cmAermecHeatPump{
    private:
      ModbusMaster _node;
    protected:
      uint8_t _slaveId;
      Stream &_stream;
      uint8_t _dere; //Driver Enable/Receiver Enable
      
      void preTransmission(){
        digitalWrite(_dere, 1);
      };
      
      void postTransmission(){
        digitalWrite(_dere, 0);
      };
      
    public:
      cmAermecHeatPumpModbus(uint8_t slaveId, Stream &stream, uint8_t dere){
        _slaveId = slaveId;
        _stream = stream;
        _dere = dere;
      };  
      
      void begin(){
        //set selected pin to control DE/RE
        pinMode(_dere, OUTPUT);
        
        //set callback methods to control DE/RE pins before transmision and after transmision
        _node.preTransmission(preTransmission);
        _node.postTransmission(postTransmission);
        
        //start communication with heat pump
        _node.begin(_slaveId, _stream);
      }
};
#endif
