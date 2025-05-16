#pragma once
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iomanip>
#include <iostream>
#include <optional>
#include <unistd.h>
#include <utility>
#include <vector>

#define RESET "\033[0m"
#define RED "\033[31m"
#define BRIGHT_RED "\033[91m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define BRIGHT_MAGENTA "\033[95m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define DIM "\033[2m"

struct Tile {
  int num = 0;
  bool marked = false;
  bool discovered = false;
  bool hasBomb = false;
};

class Level {
public:
  const int size;
  const std::optional<int> bombCount;

  virtual void update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;
  virtual ~Level() = default;

  void getUnknownAdjacent(Vector2 pos, std::vector<Vector2> &result) const {
    result.clear();
    for (auto &direction : directions) {
      Vector2 newPos = pos + direction;
      if (!isOutOfBounds(newPos) && !playingField[newPos].discovered) {
        result.push_back(newPos);
      }
    }
  }

  std::vector<std::pair<Vector2, int>> getOpenCells() {
    return std::exchange(newOpenCells, {});
  }

  friend std::ostream &operator<<(std::ostream &os, const Level &level) {
    os << "    ";
    for (int i = 0; i < level.size; i++) {
      os << std::setw(3) << i;
    }
    os << '\n';

    for (int i = 0; i < level.size; i++) {
      os << std::setw(3) << i;
      for (int j = 0; j < level.size; j++) {
        Tile tile = level.playingField[{i, j}];
        std::string content;

        if (!tile.discovered) {
          if (tile.marked) {
            content = std::string(RED) + "█ " + RESET;
          } else {
            content = std::string(DIM) + "▓ " + RESET;
          }
        } else if (tile.hasBomb) {
          content = std::string(RED) + "B " + RESET;
        } else if (tile.num == 0) {
          content = std::string(WHITE) + "◦ " + RESET;
        } else {
          switch (tile.num) {
          case 1:
            content =
                std::string(BLUE) + std::to_string(tile.num) + " " + RESET;
            break;
          case 2:
            content =
                std::string(GREEN) + std::to_string(tile.num) + " " + RESET;
            break;
          case 3:
            content = std::string(BRIGHT_RED) + std::to_string(tile.num) + " " +
                      RESET;
            break;
          case 4:
            content =
                std::string(MAGENTA) + std::to_string(tile.num) + " " + RESET;
            break;
          case 5:
            content =
                std::string(YELLOW) + std::to_string(tile.num) + " " + RESET;
            break;
          default:
            content =
                std::string(CYAN) + std::to_string(tile.num) + " " + RESET;
            break;
          }
        }
        os << content;
      }
      os << '\n';
    }
    return os;
  } // PERF: may not be performant
  inline bool isMarked(Vector2 pos) { return playingField[pos].marked; }

  // NOTE: for debug use only
  inline Tile getTile(Vector2 pos) { return playingField[pos]; }

protected:
  std::vector<std::pair<Vector2, int>> newOpenCells;
  Matrix2D<Tile> playingField;

  Level(int s = 0, std::optional<int> bc = std::nullopt)
      : size(s), bombCount(bc), playingField(s, s) {}

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  const std::vector<Vector2> directions = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                           {0, 1},   {1, -1}, {1, 0},  {1, 1}};
};
