#pragma once
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <optional>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

#include "vector2.cpp"
#include "matrix2d.cpp"

class ILevel {
protected:
  int size;
  std::optional<int> bombCount;
  std::vector<std::pair<Vector2, int>> openCells;

public:
  virtual void update() = 0;

  // TODO: loading actions directly might be more efficient
  virtual void mark(Vector2 pos) = 0;
  virtual void probe(Vector2 pos) = 0;

  virtual int getCell(Vector2 pos) const = 0;

  virtual ~ILevel() = default;

  virtual int getSize() const {
    return size;
  }

  std::optional<int> getBombCount() const {
    return bombCount;
  }

  const std::vector<std::pair<Vector2, int>>& getOpenCells() const {
    return openCells;
  }

  friend std::ostream& operator<<(std::ostream& os, const ILevel& level) {
    for (int i=0; i<level.size; i++) {
      for (int j=0; j<level.size; j++) {
        int tile = level.getCell({i, j}); 
        if (tile == -1)
          os << "# ";
        else if (tile == 0)
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
  std::istream &in;
  std::ostream &out;
  Matrix2D<int> playingField;

  enum class Action { PROBE, MARK };
  std::vector<std::pair<Vector2, Action>> queuedActions;

  void setCell(Vector2 pos, int value) {
    openCells.push_back({pos, value});
    playingField[pos] = value;
  }

  InputLevel(std::istream& in, std::ostream& out, int size, int bombs)
  : in(in), out(out), playingField(size, size, -1) {
    this->size = size;
    this->bombCount = bombs < 0 ? std::nullopt : std::make_optional(bombs);
  }
public:

  static InputLevel* create(std::istream& in, std::ostream& out) {
    int bombs, size, openCellsCount;
    in >> size >> bombs >> openCellsCount;

    InputLevel *level = new InputLevel(in, out, size, bombs);
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos.y >> pos.x >> num;
      level->setCell(pos, num);
    }
    
    return level;
  }

  void mark(Vector2 pos) override { 
    queuedActions.push_back({pos, Action::MARK});
  }

  void probe(Vector2 pos) override {
    queuedActions.push_back({pos, Action::PROBE});
  }

  // TODO: how to deal with end condition
  void update() override {
    // Print out current actions
    int actionCount = queuedActions.size();
    out << actionCount << '\n';
    for (auto& action : queuedActions) {
      out << action.first.y << ' ' << action.first.x << ' ';
      if (action.second == Action::PROBE)
        out << 'A';
      else 
        out << 'B';
      out << '\n';
    }
    queuedActions.clear();

    // Receive next board state
    int openCellsCount;
    in >> openCellsCount;
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos.y >> pos.x >> num;
      setCell(pos, num);
    }
  }

  
  inline int getCell(Vector2 pos) const override {
    return playingField[pos];
  }
};

class GeneratedLevel : public ILevel {
private:
  
  // Number of bombs around tile, if negative there is a bomb in the tile
  Matrix2D<int> playingField;
  Matrix2D<char> discovered;

  inline bool isOutOfBounds(Vector2 pos) {
    return pos.x < 0 || pos.y < 0 || pos.x >= size || pos.y >= size;
  }

  void revealCells(Vector2 pos) {
    if (isOutOfBounds(pos))
      return;

    if (playingField[pos] < 0)
      return;
    else if (playingField[pos] > 0)
        discovered[pos] = true;
    else if (playingField[pos] == 0 && !discovered[pos]) {
      openCells.push_back({pos, playingField[pos]});
      discovered[pos] = true;
      for (auto& direction : Vector2::AllDirections()) {
        revealCells(direction+pos);
      }
    }
  }
  
public:
  GeneratedLevel(int size, int bombCount) :
  playingField(size, size, 0),
  discovered(size, size, 0)
  {
    if (bombCount <= 0)
      throw std::runtime_error("Bombcount must be 1 or higher");
    this->size = size;
    this->bombCount = std::optional(bombCount);

    Vector2 initial_probe = Vector2::getRandom(size, size);
    openCells.push_back({initial_probe, playingField[initial_probe]});

    int placed = 0;
    while (placed < bombCount) {
      Vector2 pos = Vector2::getRandom(size, size);
      if (pos == initial_probe || playingField[pos] == -1) 
        continue;

      playingField[pos] = -20;
      placed++;

      for (auto& adjacent : Vector2::AllDirections()) {
          if (!isOutOfBounds(pos+adjacent)) {
            playingField[pos+adjacent]++;
          } 
      }
    }

    probe(initial_probe);
  }

  void update() override {
    std::cout << "Updated\n";
  }

  void mark(Vector2 pos) override {
    std::cout << "Marked: " << pos << '\n';
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
    return discovered[pos] ? playingField[pos] : -1;
  }
};
