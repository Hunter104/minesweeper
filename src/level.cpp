#include <cerrno>
#include <ostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cerrno>
#include <istream>
#include <optional>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include "vector2.cpp"
#pragma once

class Level {
private:
  int size;
  std::istream &in;
  std::ostream &out;

  std::optional<int> bombCount;
  std::vector<std::vector<int>> playingField;
  std::vector<std::pair<Vector2, int>> openCells;

  enum action { PROBE, MARK };
  std::vector<std::pair<Vector2, action>> queuedActions;

  void setCell(Vector2 pos, int value) {
    openCells.push_back({pos, value});
    playingField[pos.x][pos.y] = value;
  }

public:
  Level(std::istream& in, std::ostream& out)
  : in(in), out(out)
  {
    int bombs, openCellsCount;
    in >> size >> bombs >> openCellsCount;
    bombCount = bombs < 0 ? std::nullopt : std::optional(bombs);
    playingField = std::vector<std::vector<int>>(size, std::vector<int>(size, -1));

    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      setCell(pos, num);
    }
  }

  void mark(Vector2 pos) {
    queuedActions.push_back({pos, MARK});
  }

  void probe(Vector2 pos) {
    queuedActions.push_back({pos, PROBE});
  }

  // TODO: how to deal with end condition
  void update() {
    // Print out current actions
    int actionCount = queuedActions.size();
    out << "actionCount\n";
    for (auto& action : queuedActions) {
      out << action.first.y << ' ' << action.first.x << ' ';
      if (action.second == PROBE)
        out << 'A';
      else 
        out << 'B';
      out << '\n';
    }
    queuedActions.resize(0);

    // Receive next board state
    int openCellsCount;
    in >> openCellsCount;
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      setCell(pos, num);
    }
  }

  inline const std::vector<std::pair<Vector2, int>>& getOpenCells() const {
    return openCells;
  }

  inline const int getSize() const {
    return size;
  }

  friend std::ostream& operator<<(std::ostream& os, const Level& level) {
    for (int i=0; i<level.size; i++) {
      for (int j=0; j<level.size; j++) {
        int tile = level.playingField[i][j];
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

