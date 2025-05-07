#pragma once
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <unistd.h>
#include <utility>
#include <vector>

constexpr int TILE_UNKNOWN = -1;

class Level {
protected:
  struct Tile {
    int num = 0;
    bool discovered = false;
    bool marked = false;
  };
  int size;
  std::optional<int> bombCount;
  Matrix2D<Tile> playingField;
  std::vector<std::pair<Vector2, int>> newOpenCells;

  Level(int s = 0, std::optional<int> bc = std::nullopt)
      : size(s), bombCount(bc), playingField(size, size) {}

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

public:
  virtual bool update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;
  virtual ~Level() = default;

  int getSize() const { return size; }
  std::optional<int> getBombCount() const { return bombCount; }

  std::vector<Vector2> getUnkownAdjacent(Vector2 pos) const {
    std::vector<Vector2> unkowns;
    for (auto &direction : Vector2::AllDirections()) {
      Vector2 newPos = pos + direction;
      if (!isOutOfBounds(newPos) && !playingField[newPos].discovered)
        unkowns.push_back(newPos);
    }

    return unkowns;
  }

  std::vector<Vector2> getAllUnknowns() const {
    std::vector<Vector2> unkowns;
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (!playingField[{i, j}].discovered)
          unkowns.emplace_back(i, j);
      }
    }

    return unkowns;
  }

  const std::vector<std::pair<Vector2, int>> getOpenCells() {
    return std::exchange(newOpenCells, {});
  }

  friend std::ostream &operator<<(std::ostream &os, const Level &level) {
    std::cout << "  ";
    for (int i = 0; i < level.size; i++)
      std::cout << i << " ";
    std::cout << '\n';
    for (int i = 0; i < level.size; i++) {
      std::cout << i << " ";
      for (int j = 0; j < level.size; j++) {
        Vector2 pos = {i, j};
        Tile tile = level.playingField[pos];
        if (!tile.discovered) {
          os << (tile.marked ? "M " : "# ");
        } else if (tile.num == 0)
          os << ". ";
        else
          os << tile.num << ' ';
      }
      os << '\n';
    }
    return os;
  }
};
;
;
