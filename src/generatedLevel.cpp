#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <optional>
#include <stdexcept>
#include <unistd.h>
#include <unordered_set>
#include <vector>

class GeneratedLevel : public Level {
private:
  bool updated = false;

  void revealCells(Vector2 pos) {
    if (isOutOfBounds(pos) || playingField[pos].discovered)
      return;

    playingField[pos].discovered = true;
    newOpenCells.emplace_back(pos, playingField[pos].num);

    if (playingField[pos].num != 0)
      return;

    for (auto &direction : directions) {
      revealCells(direction + pos);
    }
  }

public:
  GeneratedLevel(int size, int bombCount) : Level(size, bombCount) {
    if (bombCount >= size * size)
      throw std::invalid_argument(
          "Bomb count higher than or equal to tile count.");
    if (bombCount <= 0)
      throw std::runtime_error("Bomb count must be 1 or higher");

    Vector2 initial_probe = Vector2::getRandom(size, size);
    std::unordered_set<Vector2> excluded;
    excluded.insert(initial_probe);
    std::unordered_set<Vector2> bombPositions;
    for (const auto &adjacent : directions) {
      Vector2 adjPos = initial_probe + adjacent;
      if (!isOutOfBounds(adjPos)) {
        excluded.insert(adjPos);
      }
    }

    if (bombCount > size * size - static_cast<int>(excluded.size())) {
      throw std::logic_error(
          "More bombs than avaliable spaces in level generation");
    }
    int placed = 0;
    while (placed < bombCount) {
      Vector2 pos = Vector2::getRandom(size, size);
      if (playingField[pos].hasBomb || excluded.find(pos) != excluded.end())
        continue;

      playingField[pos].num = -50;
      playingField[pos].hasBomb = true;
      bombPositions.insert(pos);
      placed++;

      for (const auto &adjacent : directions) {
        Vector2 adjPos = pos + adjacent;
        if (!isOutOfBounds(adjPos)) {
          playingField[adjPos].num++;
        }
      }
    }
    probe(initial_probe);
  }

  void update() override {}

  void mark(Vector2 pos) override {
#ifdef DEBUG
    std::cout << pos << " marked" << std::endl;
#endif
    playingField[pos].marked = true;
  }

  void probe(Vector2 pos) override {
    if (playingField[pos].hasBomb)
      throw std::runtime_error("Position " + std::to_string(pos.x) + "," +
                               std::to_string(pos.y) + " has a bomb.");

    revealCells(pos);
  }
};
