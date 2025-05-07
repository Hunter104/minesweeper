#pragma once

#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <utility>
#include <vector>

constexpr int TILE_UNKNOWN = -1;

class Level {
public:
  struct Tile {
    int num = 0;
    bool discovered = false;
    bool marked = false;
  };

  virtual ~Level() = default;

  virtual bool update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;

  int getSize() const { return size; }
  std::optional<int> getBombCount() const { return bombCount; }

  std::vector<Vector2> getUnkownAdjacent(Vector2 pos) const {
    std::vector<Vector2> result;
    for (const auto &dir : Vector2::AllDirections()) {
      Vector2 newPos = pos + dir;
      if (!isOutOfBounds(newPos) && !playingField[newPos].discovered)
        result.push_back(newPos);
    }
    return result;
  }

  std::vector<Vector2> getAllUnknowns() const {
    std::vector<Vector2> result;
    for (int i = 0; i < size; ++i) {
      for (int j = 0; j < size; ++j) {
        if (!playingField[{i, j}].discovered && !playingField[{i, j}].marked)
          result.emplace_back(i, j);
      }
    }
    return result;
  }

  std::vector<std::pair<Vector2, int>> getOpenCells() {
    return std::exchange(newOpenCells, {});
  }

  friend std::ostream &operator<<(std::ostream &os, const Level &level) {
    os << "  ";
    for (int i = 0; i < level.size; ++i)
      os << i << " ";
    os << '\n';

    for (int i = 0; i < level.size; ++i) {
      os << i << " ";
      for (int j = 0; j < level.size; ++j) {
        const auto &tile = level.playingField[{i, j}];
        if (!tile.discovered)
          os << (tile.marked ? "M " : "# ");
        else if (tile.num == 0)
          os << ". ";
        else
          os << tile.num << ' ';
      }
      os << '\n';
    }

    return os;
  }

  const Tile getTile(Vector2 pos) const { return playingField[pos]; }

protected:
  int size;
  std::optional<int> bombCount;
  Matrix2D<Tile> playingField;
  std::vector<std::pair<Vector2, int>> newOpenCells;

  Level(int s = 0, std::optional<int> bc = std::nullopt)
      : size(s), bombCount(bc), playingField(size, size) {}

  bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }
};
