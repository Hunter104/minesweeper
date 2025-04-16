#pragma once
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <optional>
#include <unistd.h>
#include <vector>

#include "matrix2d.cpp"
#include "vector2.cpp"

class Level {
private:
  int size;
  std::optional<int> bombCount;
  std::vector<std::pair<Vector2, int>> openCells;
  Matrix2D<int> playingField;

  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

  void setCell(Vector2 pos, int value) {
    openCells.push_back({pos, value});
    playingField[pos] = value;
  }

  Level(int size, int bombs)
      : playingField(size, size, -1) {
    this->size = size;
    this->bombCount = bombs < 0 ? std::nullopt : std::make_optional(bombs);
  }

  inline bool isOutOfBounds(Vector2 pos) {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

public:
  static Level create(std::istream &in, std::ostream &out) {
    int bombs, size, openCellsCount;
    in >> size >> bombs >> openCellsCount;

    Level level = Level(size, bombs);
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      in >> pos.y >> pos.x >> num;
      level.setCell(pos, num);
    }

    return level;
  }

  int getSize() const { return size; }

  void mark(Vector2 pos) {
    queuedActions.push_back({pos, Action::MARK});
  }

  void probe(Vector2 pos) {
    queuedActions.push_back({pos, Action::PROBE});
  }

  std::vector<std::pair<Vector2, int>> getOpenCells() const {
    return openCells;
  }

  virtual std::vector<Vector2> getUnkownAdjacent(Vector2 pos) {
    std::vector<Vector2> unkowns;
    for (auto& direction : Vector2::AllDirections()) {
      Vector2 newPos = pos +direction;
      if (!isOutOfBounds(newPos) && playingField[newPos] < 0)
        unkowns.push_back(newPos);
    }

    return unkowns;
  }

  friend std::ostream &operator<<(std::ostream &os, const Level &level) {
    for (int i = 0; i < level.size; i++) {
      for (int j = 0; j < level.size; j++) {
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

  bool update() {
    int actionCount = queuedActions.size();
    std::cout << actionCount << '\n';
    if (actionCount == 0)
      return false;

    for (auto &action : queuedActions) {
      std::cout << action.first.y << ' ' << action.first.x << ' ';
      if (action.second == Action::PROBE)
        std::cout << 'A';
      else
        std::cout << 'B';
      std::cout << '\n';
    }
    queuedActions.clear();

    // Receive next board state
    int openCellsCount;
    std::cin >> openCellsCount;
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos.y >> pos.x >> num;
      setCell(pos, num);
    }

    return true;
  }

};
