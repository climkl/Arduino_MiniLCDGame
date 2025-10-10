#pragma once

#include "inttypes.h"
#include "Print.h" //arduino

namespace Climduino
{
    //Define all our constants
    //we are using a LCD1602 which uses the Hitachi HD44780 controller chip. Manual: https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf

    //pg24/25

    //Commands
    #define CMD_CLEAR_DISPLAY 0x01 //D0
    #define CMD_RETURN_HOME 0x02 //D1
    #define CMD_ENTRY_MODE 0x04 //D2. Defines what automatically happens when you write a characters.
    #define CMD_DISPLAYCONTROL 0x08 //D3. D2 = I/D. D1 = S.
    #define CMD_CURSORSHIFT 0x10 //D4. D3 = S/C. D2 = R/L.
    #define CMD_FUNCTIONSET 0x20 //D5. D4 = DL. D3 = N. D2 = F.
    #define CMD_SETCGRAMADDR 0x40 //D6. Custom char
    #define CMD_SETDDRAMADDR 0x80 //D7. Address is the cursor.

    //Extra bits for Specific Commands
    //CMD_ENTRY_MODE
    #define ENTRYMODE_INCREMENT 0x02 //D1
    #define ENTRYMODE_DECREMENT 0x00
    #define ENTRYMODE_SHIFT 0x01 
    #define ENTRYMODE_CURSOR 0x00

    //CMD_DISPLAYCONTROL
    #define DISPLAY_BLINK_ON 0x01
    #define DISPLAY_BLINK_OFF 0x00
    #define DISPLAY_CURSOR_ON 0x02
    #define DISPLAY_CURSOR_OFF 0x00
    #define DISPLAY_ON 0x04
    #define DISPLAY_OFF 0x00

    //CMD_CURSORSHIFT
    #define CURSOR_RIGHT 0x04
    #define CURSOR_LEFT 0x00
    #define CUROSR_DISPLAYSHIFT 0x08
    #define CURSOR_MOVE 0x00

    //CMD_FUNCTIONSET
    #define FUNC_5x8DOTS 0x00
    #define FUNC_5x10DOTS 0x04
    #define FUNC_1LINE 0x00
    #define FUNC_2LINE 0x08
    #define FUNC_4BITMODE 0x00 //D4=0
    #define FUNC_8BITMODE 0x10 //D4=1

    class LCD : public Print {
        public:

        //Constructors - define pins + 4 or 8 bits depending on arguments
        LCD(uint8_t rs, uint8_t rw, uint8_t enable, 
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

        LCD(uint8_t rs, uint8_t rw, uint8_t enable,
		    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

        //call in Arduino setup()
        void begin();
        
        //print functionality by Arduino library, by overrding their write
        //call write() for char using hex code, print() for strings
        virtual size_t write(uint8_t value);
        using Print::write;

        void clearScreen();
        
        //where 0 0 is the top left
        void setCursor(uint8_t column, uint8_t row);

        //8 slot index we can use
        void addCustomChar(uint8_t slotIndex, const uint8_t (&charMatrix)[8]);
        
        private:

        void internal_Init(bool fourbits, uint8_t rs, uint8_t rw, uint8_t enable, 
            uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
		    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

        //store our pins =============
        uint8_t _rs_pin; //register select
        uint8_t _rw_pin; //read/write mode
        uint8_t _enable_pin;
        uint8_t _data_pins[8];

        //and configurations
        uint8_t _functionParam; //bit flag for CMD_FUNCTIONSET
        uint8_t _displayControlParam; //bit flag for CMD_DISPLAYCONTROL
        uint8_t _entryModeParam; //bit flag for CMD_ENTRYMODE

        uint8_t _rowOffsets[2];
        uint8_t _columns;
        uint8_t _rows;

        //low level commands to hardware =============
        // send data with either command or character mode
        void send(uint8_t value, uint8_t mode);
        void write_8bit(uint8_t value);
        void write_4bit(uint8_t value);
        //pulse is used to say "DO IT NOW!"
        void pulse();
        // send a command
        void command(uint8_t value);
        // write a character
        void writeChar(uint8_t value);
        
        //helper func
        bool isFourBit();
        
        void setRowOffsets();
    };


}