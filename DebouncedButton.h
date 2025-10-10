#pragma once

#include "inttypes.h"

namespace Climduino
{
  enum BtnEvent
  {
    None, //no changes this frame
    Released, //let go this frame
    Down, //just pressed down this frame 
  };
  
  class DebouncedButton
  {

  public:
    DebouncedButton();
    setup(uint8_t pin);

    //also returns change in Btn state, as a workaround to event (for simple cases where caller is updating and retrieving the state)
    BtnEvent update();

    //returns 0 - not held, 1 - held - used for continuous state
    int getBtnState();
    
    //KIV Arduino Uno doesn't support std::function without additional library, making events a bit tedious since u need static functions.

  private:
    uint8_t _pin;
    int _btnState; //stable states
    int _lastBtnState; //raw bouncy state all the time
    unsigned long _lastDebounceTime;
  };
}