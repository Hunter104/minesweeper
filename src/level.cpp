#pragma once
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <vector>

#include "matrix2d.cpp"
#include "vector2.cpp"

constexpr int TILE_UNKOWN = -1;

class ILevel {
protected:
  int size;
  std::optional<int> bombCount;
  std::vector<std::pair<Vector2, int>> newOpenCells;
  std::vector<Vector2> markedCells; // NEW: track marked positions

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  void setCell(Vector2 pos, int value) { newOpenCells.emplace_back(pos, value); }

  bool isMarked(Vector2 pos) const { // NEW: check if cell is marked
    return std::find(markedCells.begin(), markedCells.end(), pos) != markedCells.end();
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
    for (int i=0; i<size; i++) {
      for (int j=0; j<size; j++) {
        if (getCell({i, j}) < 0)
          unkowns.emplace_back(i, j);
      }
    }

    return unkowns;
  }

  const std::vector<std::pair<Vector2, int>> getOpenCells() {
    auto result = std::move(newOpenCells);
    return result;
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
        if (tile == TILE_UNKOWN) {
          if (level.isMarked(pos))
            os << "📍";
          else
            os << "# ";
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

class InputLevel : public ILevel {
private:
  Matrix2D<int> playingField;

  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

  void setCell(Vector2 pos, int value) { newOpenCells.emplace_back(pos, value); }

  InputLevel(int size, int bombs) : playingField(size, size, TILE_UNKOWN) {
    this->size = size;
    this->bombCount = bombs < 0 ? std::nullopt : std::make_optional(bombs);
  }

public:
  static ILevel *create() {
    int bombs, size, openCellsCount;
    std::cin >> size >> bombs >> openCellsCount;

    InputLevel *level = new InputLevel(size, bombs);
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos.y >> pos.x >> num;
      level->setCell(pos, num);
      level->playingField[pos] = num;
    }

    return level;
  }

  void mark(Vector2 pos) override {
    markedCells.push_back(pos); // NEW: mark it
    queuedActions.emplace_back(pos, Action::MARK);
  }

  void probe(Vector2 pos) override {
    queuedActions.emplace_back(pos, Action::PROBE);
  }

  bool update() override {
    int actionCount = queuedActions.size();
    std::cout << actionCount << '\n';
    if (actionCount == 0)
      return false;

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
      std::cin >> pos.y >> pos.x >> num;
      setCell(pos, num);
      playingField[pos] = num;
    }

    return true;
  }

  inline int getCell(Vector2 pos) const override { return playingField[pos]; }
};

class GeneratedLevel : public ILevel {
private:
  Matrix2D<int> playingField;
  Matrix2D<char> discovered;

  void revealCells(Vector2 pos) {
    if (isOutOfBounds(pos))
      return;

    if (playingField[pos] < 0)
      return;
    else if (playingField[pos] > 0) {
      discovered[pos] = true;
      newOpenCells.emplace_back(pos, playingField[pos]);
    } else if (playingField[pos] == 0 && !discovered[pos]) {
      newOpenCells.emplace_back(pos, playingField[pos]);
      discovered[pos] = true;
      for (auto &direction : Vector2::AllDirections()) {
        revealCells(direction + pos);
      }
    }
  }

public:
  GeneratedLevel(int size, int bombCount)
      : playingField(size, size, 0), discovered(size, size, 0) {
    if (bombCount <= 0)
      throw std::runtime_error("Bombcount must be 1 or higher");
    this->size = size;
    this->bombCount = bombCount;

    Vector2 initial_probe = Vector2::getRandom(size, size);

    int placed = 0;
    while (placed < bombCount) {
      Vector2 pos = Vector2::getRandom(size, size);
      if (pos == initial_probe || playingField[pos] < -1)
        continue;

      playingField[pos] = -50;
      placed++;

      for (auto &adjacent : Vector2::AllDirections()) {
        if (!isOutOfBounds(pos + adjacent)) {
          playingField[pos + adjacent]++;
        }
      }
    }
    probe(initial_probe);
  }

  bool update() override {
    return true;
  }

  void mark(Vector2 pos) override {
    markedCells.push_back(pos); // NEW: track marked pos
  }

  void probe(Vector2 pos) override {
    if (playingField[pos] < 0) {
      std::ostringstream text;
      text << "Position " << pos << " has a bomb.";
      throw std::runtime_error(text.str());
    }

    revealCells(pos);
  }

  int getCell(Vector2 pos) const override {
    return discovered[pos] ? playingField[pos] : TILE_UNKOWN;
  }
};
