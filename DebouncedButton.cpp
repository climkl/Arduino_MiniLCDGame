#include "DebouncedButton.h"
#include "Arduino.h"

namespace Climduino
{
  DebouncedButton::DebouncedButton()
  {
  
  }

  void DebouncedButton::setup(uint8_t pin)
  {
    _pin = pin;
    pinMode(pin, INPUT_PULLUP);
    _lastBtnState = HIGH;
    _btnState = HIGH;
  }

  BtnEvent DebouncedButton::update()
  {
    BtnEvent evnt = BtnEvent::None;
    int reading = digitalRead(_pin);
    if (reading != _lastBtnState)
    {
      //will keep updating until bouncing is done
      _lastDebounceTime = millis();
    }

    if ((millis() - _lastDebounceTime > 50))
    {
      //when stabilized for more than 50 ms, we then accept the input state
      if (reading != _btnState)
      {
        _btnState = reading;
        if (reading == LOW) //cos we pull up
        {
          evnt = BtnEvent::Down;
          //Serial.println("Button Down!");
        }
        else if (reading == HIGH)
        {
          evnt = BtnEvent::Released;
          //Serial.println("Button Released!");
        }
      }
    }
    _lastBtnState = reading;

    return evnt;
  }

  int DebouncedButton::getBtnState()
  {
    return _btnState;
  }
}