#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "vector2.cpp"
#include <iostream>
#include <optional>
#include <ostream>
#include <queue>
#include <unistd.h>
#include <utility>
#include <vector>
#define BOMB 'B'
#define CLEAR 'A'

class InputLevel : public Level {
private:
  std::queue<std::pair<Vector2, char>> queuedActions;

  InputLevel(int size, int bombs)
      : Level(size, bombs < 0 ? std::nullopt : std::make_optional(bombs)) {}

  void getNewlyOpenedCells() {
    int openCellsCount;
    std::cin >> openCellsCount;
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos >> num;
      newOpenCells.emplace_back(pos, num);
      playingField[pos].num = num;
      playingField[pos].discovered = true;
    }
  }

public:
  static Level *create() {
    int bombs, size;
    std::cin >> size >> bombs;

    InputLevel *level = new InputLevel(size, bombs);
    level->getNewlyOpenedCells();

    return level;
  }

  void mark(Vector2 pos) override {
    playingField[pos].marked = true;
    queuedActions.push({pos, BOMB});
  }

  void probe(Vector2 pos) override { queuedActions.push({pos, CLEAR}); }

  void update() override {
    int actionCount = queuedActions.size();
    std::cout << actionCount << '\n';

    while (!queuedActions.empty()) {
      auto action = queuedActions.front();
      queuedActions.pop();
      std::cout << action.first.y << ' ' << action.first.x << ' '
                << action.second << '\n';
    }
    std::cout << std::flush;
    getNewlyOpenedCells();
  }
};
