#pragma once

namespace CPPLib
{
  namespace Common
  {
    struct Vector2Int {
      int x;
      int y;

      Vector2Int(): x(0), y(0) {}
      Vector2Int(int x, int y): x(x), y(y) {}
    };
  }
}