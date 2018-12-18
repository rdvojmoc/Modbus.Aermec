#ifndef cmAermecHeatPump_h;
#define cmAermecHeatPump_h;

class cmAermecHeatPump {

    protected:
      //Constructor is protected to prevent directly creating cmAermecHeatPump objects
      cmAermecHeatPump()
      {
      };
    public:
      bool AvtoOn;
      bool ManualOn;
      bool XS;
      bool AvtoModeOn;
      bool ManualModeOn;
      bool ModeXS;
      bool Alarm;
      bool Manual;
      bool Interlock;

      /** Initialize the control module
     *
     * 
     */
      virtual void begin() = 0 ;
      
      virtual void process(int regime){
        
      };
};
#endif
