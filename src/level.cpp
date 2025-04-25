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
  std::vector<std::pair<Vector2, int>> openCells;
  std::vector<Vector2> markedCells;

  inline bool isOutOfBounds(Vector2 pos) const {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  void setCell(Vector2 pos, int value) { 
    openCells.emplace_back(pos, value); 
  }

public:
  virtual bool update() = 0;
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;
  virtual int getCell(Vector2 pos) const = 0;
  virtual ~ILevel() = default;

  bool isMarked(Vector2 pos) const {
    return std::find(markedCells.begin(), markedCells.end(), pos) != markedCells.end();
  }

  int getSize() const { return size; }
  std::optional<int> getBombCount() const { return bombCount; }

  virtual std::vector<Vector2> getUnkownAdjacent(Vector2 pos) const {
    std::vector<Vector2> unkowns;
    for (auto &direction : Vector2::AllDirections()) {
      Vector2 newPos = pos + direction;
      if (!isOutOfBounds(newPos) && getCell(newPos) == TILE_UNKOWN && !isMarked(newPos))
        unkowns.push_back(newPos);
    }
    return unkowns;
  }

  const std::vector<std::pair<Vector2, int>> &getOpenCells() const {
    return openCells;
  }

  friend std::ostream &operator<<(std::ostream &os, const ILevel &level) {
    os << "  ";
    for (int i = 0; i < level.size; i++)
      os << i << " ";
    os << '\n';
    for (int i = 0; i < level.size; i++) {
      os << i << " ";
      for (int j = 0; j < level.size; j++) {
        Vector2 pos = {i, j};
        int tile = level.getCell(pos);
        if (tile == TILE_UNKOWN) {
          if (level.isMarked(pos))
            os << "B ";
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
  int getMarkedCount() const {
    return markedCells.size();
}
};

class InputLevel : public ILevel {
private:
  Matrix2D<int> playingField;
  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

public:
  InputLevel(int size, int bombs) : playingField(size, size, TILE_UNKOWN) {
    this->size = size;
    this->bombCount = bombs < 0 ? std::nullopt : std::make_optional(bombs);
  }

  static ILevel *create() {
    int bombs, size, openCellsCount;
    std::cin >> size >> bombs >> openCellsCount;

    InputLevel *level = new InputLevel(size, bombs);
    for (int i = 0; i < openCellsCount; i++) {
      Vector2 pos;
      int num;
      std::cin >> pos.y >> pos.x >> num;
      level->setCell(pos, num);
      level->playingField.at(pos) = num;
    }
    return level;
  }

  void mark(Vector2 pos) override {
    if (!isOutOfBounds(pos) && getCell(pos) == TILE_UNKOWN && !isMarked(pos)) {
      markedCells.push_back(pos);
      queuedActions.emplace_back(pos, Action::MARK);
    }
  }

  void probe(Vector2 pos) override {
    if (!isOutOfBounds(pos) && !isMarked(pos)) {
      queuedActions.emplace_back(pos, Action::PROBE);
    }
  }

  bool update() override {

    std::cout << queuedActions.size() << '\n';
    for (auto& action : queuedActions) {
        std::cout << action.first.y << ' ' << action.first.x << ' '
                 << (action.second == Action::PROBE ? 'A' : 'B') << '\n';
    }
    queuedActions.clear();
    std::cout.flush();

    int openCellsCount;
    std::cin >> openCellsCount;
    
    for (int i = 0; i < openCellsCount; i++) {
        Vector2 pos;
        int num;
        std::cin >> pos.y >> pos.x >> num;
        setCell(pos, num);
        playingField[pos] = num;
    }
    
    return openCellsCount > 0;
}

  int getCell(Vector2 pos) const override { 
    return playingField.at(pos); 
  }
};

class GeneratedLevel : public ILevel {
  private:
      Matrix2D<int> playingField;
      Matrix2D<char> discovered;  // Alterado para char para evitar problemas com vector<bool>
  
      void revealCells(Vector2 pos) {
          if (isOutOfBounds(pos) || discovered.at(pos)) return;
  
          discovered.at(pos) = 1;  // Usando 1 para true
          openCells.emplace_back(pos, playingField.at(pos));
  
          if (playingField.at(pos) == 0) {
              for (auto &direction : Vector2::AllDirections()) {
                  revealCells(pos + direction);
              }
          }
      }
  
  public:
      GeneratedLevel(int size, int bombCount)
          : playingField(size, size, 0), discovered(size, size, 0) {
          if (bombCount <= 0) throw std::runtime_error("Bombcount must be positive");
          this->size = size;
          this->bombCount = bombCount;
  
          // Colocar bombas
          int placed = 0;
          while (placed < bombCount) {
              Vector2 pos = Vector2::getRandom(size, size);
              if (playingField.at(pos) >= 0) {
                  playingField.at(pos) = -1; // -1 representa bomba
                  placed++;
              }
          }
  
          // Calcular números
          for (int y = 0; y < size; y++) {
              for (int x = 0; x < size; x++) {
                  if (playingField.at({x,y}) >= 0) {
                      for (auto &dir : Vector2::AllDirections()) {
                          Vector2 adj = Vector2{x,y} + dir;
                          if (!isOutOfBounds(adj) && playingField.at(adj) == -1) {
                              playingField.at({x,y})++;
                          }
                      }
                  }
              }
          }
      }
  

  void mark(Vector2 pos) override {
    if (!isOutOfBounds(pos) && !discovered.at(pos)) {
      markedCells.push_back(pos);
    }
  }

  void probe(Vector2 pos) override {
    if (isOutOfBounds(pos)) return;
    
    if (playingField.at(pos) == -1) {
      throw std::runtime_error("Bomba encontrada em " + std::to_string(pos.x) + "," + std::to_string(pos.y));
    }
    revealCells(pos);
  }

  bool update() override {
    return true;
  }

  int getCell(Vector2 pos) const override {
    return discovered.at(pos) ? playingField.at(pos) : TILE_UNKOWN;
  }
};
