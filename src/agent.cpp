#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "solver.cpp"
#include "vector2.cpp"
#include <map>
#include <stdexcept>
#include <unistd.h>
#include <vector>

// Função geradora de combinações
void generateCombinations(const std::vector<int> &variables, int r,
                          std::vector<std::vector<int>> &result,
                          std::vector<int> &current, int start) {
  if (static_cast<int>(current.size()) == r) {
    result.push_back(current);
    return;
  }
  for (int i = start; i < static_cast<int>(variables.size()); ++i) {
    current.push_back(variables[i]);
    generateCombinations(variables, r, result, current, i + 1);
    current.pop_back();
  }
}

// Negative variables are false and positive ones are true
class Agent {
private:
  Level *level;
  Solver solver;
  Matrix2D<int> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  int foundBombCount = 0;

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

  /* Query bomb existence in tile,
  second parameter indicates wether we query
  its existence or lack thereof */
  bool checkBomb(Vector2 tile, bool exists = true) {
    int var = hasBombVariables[tile];
    var = exists ? var : -var;

    if (solver.solve(-var))
      return false;
    solver.addClause(var);
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const Agent &kb) {
    os << kb.solver;
    return os;
  }

  void generateClauses(const std::vector<int> &variables, int k) {
    int n = variables.size();
    if (n < k)
      throw std::logic_error("More bombs than avaliable spaces.");

    // Caso especial k=0
    if (k == 0) {
      for (int var : variables) {
        solver.addClause(-var);
      }
      return;
    }

    // Caso especial k=n
    if (k == n) {
      for (int var : variables) {
        solver.addClause(var);
      }
      return;
    }

    // L
    int rL = n - k + 1;
    std::vector<std::vector<int>> combinationsL;
    std::vector<int> currentL;
    generateCombinations(variables, rL, combinationsL, currentL, 0);
    for (const auto &combination : combinationsL) {
      solver.addClause(combination);
    }

    // U
    int rU = k + 1;
    std::vector<std::vector<int>> combinationsU;
    std::vector<int> currentU;
    generateCombinations(variables, rU, combinationsU, currentU, 0);
    for (const auto &combination : combinationsU) {
      std::vector<int> negatedCombination;
      for (int var : combination) {
        negatedCombination.push_back(-var);
      }
      solver.addClause(negatedCombination);
    }
  }

  bool decide() {
    if (level->getBombCount().has_value() &&
        level->getBombCount().value() == foundBombCount)
      return false;

    const auto &newOpenCells = level->getOpenCells();
    if (newOpenCells.empty())
      return false;

    for (auto &[position, value] : newOpenCells) {
      solver.addClause(-hasBombVariables[position]);
      if (value == 0)
        continue;
      std::vector<int> variables;
      for (auto &adjacent : level->getUnkownAdjacent(position)) {
        variables.push_back(hasBombVariables[adjacent]);
      }
      if (variables.empty() && value > 0)
        throw std::logic_error(
            "Contradiction: Cell " + std::to_string(position.x) + "," +
            std::to_string(position.y) + " shows " + std::to_string(value) +
            " bombs but has no unknown neighbors");
      generateClauses(variables, value);
    }

    for (auto &[position, value] : newOpenCells) {
      for (auto &adjacent : level->getUnkownAdjacent(position)) {
        if (checkBomb(adjacent)) {
          level->mark(adjacent);
          foundBombCount++;
        } else if (checkBomb(adjacent, false))
          level->probe(adjacent);
      }
    }

    return true;
  }
};
