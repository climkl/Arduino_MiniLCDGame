#include "Gameplay.h"

Gameplay::Gameplay(uint8_t (&screen)[SCREEN_COLUMNS][SCREEN_ROWS])
: _screen(screen)
{
  
}

void Gameplay::StartGame()
{
  _gameplayActive = true;
  _isDead = false;
  _currentTickInterval = MAX_TICK_INTERVAL;
  _lastTick = millis();
  _lastIncrementTickTime = millis();
  _score = 0;
  for (int i = 0 ; i < MAX_ENEMIES; ++i)
  {
    _enemies[i].Alive = false;
  }
  
  setPlayerPos(0, 0);
  trySpawnEnemy();
}

void Gameplay::setPlayerPos(int x, int y)
{
  _screen[_playerPos.x][_playerPos.y] = EMPTY_CHAR; //clear first
  _playerPos.x = x;
  _playerPos.y = y;

   uint8_t onScreen = _screen[x][y];
   if (onScreen == EMPTY_CHAR)
   {
    _screen[x][y] = PLAYER_CHAR;
   }
   else if (onScreen == ENEMY_CHAR)
   {
    setGameOver(x, y);
   }
}

void Gameplay::setGameOver(int x, int y)
{
  _isDead = true;
  _gameplayActive = false;

  _screen[x][y] = EXPLOSION_CHAR;
}

void Gameplay::setEnemyPos(uint8_t arrIndex, int x, int y)
{
  EnemyInfo &info = _enemies[arrIndex];
  if (!info.Alive)
  {
    Serial.println("Error trying to set position of a dead thing");
    return;
  }

  _screen[info.Position.x][info.Position.y] = EMPTY_CHAR;
  info.Position.x = x;
  info.Position.y = y;

  uint8_t onScreen = _screen[x][y];
  //Serial.print(String("pos x: ") + x + ", y: " + y + " char: 0x");
  //Serial.println((uint8_t)onScreen, HEX);
  if (onScreen == EMPTY_CHAR) //ok to move
  { 
    _screen[x][y] = ENEMY_CHAR;
  }
  else if (onScreen == PLAYER_CHAR)
  {
   setGameOver(x, y);
  }
}

void Gameplay::clearEnemy(uint8_t arrIndex)
{          
  //Serial.println("Clear enemy");
  EnemyInfo &info = _enemies[arrIndex];
  info.Alive = false;
  _screen[info.Position.x][info.Position.y] = EMPTY_CHAR;
}

void Gameplay::Update(Climduino::BtnEvent moveBtn, Climduino::BtnEvent enterBtn)
{
  if (_gameplayActive)
  {
    if (moveBtn == Climduino::BtnEvent::Down)
    {
      onMoveBtnDown();
    }

    unsigned long milli = millis();
    if (_currentTickInterval > MIN_TICK_INTERVAL)
    {
      if (milli - _lastIncrementTickTime > TICKTIME_INCREMENTTIME)
      {
        _currentTickInterval = (long)(_currentTickInterval * 0.995); //_currentTickInterval--;
        _lastIncrementTickTime = milli;
      }
    }

    //to simplify, we update all enemies according to the tick
    if (milli - _lastTick > _currentTickInterval)
    {
      moveEnemies();
      _lastTick = milli;

      if (_gameplayActive)
      {
        //Serial.print("SpawnCounter: ");
        //Serial.println(_enemySpawnCounter);

        if (_enemySpawnCounter - 1 == 0)
        {
          //dont decrement if no space to spawn
          trySpawnEnemy();
        }
        else
        {
          _enemySpawnCounter--;
        }
        _score++; //just increment per tick for ez scoring
      }
    }
    
    
  }
}

void Gameplay::moveEnemies()
{
  for (int i = 0; i < MAX_ENEMIES; ++i)
  {
      if (_enemies[i].Alive)
      {
        //move them left
        if (_enemies[i].Position.x > 0)
        {
          setEnemyPos(i, _enemies[i].Position.x - 1, _enemies[i].Position.y);
          //_enemies[i].LastTimeMoved = millis();
          if (!_gameplayActive) break;
        }
        else
        {
          //going off screen, we can cull them
          clearEnemy(i);
        }
    }
  }
}

void Gameplay::onMoveBtnDown()
{
  //since we just have 2 lanes and 1 button, a very simple up down toggle
  int y = _playerPos.y == 0 ? 1 : 0;
  setPlayerPos(_playerPos.x, y);
}

bool Gameplay::trySpawnEnemy()
{
  //Serial.println("Try Spawn Enemy");
  //find empty spot in array
  int index = -1;
  for (int i = 0; i < MAX_ENEMIES; ++i)
  {
    if (!_enemies[i].Alive)
    {
      index = i;
      break;
    }
  }

  if (index == -1) return false;

  //spawn at right edge
  int x = SCREEN_COLUMNS - 1;
  int y = 0;
  
  //find empty y
  if (_screen[x][0] == EMPTY_CHAR && _screen[x][1] == EMPTY_CHAR) { y = random(0, 2);}
  else if (_screen[x][0] == EMPTY_CHAR) { y = 0; }
  else if (_screen[x][1] == EMPTY_CHAR) { y = 1; }
  else  
  {
    Serial.println("no space to spawn");
    return false; //both positions are full
  }

  _enemies[index].Position.x = x;
  _enemies[index].Position.y = y;
  _enemies[index].Alive = true;

  //Serial.println(String("Spawn Enemy at x: ") + x + ", y: " + y);
  _screen[x][y] = ENEMY_CHAR;

  _enemySpawnCounter = (uint8_t)random(SPAWN_MIN_TICKS, SPAWN_MAX_TICKS + 1); //_currentSpawnInterval;
}
