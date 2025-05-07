#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <unistd.h>
#include <unordered_set>

class GeneratedLevel : public Level {
private:
  bool updated = false;

  void revealCells(Vector2 pos) {
    if (isOutOfBounds(pos) || playingField[pos].discovered ||
        playingField[pos].num < 0)
      return;

    playingField[pos].discovered = true;
    newOpenCells.emplace_back(pos, playingField[pos].num);
    if (playingField[pos].num == 0) {
      for (auto &direction : Vector2::AllDirections()) {
        revealCells(direction + pos);
      }
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
    std::unordered_set<Vector2> bombPositions;

    int placed = 0;
    while (placed < bombCount) {
      Vector2 pos = Vector2::getRandom(size, size);
      if (pos == initial_probe || bombPositions.count(pos) > 0)
        continue;

      playingField[pos].num = -50;
      bombPositions.insert(pos);
      placed++;

      for (const auto &adjacent : Vector2::AllDirections()) {
        Vector2 adjPos = pos + adjacent;
        if (!isOutOfBounds(adjPos)) {
          playingField[adjPos].num++;
        }
      }
    }
    probe(initial_probe);
  }

  bool update() override {
    if (updated) {
      updated = false;
      return true;
    } else {
      return false;
    }
    return true;
  }

  void mark(Vector2 pos) override { playingField[pos].marked = true; }

  void probe(Vector2 pos) override {
    updated = true;
    if (playingField[pos].num < 0)
      throw std::runtime_error("Position " + std::to_string(pos.x) + "," +
                               std::to_string(pos.y) + " has a bomb.");

    revealCells(pos);
  }
};
