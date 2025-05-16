#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <unistd.h>
#include <unordered_set>
#include <utility>
#include <vector>

class InputLevel : public Level {
private:
  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

  void setCell(Vector2 pos, int value) {
    newOpenCells.emplace_back(pos, value);
  }

  InputLevel(int size, int bombs)
      : Level(size, bombs < 0 ? std::nullopt : std::make_optional(bombs)) {}

public:
  static Level *create() {
    int bombs, size, openCellsCount;
    std::cin >> size >> bombs >> openCellsCount;

    InputLevel *level = new InputLevel(size, bombs);
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos >> num;
      level->setCell(pos, num);
      level->playingField[pos].num = num;
    }

    return level;
  }

  void mark(Vector2 pos) override {
    playingField[pos].marked = true;
    queuedActions.emplace_back(pos, Action::MARK);
  }

  void probe(Vector2 pos) override {
    queuedActions.emplace_back(pos, Action::PROBE);
  }

  void update() override {
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

    int openCellsCount;
    std::cin >> openCellsCount;
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos >> num;
      setCell(pos, num);
      playingField[pos].num = num;
    }
  }
};
