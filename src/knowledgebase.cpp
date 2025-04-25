#pragma once
#include "level.cpp"
#include "matrix2d.cpp"
#include "solver.cpp"
#include "vector2.cpp"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <unistd.h>
#include <vector>
#include <climits>
#include <algorithm>

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
  void generateMoves(ILevel* level, std::vector<std::pair<Vector2, bool>>& moves) {
    moves.clear();
    std::vector<Vector2> safeMoves;
    std::vector<Vector2> bombMoves;
    std::vector<Vector2> unknownCells;

    auto isInside = [&](Vector2 pos) {
        return pos.x >= 0 && pos.y >= 0 && pos.x < level->getSize() && pos.y < level->getSize();
    };

    for (int y = 0; y < level->getSize(); y++) {
        for (int x = 0; x < level->getSize(); x++) {
            Vector2 pos(x, y);
            if (level->getCell(pos) == TILE_UNKOWN && !level->isMarked(pos)) {
                if (checkBomb(pos, false)) {
                    safeMoves.push_back(pos);
                } else if (checkBomb(pos)) {
                    bombMoves.push_back(pos);
                } else {
                    unknownCells.push_back(pos);
                }
            }
        }
    }

    for (const auto& pos : bombMoves) {
        moves.emplace_back(pos, true);
    }

    for (const auto& pos : safeMoves) {
        moves.emplace_back(pos, false);
    }

    if (moves.empty() && !unknownCells.empty()) {
        Vector2 bestMove;
        int minAdjacentBombs = INT_MAX;
        
        for (const auto& pos : unknownCells) {
            int bombCount = 0;
            for (const auto& dir : Vector2::AllDirections()) {
                Vector2 adj = pos + dir;
                if (isInside(adj) && level->isMarked(adj)) {
                    bombCount++;
                }
            }
            
            if (bombCount < minAdjacentBombs) {
                minAdjacentBombs = bombCount;
                bestMove = pos;
            }
        }
        
        moves.emplace_back(bestMove, false);
    }
}
  /* Query bomb existence in tile,
  second parameter indicates wether we query
  its existence or lack thereof */
  bool checkBomb(Vector2 tile, bool exists = true) {
    int var = hasBombVariables.at(tile);  // Usando at() para acesso seguro
    var = exists ? var : -var;

    if (solver.solve(-var))
        return false;
    solver.addClause(var);
    return true;
}

void feedNewInfo(ILevel* level) {
  const auto& openCells = level->getOpenCells();
  for (auto& cell : openCells) {
      solver.addClause(-hasBombVariables.at(cell.first));
      
      if (cell.second == 0) continue;
      
      auto unknowns = level->getUnkownAdjacent(cell.first);
      if (unknowns.empty()) continue;
      
      std::vector<int> variables;
      for (auto& adjacent : unknowns) {
          variables.push_back(hasBombVariables.at(adjacent));
      }
      
      generateClauses(variables, cell.second);
      
      if (level->isMarked(cell.first)) {
          solver.addClause(hasBombVariables.at(cell.first));
      }
  }
}


  void generateClauses(const std::vector<int> &variables, int k) {
    int n = variables.size();
    if (n < k || k < 0) {
      return;
  }

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
  bool hasUnresolvedCells() const {
    for (const auto& [var, pos] : inverseLookup) {
        std::vector<int> clause = {var};
        
        bool cannotBeTrue = !solver.solve(clause);
        bool cannotBeFalse = !solver.solve({-var});
        
        if (!cannotBeTrue && !cannotBeFalse) {
            return true;
        }
    }
    return false;
}
};
