#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <unistd.h>
#include <utility>
#include <vector>

class InputLevel : public Level {
private:
  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

  InputLevel(int size, int bombs)
      : Level(size, bombs < 0 ? std::nullopt : std::make_optional(bombs)) {}

  void readOpenCells() {
    int openCellsCount;
    std::cin >> openCellsCount;
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos >> num;
      newOpenCells.emplace_back(pos, num);
      playingField[pos].num = num;
      playingField[pos].discovered = true;
      playingField[pos].num = num;
    }
  }

public:
  static Level *create() {
    int bombs, size;
    std::cin >> size >> bombs;

    InputLevel *level = new InputLevel(size, bombs);
    level->readOpenCells();

    return level;
  }

  void mark(Vector2 pos) override {
    playingField[pos].marked = true;
    queuedActions.emplace_back(pos, Action::MARK);
  }

  void probe(Vector2 pos) override {
    queuedActions.emplace_back(pos, Action::PROBE);
  }

  bool update() override {
    int actionCount = queuedActions.size();
    std::cout << actionCount << '\n';

    for (auto &action : queuedActions) {
      std::cout << action.first.y << ' ' << action.first.x << ' ';
      if (action.second == Action::PROBE)
        std::cout << 'A';
      else
        std::cout << 'B';
      std::cout << '\n';
    }
    queuedActions.clear();

    readOpenCells();

    return true;
  }
};
