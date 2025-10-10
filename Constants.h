#pragma once 

//Pin Defines
constexpr uint8_t RS_PIN = 2;
constexpr uint8_t RW_PIN = 0;
constexpr uint8_t E_PIN = 3;
constexpr uint8_t BTN_MOVE_PIN = 5;
constexpr uint8_t BTN_ENTER_PIN = 6;

//Screen
constexpr uint8_t SCREEN_ROWS = 2;
constexpr uint8_t SCREEN_COLUMNS = 16;

//Character Codes
constexpr uint8_t EMPTY_CHAR = 0x20;
constexpr uint8_t PLAYER_CHAR = 1; //0xFF //slot index to use with custom player char
constexpr uint8_t ENEMY_CHAR = 0xFF; //0xEF
constexpr uint8_t EXPLOSION_CHAR = 0x2A;

//5 x 8 matrix
constexpr uint8_t CustomPlayerCharMatrix[8] = 
{
  0b11111,
  0b11111,
  0b10101,
  0b11111,
  0b01110,
  0b11111,
  0b01110,
  0b01010,
};

//Gameplay Balance
constexpr uint8_t MAX_ENEMIES = 15;
constexpr int  MAX_TICK_INTERVAL = 500; //Tick time per enemy action (ms) at the start. Player is still free to roam in between ticks
constexpr int MIN_TICK_INTERVAL = 50;
constexpr int TICKTIME_INCREMENTTIME = 300; //Speed tick time every this many ms
constexpr int SPAWN_MAX_TICKS = 4;
constexpr int SPAWN_MIN_TICKS = 2;