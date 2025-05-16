#pragma once
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iomanip>
#include <iostream>
#include <optional>
#include <unistd.h>
#include <utility>
#include <vector>

constexpr int TILE_UNKNOWN = -1;

struct Tile {
  int num = 0;
  bool marked = false;
  bool discovered = false;
  bool hasBomb = false;
};

class Level {
protected:
  int size;
  std::optional<int> bombCount;
  std::vector<std::pair<Vector2, int>> newOpenCells;
  Matrix2D<Tile> playingField;

  Level(int s = 0, std::optional<int> bc = std::nullopt)
      : size(s), bombCount(bc), playingField(s, s) {}

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  void setCell(Vector2 pos, int value) {
    newOpenCells.emplace_back(pos, value);
  }

  const std::vector<Vector2> directions = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                           {0, 1},   {1, -1}, {1, 0},  {1, 1}};

public:
  virtual void update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;
  virtual ~Level() = default;

  int getSize() const { return size; }
  std::optional<int> getBombCount() const { return bombCount; }

  std::vector<Vector2> getUnknownAdjacent(Vector2 pos) const {
    std::vector<Vector2> unkowns;
    for (auto &direction : directions) {
      Vector2 newPos = pos + direction;
      if (!isOutOfBounds(newPos) && !playingField[newPos].discovered)
        unkowns.push_back(newPos);
    }

    return unkowns;
  }

  std::vector<std::pair<Vector2, int>> getOpenCells() {
    return std::exchange(newOpenCells, {});
  }

  friend std::ostream &operator<<(std::ostream &os, const Level &level) {
    std::cout << std::setw(3) << " ";
    for (int i = 0; i < level.size; i++)
      std::cout << std::setw(3) << i;
    std::cout << '\n';

    for (int i = 0; i < level.size; i++) {
      std::cout << std::setw(3) << i;
      for (int j = 0; j < level.size; j++) {
        Tile tile = level.playingField[{i, j}];
        std::string content;
        if (!tile.discovered)
          content = tile.marked ? "M" : "#";
        else if (tile.num == 0)
          content = ".";
        else
          content = std::to_string(tile.num);
        os << std::setw(3) << content;
      }
      os << '\n';
    }
    return os;
  }
};
