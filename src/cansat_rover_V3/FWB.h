// for types of button presses for the Arduino: click, double-click, long press (and release), very long press 
// might work with other controllers. modified from: http://pastebin.com/gQLTrHVF
/*
  i had to make a few changes (update it?) to make the code work. main changes:
  1. REMOVED: #include "WProgram.h" (no longer required).
  2. CHANGED: bpUP TO bp.
  3. ADDED: event listeners for ALL four types of presses in the sample code.
  4. CHANGED: time intervals for single, double, long clicks and for long press. these feel more intuitive to me.
  5. CHANGED: event OnLongPress is raised ONLY after the button is released. this, again, feels more intuitive. code is tested and at http://pastebin.com/87cCn6h9
*/

//
// jb_exe
// Class from the code of JEFF'S ARDUINO BLOG
// http://jmsarduino.blogspot.com/2009/10/4-way-button-click-double-click-hold.html
// 
// the modified version at http://pastebin.com/gQLTrHVF
// further modified by mahesh [at] tinymogul.com


// How to use me :
// "FWB_Project.pde"
// #include "FWB.h"
// #define BP 0 //the pin where your button is connected
//
// FWB bp;
//
// void OnClick(int pin) {
//   //Your code here
// }
//
// void OnDblClick(int pin) {
//   //Your code here
// }
//
// void OnLongPress(int pin) {
//   //Your code here
// }
//
// void OnVLongPress(int pin) {
//   //Your code here
// }
//
// void setup()
// {
//   // errors in code fixed here. empty event handlers added.
//   bp.Configure(BP);
//   bp.OnClick = OnClick;
//   bp.OnDblClick = OnDblClick;
//   bp.OnLongPress = OnLongPress;
//   bp.OnVLongPress = OnVLongPress;
// }
//
// void loop()
// {
//  // Test button state
//  bp.CheckBP();
// }

#define PULL_UP         1

class FWB
{
private:
  int _pin;

  // Properties //
  ////////////////

  // Debounce period to prevent flickering when pressing or releasing the button (in ms)
  int Debounce;
  // Max period between clicks for a double click event (in ms)
  int DblClickDelay;
  // Hold period for a long press event (in ms)
  int LongPressDelay;

  // Variables //
  ///////////////

  // Value read from button
  boolean state;
  // Last value of button state
  boolean _lastState;
  // whether we're waiting for a double click (down)
  boolean _dblClickWaiting;
  // whether to register a double click on next release, or whether to wait and click
  boolean _dblClickOnNextUp;
  // whether it's OK to do a single click
  boolean _singleClickOK;

  // time the button was pressed down
  long _downTime;
  // time the button was released
  long _upTime;

  // whether to ignore the button release because the click+hold was triggered
  boolean _ignoreUP;
  // when held, whether to wait for the up event
  boolean _waitForUP;
  // whether or not the hold event happened already
  boolean _longPressHappened;

public:
  void (*OnClick)(void);
  void (*OnDblClick)(void);
  void (*OnLongPress)(void);

  FWB()
     {
      // Initialization of properties
      Debounce = 30;
      DblClickDelay = 300;
      LongPressDelay = 2000;    

      // Initialization of variables
      state = true;
      _lastState = true;
      _dblClickWaiting = false;
      _dblClickOnNextUp = false;
      _singleClickOK = false; //Default = true
      _downTime = -1;
      _upTime = -1;
      _ignoreUP = false;
      _waitForUP = false;
      _longPressHappened = false;
     }
  
  void Configure(int pin)
     {
      _pin = pin;
      pinMode(_pin, INPUT);
      digitalWrite(_pin, HIGH); //Pull up pin
     }

   void CheckBP(void)
      {
       int resultEvent = 0;
       long millisRes = millis();
       state = digitalRead(_pin) == HIGH;

       //---- Κουμπί πατήθηκε ----
       if (state != PULL_UP && _lastState == PULL_UP  && (millisRes - _upTime) > Debounce)
          {
           //Serial.println("button down");
           _downTime = millisRes;
           _ignoreUP = false;
           _waitForUP = false;
           _singleClickOK = true;
           _longPressHappened = false;
           if ((millisRes - _upTime) < DblClickDelay && _dblClickOnNextUp == false && _dblClickWaiting == true)
               _dblClickOnNextUp = true;
           else
               _dblClickOnNextUp = false;
           _dblClickWaiting = false;
          }
       // Button released
       else if (state == PULL_UP && _lastState != PULL_UP && (millisRes - _downTime) > Debounce)
          {
           //Serial.println("button up");
           if (_ignoreUP == false) //Replace "(!_ignoreUP)" by "(not _ignoreUP)"
              {
               _upTime = millisRes;
               if (_dblClickOnNextUp == false) 
                   _dblClickWaiting = true;
               else
                  {
                   resultEvent = 2;
                   _dblClickOnNextUp = false;
                   _dblClickWaiting = false;
                   _singleClickOK = false;
                  }
              }
          }

       // Test for normal click event: DblClickDelay expired
       if (state == PULL_UP && (millisRes - _upTime) >= DblClickDelay && _dblClickWaiting == true && _dblClickOnNextUp == false && _singleClickOK == true && resultEvent != 2)
          {
           resultEvent = 1;
           _dblClickWaiting = false;
          }

       //----- Έλεγχος για παρατεταμένο πάτημα -----
       //Είναι πατημένο και ο χρόνος πέρασε την καθυστέρηση του LongPressDelay
       if (state != PULL_UP && (millisRes - _downTime) >= LongPressDelay)
          {
           //Πυροδότησε το παρατεταμένο πάτημα
           // Αν δεν πατήθηκε πριν παρατεταμένα
           if (_longPressHappened == false)
              {
               resultEvent = 3;
               _waitForUP = true;
               _ignoreUP = true;
               _dblClickOnNextUp = false;
               _dblClickWaiting = false;
               _downTime = millis(); //ισως δεν χρειάζεται
               _longPressHappened = true;
              }
          }

       _lastState = state;
       //if (resultEvent!=0)
       //  Serial.println((String)"resultEvent: " + (String) resultEvent);

       if (resultEvent == 1 && OnClick) OnClick();
       if (resultEvent == 2 && OnDblClick) OnDblClick();
       if (resultEvent == 3 && OnLongPress) OnLongPress();
       //  if (resultEvent != 0)
       //    Usb.println(resultEvent);
      } //Τέλος της CheckBP()
};  //Τέλος ορισμού της κλάσης
