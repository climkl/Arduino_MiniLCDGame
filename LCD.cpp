#include "LCD.h"
#include "Arduino.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

namespace Climduino
{
    LCD::LCD(uint8_t rs, uint8_t rw, uint8_t enable, 
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		        uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
    {
      internal_Init(false, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
    }

    LCD::LCD(uint8_t rs, uint8_t rw, uint8_t enable, 
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
    {
      internal_Init(true, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
    }

    void LCD::internal_Init(bool fourbits, uint8_t rs, uint8_t rw, uint8_t enable, 
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		        uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
    {
      _rs_pin = rs;
      _rw_pin = rw;
      _enable_pin = enable;

      _data_pins[0] = d0;
      _data_pins[1] = d1;
      _data_pins[2] = d2;
      _data_pins[3] = d3; 
      _data_pins[4] = d4;
      _data_pins[5] = d5;
      _data_pins[6] = d6;
      _data_pins[7] = d7; 

      if (fourbits)
        _functionParam = FUNC_4BITMODE | FUNC_2LINE | FUNC_5x8DOTS;
      else
        _functionParam = FUNC_8BITMODE | FUNC_2LINE | FUNC_5x8DOTS;

      begin();
    }

    void LCD::begin()
    {
      //TODO allow row and column and dot size config
      _columns = 16;
      _rows = 2;
      setRowOffsets();
      
      //see pg 45

      //set pin Modes
      pinMode(_rs_pin, OUTPUT);

      if (_rw_pin != 0)
        pinMode(_rw_pin, OUTPUT);

      pinMode(_enable_pin, OUTPUT);
      int pinCount = isFourBit() ? 4: 8;
      for (int i = 0; i < pinCount; ++i)
      {
        pinMode(_data_pins[i], OUTPUT); 
      }

      //"Wait for more than 40ms after Vcc rises to 2.7V"
      delayMicroseconds(45000); //TODO hogging time

      //Write Defaults
      digitalWrite(_rs_pin, LOW); //instruction mode
      digitalWrite(_enable_pin, LOW); //start low so we can pulse later
      
      //TODO rw pin support - for simplicity now we just ignore
      delayMicroseconds(15000); //wait > 15ms

      if (isFourBit())
      {
        //4 bit intialization routine
        //pg 46

        //special because it starts in 8 bit mode
        //so we can't just send 2 nibbles off the bat
        //but neither can we send 8 bits since... we don't have 8 pins!

        //follow the instructions says, set DB5 and DB4 to 1 (as part of the 4 bit D4-7)
        write_4bit(0x03);
        delayMicroseconds(4100 + 400); //wait > 4.1ms

        write_4bit(0x03);
        delayMicroseconds(4100 + 400); //wait > 4.1ms

        write_4bit(0x03);
        delayMicroseconds(100 + 50); //wait > 100 microseconds

        //actually sets to 4 bit mode now
        write_4bit(0x02);
      }
      else
      {
        //8 bit Initialization Routine
        //pg 45
       
        command(CMD_FUNCTIONSET | _functionParam);
        delayMicroseconds(4100 + 400); //wait > 4.1ms
        
        command(CMD_FUNCTIONSET | _functionParam);
        delayMicroseconds(100 + 50); //wait > 100 microseconds

        command(CMD_FUNCTIONSET | _functionParam);
      }

      //final set
      command(CMD_FUNCTIONSET | _functionParam);

      _displayControlParam = DISPLAY_ON | DISPLAY_CURSOR_OFF | DISPLAY_BLINK_OFF; //DISPLAY_CURSOR_ON | DISPLAY_BLINK_ON;
      command(CMD_DISPLAYCONTROL | _displayControlParam);

      clearScreen();
      setCursor(0, 0);

      _entryModeParam = ENTRYMODE_INCREMENT | ENTRYMODE_CURSOR;
      command(CMD_ENTRY_MODE | _entryModeParam);

      Serial.println("LCD Initialized");
    }

    void LCD::clearScreen()
    {
      command(CMD_CLEAR_DISPLAY);
      delayMicroseconds(2000);
    }

    // Cursor =========================
    void LCD::setRowOffsets()
    {
      //cos its like 
      //00 01 02 ---- 0F
      //40 41 42 ---- 4F
      _rowOffsets[0] = 0x00;
      _rowOffsets[1] = 0x40;
    }

    void LCD::setCursor(uint8_t column, uint8_t row)
    {
      if (column > _columns - 1)
      {
        Serial.print("Trying to set invalid column: ");
        Serial.print(column);
        return;
      }
      if (row > _rows - 1)
      {
        Serial.print("Trying to set invalid row: ");
        Serial.print(row);
        return;
      }

      // 1 bit for command, 7 bits for the address data
      uint8_t address = column + _rowOffsets[row];
      command(CMD_SETDDRAMADDR | address);
    }

    void LCD::addCustomChar(uint8_t slotIndex, const uint8_t (&charMatrix)[8])
    {
      command(CMD_SETCGRAMADDR | (slotIndex << 3)); // << 3 = *8
      for (int i = 0; i < 8; i++) 
      {
        write(charMatrix[i]);  // goes into CGRAM, not screen
      }
    }

    inline void LCD::command(uint8_t value)
    {
      send(value, LOW);
    }

    inline size_t LCD::write(uint8_t value)
    {
      send(value, HIGH);
      return 1;
    }


    // Core Low Level ============================================================
    inline bool LCD::isFourBit()
    {
      //cannot do _functionParam & FUNC_4BITMODE since FUNC_4BITMODE is just 0
      return !(_functionParam & FUNC_8BITMODE); 
    }

    void LCD::send(uint8_t value, uint8_t mode)
    {
      //set mode - command instructions or character data
      digitalWrite(_rs_pin, mode);

      if (isFourBit())
      {
        write_4bit(value >> 4); //send high nibble D4-D7
        write_4bit(value); //write func already only reads the lower bits
      }
      else
      {
        write_8bit(value);
      }

      delayMicroseconds(37 + 50); // > 37 us
    }

    void LCD::write_8bit(uint8_t value)
    {
      for (int i = 0; i < 8; ++i)
      {
        digitalWrite(_data_pins[i], (value >> i) & 0x01);
      }

      pulse();
    }

    void LCD::write_4bit(uint8_t value)
    {
      for (int i = 0; i < 4; ++i)
      {
        //e.g. 0010
        //shift right = 0001
        //check if its 1 or 0
        digitalWrite(_data_pins[i], (value >> i) & 0x01); 
      }

      pulse();
    }

    void LCD::pulse()
    {
      digitalWrite(_enable_pin, LOW);
      delayMicroseconds(1); // > 450 ns - pg 49
      digitalWrite(_enable_pin, HIGH);
      delayMicroseconds(1); // > 450 ns
      digitalWrite(_enable_pin, LOW);
      //delayMicroseconds(37 + 50); // > 37 us
    }
}