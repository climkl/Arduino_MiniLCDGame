#include "LCD.h"
#include "Common.h"
#include "Constants.h"
#include "Gameplay.h"

enum GameState
{
  None,
  Start,
  InGame,
  GameOver,
};

//Climduino::LCD lcd(RS_PIN, RW_PIN, E_PIN, 6, 7, 8, 9, 10, 11, 12, 13); //8 bit
static Climduino::LCD _lcd(RS_PIN, RW_PIN, E_PIN, 10, 11, 12, 13); //4 bit

static uint8_t _last_screen[SCREEN_COLUMNS][SCREEN_ROWS]; // store the last one to compare and only render difference
static uint8_t _screen[SCREEN_COLUMNS][SCREEN_ROWS];
static GameState _lastGameState = GameState::None;
static GameState _gameState = GameState::Start;

static Gameplay _gameplay(_screen);

static Climduino::DebouncedButton _moveBtn;
static Climduino::DebouncedButton _enterBtn;

void setup() {
  Serial.begin(9600);

  _moveBtn.setup(BTN_MOVE_PIN);
  _enterBtn.setup(BTN_ENTER_PIN);

  _lcd.begin();
  
  _lcd.addCustomChar(PLAYER_CHAR, CustomPlayerCharMatrix);
}

void emptyGameplayScreen()
{
  for (int x = 0; x < SCREEN_COLUMNS; x++) {
    for (int y = 0; y < SCREEN_ROWS; y++) {
      _last_screen[x][y] = EMPTY_CHAR;
      _screen[x][y] = EMPTY_CHAR;
    }
  }
}

void loop() {

  Climduino::BtnEvent moveBtnEvnt = _moveBtn.update();
  Climduino::BtnEvent enterBtnEvnt = _enterBtn.update();

  GameState currentState = _gameState;
  switch (_gameState)
  {
    case GameState::Start:
      if (_lastGameState != _gameState)
      {
        _lcd.setCursor(0, 0);
        _lcd.write(PLAYER_CHAR);
        _lcd.print(" Press Enter");
        _lcd.setCursor(0, 1);
        _lcd.print("To Start");
      }
      if (enterBtnEvnt == Climduino::BtnEvent::Down)
      {
        //Serial.print("Starting Game");
        _lcd.clearScreen();
        emptyGameplayScreen();
        _gameplay.StartGame();
        _gameState = GameState::InGame;
      }
      break;
    case GameState::GameOver:
     if (_lastGameState != _gameState)
      {
        _lcd.setCursor(0, 0);
        _lcd.print("GAME OVER");
        _lcd.setCursor(0, 1);
        _lcd.print("SCORE ");
        _lcd.print(_gameplay.GetScore());
      }
      if (enterBtnEvnt == Climduino::BtnEvent::Down)
      {
        _gameState = GameState::Start;
        _lcd.clearScreen();
      }
      break;
    case GameState::InGame:
      _gameplay.Update(moveBtnEvnt, enterBtnEvnt);
    
      for (int x = 0; x < SCREEN_COLUMNS; ++x)
      {
        for (int y = 0; y < SCREEN_ROWS; ++y)
        {
          uint8_t last = _last_screen[x][y];
          uint8_t current = _screen[x][y];

          if (last == current) continue;

          //Serial.print(String("x: ") + x + ", y: " + y + ", Char: ");
          //Serial.println((uint8_t)current, HEX);

          _lcd.setCursor(x, y);
          _lcd.write((uint8_t)current);

          _last_screen[x][y] = _screen[x][y];
        }
      };

      if (_gameplay.IsDead())
      {
        //Serial.print("GameOver");
        delay(500); //let them see the explosion
        _gameState = GameState::GameOver;
        _lcd.clearScreen();
        break;
      }
      break;
  }

  _lastGameState = currentState;
}