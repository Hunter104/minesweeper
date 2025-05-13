#pragma once
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <unistd.h>
#include <unordered_set>
#include <utility>
#include <vector>

constexpr int TILE_UNKNOWN = -1;

class ILevel {
protected:
  int size;
  std::optional<int> bombCount;
  std::vector<std::pair<Vector2, int>> newOpenCells;
  std::unordered_set<Vector2> markedCells;

  ILevel(int s = 0, std::optional<int> bc = std::nullopt)
      : size(s), bombCount(bc) {}

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  void setCell(Vector2 pos, int value) {
    newOpenCells.emplace_back(pos, value);
  }

  bool isMarked(Vector2 pos) const {
    return markedCells.find(pos) != markedCells.end();
  }

public:
  virtual bool update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;
  virtual int getCell(Vector2 pos) const = 0;
  virtual ~ILevel() = default;

  int getSize() const { return size; }
  std::optional<int> getBombCount() const { return bombCount; }

  std::vector<Vector2> getUnkownAdjacent(Vector2 pos) const {
    std::vector<Vector2> unkowns;
    for (auto &direction : Vector2::AllDirections()) {
      Vector2 newPos = pos + direction;
      if (!isOutOfBounds(newPos) && getCell(newPos) < 0)
        unkowns.push_back(newPos);
    }

    return unkowns;
  }

  std::vector<Vector2> getAllUnknowns() const {
    std::vector<Vector2> unkowns;
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (getCell({i, j}) < 0)
          unkowns.emplace_back(i, j);
      }
    }

    return unkowns;
  }

  const std::vector<std::pair<Vector2, int>> getOpenCells() {
    return std::exchange(newOpenCells, {});
  }

  friend std::ostream &operator<<(std::ostream &os, const ILevel &level) {
    std::cout << "  ";
    for (int i = 0; i < level.size; i++)
      std::cout << i << " ";
    std::cout << '\n';
    for (int i = 0; i < level.size; i++) {
      std::cout << i << " ";
      for (int j = 0; j < level.size; j++) {
        Vector2 pos = {i, j};
        int tile = level.getCell(pos);
        if (tile == TILE_UNKNOWN) {
          os << (level.isMarked(pos) ? "M " : "# ");
        } else if (tile == 0)
          os << ". ";
        else
          os << tile << ' ';
      }
      os << '\n';
    }
    return os;
  }
};
;
