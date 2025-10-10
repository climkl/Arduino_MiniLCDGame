#pragma once

#include <Arduino.h>
#include "Common.h"
#include "Constants.h"
#include "DebouncedButton.h"
#include "stdlib.h"

struct EnemyInfo
{
  bool Alive = false;
  CPPLib::Common::Vector2Int Position;
};
  

class Gameplay
{
  public:
    Gameplay(uint8_t (&screen)[SCREEN_COLUMNS][SCREEN_ROWS]);

    void StartGame();
    void Update(Climduino::BtnEvent moveBtn, Climduino::BtnEvent enterBtn);

    bool IsDead() const
    {
      return _isDead;
    }

    unsigned long GetScore() const
    {
      return _score;
    }

  private:
    uint8_t (&_screen)[SCREEN_COLUMNS][SCREEN_ROWS];
    CPPLib::Common::Vector2Int _playerPos;
    EnemyInfo _enemies[MAX_ENEMIES]; //arduino doesn't have std::list or vector natively

    bool _gameplayActive = false;
    bool _isDead = false;
    int _currentTickInterval;
    unsigned long _lastTick;
    unsigned long _lastIncrementTickTime;

    uint8_t _enemySpawnCounter;

    unsigned long _score;

    void setPlayerPos(int x, int y);
    void onMoveBtnDown();

    bool trySpawnEnemy();
    void moveEnemies();
    void setEnemyPos(uint8_t arrIndex, int x, int y);
    void clearEnemy(uint8_t arrIndex);

    void setGameOver(int x, int y);
};

