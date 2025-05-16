#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "solver.cpp"
#include "vector2.cpp"
#include <stdexcept>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Agent {
private:
  Level *level;
  Solver solver;
  Matrix2D<int> hasBombVariables;
  std::unordered_map<int, Vector2> inverseLookup;
  std::unordered_set<Vector2> alreadyActedTiles;
  std::unordered_set<Vector2> toVisit;
  int foundBombCount = 0;

public:
  Agent(Level *level)
      : level(level), hasBombVariables(level->size, level->size, -1) {
    int mapSize = level->size;
    for (int x = 0; x < mapSize; x++) {
      for (int y = 0; y < mapSize; y++) {
        int variable = solver.addVariable();
        hasBombVariables[x][y] = variable;
        inverseLookup[variable - 1] = {x, y};
      }
    }
  }

  /* Query bomb existence in tile,
  second parameter indicates wether we query
  its existence or lack thereof */
  bool checkBomb(Vector2 tile, bool exists = true) {
    int var = hasBombVariables[tile];
    var = exists ? var : -var;

    if (solver.solve(-var) == SolverStatus::SATISFIABLE)
      return false;
    solver.addClause(var);
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const Agent &kb) {
    os << kb.solver;
    return os;
  }

  bool decide() {
    if (level->bombCount.has_value() &&
        level->bombCount.value() == foundBombCount) {
#ifdef DEBUG
      std::cout << "Found all bombs, stopping." << std::endl;
#endif
      return false;
    }

    const auto newOpenCells = level->getOpenCells();

    std::vector<Vector2> currentUnkowns;
    // Max 9 adjacent tiles
    currentUnkowns.reserve(9);

    // TODO: adicionar thread pool para otimização de cláusuluas
    for (auto &[position, value] : newOpenCells) {
      solver.addClause(-hasBombVariables[position]);
      toVisit.erase(position);
      if (value == 0)
        continue;

      // Get variables for adjacent unkown spaces
      std::vector<int> variables;
      level->getUnknownAdjacent(position, currentUnkowns);
      for (auto &adjacent : currentUnkowns) {
        variables.push_back(hasBombVariables[adjacent]);
        if (!level->isMarked(adjacent))
          toVisit.insert(adjacent);
      }

      if (variables.empty() && value > 0)
        throw std::logic_error(
            "Contradiction: Cell " + std::to_string(position.x) + "," +
            std::to_string(position.y) + " shows " + std::to_string(value) +
            " bombs but has no unknown neighbors");

      solver.addCardinalityConstraint(variables, value);
    }

    // TODO: adicionar thread pool para otimização de verificação
    bool madeProgress = false;
    for (auto it = toVisit.begin(); it != toVisit.end();) {
      Vector2 pos = *it;
      if (alreadyActedTiles.find(pos) != alreadyActedTiles.end()) {
        it++;
        continue;
      }
      if (checkBomb(pos)) {
        level->mark(pos);
        foundBombCount++;
        madeProgress = true;
        it = toVisit.erase(it);
        alreadyActedTiles.insert(pos);
      } else if (checkBomb(pos, false)) {
        level->probe(pos);
        madeProgress = true;
        it = toVisit.erase(it);
        alreadyActedTiles.insert(pos);
      } else {
        ++it;
      }
    }

#ifdef DEBUG
    if (!madeProgress) {
      std::cout << "Couldn't make any more progress on tiles: \n";
      for (auto pos : toVisit) {
        std::cout << pos << ' ';
      }
      std::cout << std::endl;
      std::cout << *level;
    }
#endif // DEBUG
    return madeProgress;
  }
};
