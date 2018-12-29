#ifndef cmAermecHeatPumpModbus_h
#define cmAermecHeatPumpModbus_h

#include "src/ModbusMaster/ModbusMaster.h"
#include "cmAermecHeatPump.h"

class cmAermecHeatPumpModbus : public cmAermecHeatPump{
    private:
      ModbusMaster _node;
      bool firstChange = true;
      bool _oldXS;
      bool _oldModeXS;
      float _oldTempSP;
      long lastReadTime = 0;
      long lastReadRetryTime = 0;
      long lastWriteRetryTime = 0;
      long currentReadTime = 0;
      long currentWriteTime = 0;

      uint16_t scaleValue(float value, uint16_t factor) {
        return value * factor;  
      }

      float inverseScaleValue(int16_t value, float factor){
        //prevent division by 0
        if(value == 0){
          return 0;
        }
        return (float) value / factor;
      }

    protected:
      static const float DEF_MAN_TEMPSP = -1000;

      /**
      Defines time span in ms in which read of the control module state will be done
      */
      static const long READ_STATE_TIME = 1000;

      /**
      Defines time span in ms in which write or read transaction will be repeated
      */
      static const long RETRY_TIME = 60000;
      
      uint8_t _slaveId;
      
      /**
      Driver Enable/Receiver Enable
      */
      uint8_t _dere;
      
      /**
      Set DE/RE state before communication is started
      */
      virtual void setDERE(){
        pinMode(_dere, OUTPUT);
        digitalWrite(_dere, 0);
      }
      
      /**
      Write high state to _dere pin before Modbus transaction - ModbusMaster library need this for correct behavior
      */
      virtual void preTransmission(){
          digitalWrite(_dere, 1);
      };

      /**
      Write low state to _dere pin after Modbus transaction - ModbusMaster library need this for correct behavior
      */
      virtual void postTransmission(){
          digitalWrite(_dere, 0);
      };

      /** Check if desired temperature setpoint is within min/max limits in mode that is selected at the moment (ModeXS)
      @param tempSP desired temperature setpoint
      @return valid setpoint value within min/max limits
      */
      virtual float checkTemperatureSetPoint(float tempSP){
          
          float maxSP = (ModeXS) ? MaxSummerTempSP : MaxWinterTempSP;
          float minSP = (ModeXS) ? MinSummerTempSP : MinWinterTempSP;
          
          if(tempSP < minSP || tempSP > maxSP)
          {
            //if tempSP is outside defined limits return minimal summer temperature if in summer mode or maximal winter temperature if in winter mode  
            return (ModeXS) ? minSP : maxSP;
          }

          return tempSP;
      }

      /**
      Method cheks if Modbus transaction was successful or not
      @param code transaction code returned by the library
      @return true if transaction was successful or false if error occured
      */
      virtual bool isModbusTransactionSuccessful(uint8_t code){
        if(code == _node.ku8MBSuccess){
          return true;
        }
        else {
          return false;
        }
      }

      
      /**
      Method implements retry logic when reading and periodic reads defined by READ_STATE_TIME
      */
      virtual void handleReadState(){
        currentReadTime = millis();
        if(!isModbusTransactionSuccessful(ReadResultCode)){
          if(currentReadTime - lastReadRetryTime > RETRY_TIME) {
            readState();
            lastReadRetryTime = currentReadTime;
          }
        }
        else{
          if(currentReadTime - lastReadTime > READ_STATE_TIME ){
            long stopwatch = millis();
            readState();
            lastReadTime = currentReadTime;
          }
        }
      }

      /**
      Method reads new state from MODBUS enabled heat pump - override this method in a new class if you want to change what is read from the heat pump
      */
      virtual void readState(){
        //read registers from control module (heat pump)
        ReadResultCode = _node.readCoils(0, 200);
        
        //in case of successful read set boolean values that have been read
        if (isModbusTransactionSuccessful(ReadResultCode)){
          AlarmRemote = bitRead(_node.getResponseBuffer(AlarmIndicationRegisterAddress / 8), (AlarmIndicationRegisterAddress % 8) - 1);
        }
        
        ReadResultCode = _node.readHoldingRegisters(0,33);
        //in case of successful read set number values that have been read
        if (isModbusTransactionSuccessful(ReadResultCode)){
          OutsideTemperature = inverseScaleValue(_node.getResponseBuffer(OutsideTemperatureRegisterAddress), 10);
        }
      }
       
      /**
      Method implements retry logic when writing is not successful or call writeState
      */
      virtual void handleWriteState(){
        currentWriteTime = millis();
        if(!isModbusTransactionSuccessful(WriteResultCode)){
          if(currentWriteTime - lastWriteRetryTime > RETRY_TIME) {
            writeState();
            lastWriteRetryTime = currentWriteTime;
          }
        }
        else {
          writeState();
        }
      }
      
       /**
       Method writes new state to MODBUS enabled heat pump - override this method in a new class if you want to change what is written to the heat pump
       To optimize writing over MODBUS rembmer old state to compere it with new state
       */
       virtual void writeState(){
               
        //detect state change and write it to the registers
        if(_oldXS != XS | firstChange) {
           WriteResultCode = _node.writeSingleCoil(OnOffRegisterAddress, XS);
           //save current state to old state
           if(isModbusTransactionSuccessful(WriteResultCode)) {
              _oldXS = XS;
           }
        }

        if(_oldModeXS != ModeXS | firstChange) {
           WriteResultCode = _node.writeSingleCoil(ModeRegisterAddress, ModeXS);
           //save current state to old state
           if(isModbusTransactionSuccessful(WriteResultCode)) {
              _oldModeXS = ModeXS;
            }
        }

        if(_oldTempSP != TempSP | firstChange) {
          //depending on the mode (winter/summer) decide to which register write the set point
          if(ModeXS) 
          {
            //summer mode
            WriteResultCode = _node.writeSingleRegister(SummerTempSPRegisterAddress, scaleValue(TempSP, 10));
          }
          else
          {
            //winter mode
            WriteResultCode = _node.writeSingleRegister(WinterTempSPRegisterAddress, scaleValue(TempSP, 10));
          }
          //save current state to old state
          if(isModbusTransactionSuccessful(WriteResultCode)) {
            _oldTempSP = TempSP;
          }
        }
        
        //try to write until first write is successfull
        if(firstChange && WriteResultCode == 0 ) firstChange = false; 
       }
      
    public:
      /**
      Code returned by Modbus library which indicates transaction success or failiure - consult Modbus library documentation to understand codes meaning
      */
      uint8_t WriteResultCode;
      
      /**
      Code returned by Modbus library which indicates transaction success or failiure - consult Modbus library documentation to understand codes meaning
      */
      uint8_t ReadResultCode;

      /**
      Control module (heat pump) temperature setpoint set by software
      */
      float AvtoTempSP;

      /**
      Control module (heat pump) temperature setpoint set by third party (user)
      */
      float ManualTempSP = DEF_MAN_TEMPSP;

      /**
      Temperature setpoint state of the control module (heat pump) that is copied to remote contacts, over MODBUS, etc...
      */
      float TempSP;

      /**
      Maximal allowed temperature setpoint in summer mode - should be the same as heat pump configuration 
      */
      float MaxSummerTempSP = 45;

      /**
      Minimal allowed temperature setpoint in summer mode - should be the same as heat pump configuration 
      */
      float MinSummerTempSP = 25;

      /**
      Maximal allowed temperature setpoint in winter mode - should be the same as heat pump configuration 
      */
      float MaxWinterTempSP = 16;

      /**
      Minimal allowed temperature setpoint in winter mode - should be the same as heat pump configuration 
      */
      float MinWinterTempSP = 7;

      /**
      Register addresses as defined for ANKI type heat pump - consult documentation for addresses that applies for other devices
      */
      //holding registers
      uint16_t OnOffRegisterAddress = 0x0000;
      uint16_t ModeRegisterAddress = 0x0001;
      uint16_t SummerTempSPRegisterAddress = 41;
      uint16_t WinterTempSPRegisterAddress = 39;
      uint16_t OutsideTemperatureRegisterAddress = 6;
      //coli registers
      uint16_t AlarmIndicationRegisterAddress = 4;
      
      /**
      Indicates if there is active alarm on heat pump 
      */
      bool AlarmRemote;

      /**
      Holds outside temperature measured by heatpump
      */
      float OutsideTemperature;
      
      
      cmAermecHeatPumpModbus(uint8_t slaveId, uint8_t dere){
        _slaveId = slaveId;
        _dere = dere;
      };  

      /**
      @param stream Serial used for communication over RS-485 with the device
      */
      void begin(Stream &stream){
        
        //set default temperature setpoint - if in summer mode set minimal summer temp possible, if in winter mode set maximal winter temp
        AvtoTempSP = ModeXS ? MinSummerTempSP : MaxWinterTempSP;
        
        //set selected pin to control DE/RE
        setDERE();
        
        _node.preTransmission(MakeDelegate(this, &cmAermecHeatPumpModbus::preTransmission));
        _node.postTransmission(MakeDelegate(this, &cmAermecHeatPumpModbus::postTransmission));
        
        //start communication with heat pump
        _node.begin(_slaveId, stream);

      }

      /** Override base class process method to add MODBUS functions
      @param regime current regime of the process unit  0 = Automatic, 1 = Manual, 2 = Service, ...
      */
      virtual void process(int regime){

        handleReadState();
        
        //call base method before writing to ouputs
        cmAermecHeatPump::process(regime);

        //handle TempSP - always check temperature setpoint before 
        AvtoTempSP = checkTemperatureSetPoint(AvtoTempSP);
        if(regime == REGIME_AUTO){
          TempSP = AvtoTempSP;
          ManualTempSP = DEF_MAN_TEMPSP;
        }
        else if (regime > REGIME_AUTO){
          //if manual temperature setpoint is not set then AvtoTempSP is used else check the limits and set the setpoint 
          if(ManualTempSP <= DEF_MAN_TEMPSP){
            TempSP = AvtoTempSP;
          }
          else {
            TempSP = ManualTempSP = checkTemperatureSetPoint(ManualTempSP);
          }
        }

        Manual = ManualOn || ManualModeOn || ManualTempSP != DEF_MAN_TEMPSP;

        handleWriteState();
      }

       
};
#endif
