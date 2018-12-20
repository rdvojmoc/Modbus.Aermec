#ifndef cmAermecHeatPump_h
#define cmAermecHeatPump_h

class cmAermecHeatPump {
  
    static const uint8_t REGIME_AUTO = 0;
    static const uint8_t REGIME_MANUAL = 1;
    static const uint8_t REGIME_SERVICE = 2;
    
    private:
    
    protected:
      /** 
      Constructor is protected to prevent directly creating cmAermecHeatPump objects
      */
      cmAermecHeatPump()
      {
      };
      
    public:
      /**
      ON/OFF state of the contorl module (heat pump) is set by software
      */
      bool AvtoOn;

      /**
      ON/OFF state of the contorl module (heat pump) is set by third party (user)
      */
      bool ManualOn;
      
      /**
      ON/OFF state of the control module (heat pump) that is copied to remote contacts, over MODBUS, etc...
      */
      bool XS;

      /**
      Winter/Summer state of the control module (heat pump) is set by software
      */
      bool AvtoModeOn;
      
      /**
      Winter/Summer state of the control module (heat pump) is set by third party (user)
      */
      bool ManualModeOn;
      
      /**
      Winter/Summer state of the control module (heat pump) that is copied to remote contacts, over MODBUS, etc...
      */
      bool ModeXS;
      
      /**
      Control module (heat pmup) alarms are detected - set by control module or the procces logic
      */
      bool Alarm;

      /**
      Control module (heat pump) is controled by third party (user)
      */
      bool Manual;

      /**
      Control module (heat pump) control from software or third party is not possible because preconditions for safe operation are not met
      */
      bool Interlock;

      /** Process state of the properties 
      @param current regime of the process unit  0 = Automatic, 1 = Manual, 2 = Service, ...
      */
      virtual void process(int regime){
        
        XS = false;
        ModeXS  = false;

        //if Alarm is present (emergency-stop button is pushed, etc...), set machine to safe state (turn of the heat pump)
        if(Alarm){
          ManualOn = false;
          AvtoOn = false;
          XS = false;
        }

        if(regime == REGIME_AUTO){
          //if in automatic regime reset ManualOn bit(set by third party) and set XS to a current value of AvtoOn bit
          ManualOn = false;
          XS = AvtoOn;

          ManualModeOn = false;
          ModeXS = AvtoModeOn;
          
        }
        else if (regime > REGIME_AUTO){
          //if in manual or service regime, XS or ModeXS could be set by third party  
          XS = (AvtoOn && !ManualOn || ManualOn); 
          
          ModeXS = (AvtoModeOn && !ManualModeOn || ManualModeOn);
        }

        Manual = ManualOn || ManualModeOn;

        if(Interlock && regime < REGIME_SERVICE)
        {
          //in case of active interlock and if machine is in automatic or manual mode set heat pump to safe state
          XS = false;
        }
      };
};
#endif
