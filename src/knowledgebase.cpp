#pragma once
#include "matrix2d.cpp"
#include "solver.cpp"
#include "vector2.cpp"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <unistd.h>
#include <vector>
#include "level.cpp"

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
class KnowledgeBase {
private:
  Matrix2D<int> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  Solver solver;

public:
  KnowledgeBase(int mapSize) : hasBombVariables(mapSize, mapSize, -1) {
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

    if (solver.solve({-var}))
      return false;
    solver.addClause({var});
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const KnowledgeBase &kb) {
    os << kb.solver;
    return os;
  }

  void generateClauses(const std::vector<int> &variables, int k) {
    int n = variables.size();

    // Caso especial k=0
    if (k == 0) {
      for (int var : variables) {
        solver.addClause({-var});
      }
      return;
    }

    // Caso especial k=n
    if (k == n) {
      for (int var : variables) {
        solver.addClause({var});
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

  void feedNewInfo(ILevel* level) {
    const std::vector<std::pair<Vector2, int>> &openCells = level->getOpenCells();
    for (auto& cell : openCells) {
      if (cell.second == 0) continue;
      std::vector<int> variables;
      for (auto& adjacent : level->getUnkownAdjacent(cell.first)) {
        variables.push_back(hasBombVariables[adjacent]);
      }
      generateClauses(variables, cell.second);
    }
  }

};
