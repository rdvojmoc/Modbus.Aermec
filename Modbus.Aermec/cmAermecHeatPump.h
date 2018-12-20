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
      ON/OFF state of the control module (heat ppump) that is copied to remote contacts, over MODBUS, etc...
      */
      bool XS;
      
      bool AvtoModeOn;
      bool ManualModeOn;
      bool ModeXS;
      bool Alarm;
      bool Manual;
      bool Interlock;

      /** Process state of the properties 
      @param current regime of the process unit  0 = Automatic, 1 = Manual, 2 = Service, ...
      */
      virtual void process(int regime){
        
        XS = false;

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
        }
        else if (regime > REGIME_AUTO){
          //if in manual or service regime, XS could be set by third party  
          XS = (AvtoOn && !ManualOn || ManualOn);
        }

        Manual = ManualOn;
      };
};
#endif
