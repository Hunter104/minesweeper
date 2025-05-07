#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "solver.cpp"
#include "vector2.cpp"
#include <map>
#include <set>
#include <stdexcept>
#include <unistd.h>
#include <unordered_set>
#include <vector>

// Função geradora de combinações
void _generateCombinations(const std::vector<int> &variables, int r,
                           std::vector<std::vector<int>> &result,
                           std::vector<int> &current, int start) {
  if (static_cast<int>(current.size()) == r) {
    result.push_back(current);
    return;
  }
  for (int i = start; i < static_cast<int>(variables.size()); ++i) {
    current.push_back(variables[i]);
    _generateCombinations(variables, r, result, current, i + 1);
    current.pop_back();
  }
}

std::vector<std::vector<int>>
generateCombinations(const std::vector<int> &variables, int r) {
  std::vector<std::vector<int>> result;
  std::vector<int> current;
  _generateCombinations(variables, r, result, current, 0);

  return result;
}

class Agent {
private:
  enum BombQueryResult { HAS_BOMB, NO_BOMB, UNKOWN };
  Level *level;
  Solver solver;
  Matrix2D<int> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  std::unordered_set<Vector2> undecided;
  int foundBombs = 0;

  bool checkBomb(Vector2 tile, bool exists = true) {
    int var = hasBombVariables[tile];
    var = exists ? var : -var;

    if (solver.solve(-var))
      return false;
    solver.addClause(var);
    return true;
  }

  BombQueryResult queryBomb(Vector2 tile) {
    if (checkBomb(tile))
      return HAS_BOMB;
    else if (checkBomb(tile, false))
      return NO_BOMB;
    else
      return UNKOWN;
  }

  void generateClauses(const std::vector<int> &variables, int k) {
    int n = variables.size();
    if (n < k)
      throw std::logic_error("More bombs than avaliable spaces.");

    // L
    int rL = n - k + 1;
    for (const auto &combination : generateCombinations(variables, rL)) {
      solver.addClause(combination);
    }

    // U
    int rU = k + 1;
    for (const auto &combination : generateCombinations(variables, rU)) {
      std::vector<int> negatedCombination;
      for (int var : combination) {
        negatedCombination.push_back(-var);
      }
      solver.addClause(negatedCombination);
    }
  }

  void feedNewInfo(std::vector<std::pair<Vector2, int>> openCells) {
    for (auto &[position, value] : openCells) {
      solver.addClause(-hasBombVariables[position]);
      if (value == 0)
        continue;

      std::vector<int> variables;
      for (auto &adjacent : level->getUnkownAdjacent(position))
        variables.push_back(hasBombVariables[adjacent]);

      generateClauses(variables, value);
    }
  }

  void queryTile(Vector2 pos) {
    Level::Tile tile = level->getTile(pos);
    if (tile.marked || tile.discovered)
      return;

    switch (queryBomb(pos)) {
    case HAS_BOMB:
      foundBombs++;
      level->mark(pos);
      solver.addClause(hasBombVariables[pos]);
      break;
    case NO_BOMB:
      level->probe(pos);
      solver.addClause(-hasBombVariables[pos]);
      break;
    case UNKOWN:
      undecided.insert(pos);
      break;
    }
  }

public:
  Agent(Level *level)
      : level(level), hasBombVariables(level->getSize(), level->getSize(), -1) {
    int mapSize = level->getSize();
    for (int x = 0; x < mapSize; x++) {
      for (int y = 0; y < mapSize; y++) {
        int variable = solver.addVariable();
        hasBombVariables[x][y] = variable;
        inverseLookup[variable - 1] = {x, y};
      }
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Agent &kb) {
    os << kb.solver;
    return os;
  }

  bool decide() {
    if (level->getBombCount().has_value() &&
        foundBombs >= level->getBombCount().value()) {
      // All bombs found, probe all remaining tiles
      for (auto &unknown : level->getAllUnknowns())
        level->probe(unknown);

      return false;
    }

    for (Vector2 pos : undecided) {
      queryTile(pos);
    }

    const auto openCells = level->getOpenCells();
    if (openCells.size() == 0)
      return false;
    feedNewInfo(openCells);

    for (auto &[position, value] : openCells)
      for (auto &adjacent : level->getUnkownAdjacent(position))
        queryTile(adjacent);

    return true;
  }
};
