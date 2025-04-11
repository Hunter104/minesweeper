#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <istream>
#include <optional>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include "vector2.cpp"
#pragma once

class Level {
private:
  int size;
  std::optional<int> bombCount;
  std::vector<std::vector<int>> playingField;
public:
  Level(int size, int bombCount)
    : size(size),
      bombCount(bombCount >= 0 ? std::optional<int>{bombCount} : std::nullopt),
      playingField(size, std::vector<int>(size, -1))
  {}

  void setCell(Vector2 pos, int value) {
    playingField[pos.x][pos.y] = value;
  }

  void updateFromStream(std::istream& in) {
    int openCellsCount;
    in >> openCellsCount;
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      setCell(pos, num);
    }
  }

  static Level createFromStream(std::istream& in) {
    int size, bombs, openCellsCount;
    in >> size >> bombs >> openCellsCount;

    Level level(size, bombs);
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      level.setCell(pos, num);
    }

    return level;
  }

  friend std::ostream& operator<<(std::ostream& os, const Level& level) {
    for (int i=0; i<level.size; i++) {
      for (int j=0; j<level.size; j++) {
        int tile = level.playingField[i][j];
        if (tile == -1)
          os << "# ";
        else if (tile == 0)
          os << ". ";
        else 
          os << tile << ' ';
      }
      os << '\n';
    }
    return os;
  }
  
};

