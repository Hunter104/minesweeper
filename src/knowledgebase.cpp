#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include "vector2.cpp"
#include "matrix2d.cpp"
#pragma once

constexpr int UNSAT=20;

void generateCombinations(const std::vector<int>& variables, int r, std::vector<std::vector<int>>& result, std::vector<int>& current, int start);

// Negative variables are false and positive ones are true
class KnowledgeBase {
private:
  Matrix2D<int> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  std::vector<std::vector<int>> clauses;
  int variableCount = 0;

  bool isSatisfiable(const std::string& dimacsCnf) const {
    FILE* claspIn = popen("clasp - > /dev/null", "w");
    if (!claspIn)
      throw std::runtime_error("Failed to start clasp.");

    fwrite(dimacsCnf.c_str(), 1, dimacsCnf.size(), claspIn);
    int status = pclose(claspIn);
    if (WIFEXITED(status)) {
      int code = WEXITSTATUS(status);
      return code != 20;
    }
    throw std::runtime_error("Clasp failed to execute.");
  }
public:
  KnowledgeBase(int mapSize) : hasBombVariables(mapSize, mapSize, -1) {
    for (int x=0; x<mapSize; x++) {
      for (int y=0; y<mapSize; y++) {
        hasBombVariables[x][y] = variableCount+1;
        inverseLookup[variableCount+1] = {x, y};
        variableCount++;
      }
    }
  }

  std::string toDimacsWithClause(const std::vector<int>& additionalClause) const {
    std::ostringstream text;
    text << "p cnf " << variableCount << ' ' << clauses.size() + 1 << '\n';
    for (const auto& clause : clauses) {
      for (int var : clause)
        text << var << ' ';
      text << "0\n";
    }
    for (int var : additionalClause)
      text << var << ' ';
    text << "0\n";
    return text.str();
  }

  /* Query bomb existence in tile, 
  second parameter indicates wether we query 
  its existence or lack thereof */
  bool checkBomb(Vector2 tile, bool checkExists = true) {
    int modifier = checkExists ? 1 : -1;
    int variable = modifier * hasBombVariables[tile];
    std::string dimacs = toDimacsWithClause({-variable});
    if (isSatisfiable(dimacs)) return false;
    clauses.push_back({variable});
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const KnowledgeBase& kb) {
    os << "p cnf " << kb.variableCount << ' ' << kb.clauses.size() << '\n';
    for (auto& clause : kb.clauses) {
      for (auto& variable : clause) {
        os << variable << ' ';
      }
      os << "0\n";
    }
    return os;
  }

  void generateClauses(const std::vector<int>& variables, int k) {
    int n = variables.size();

    // Caso especial k=0
    if (k == 0) {
        for (int var : variables) {
            clauses.push_back({-var});
        }
        return;
    }

    // Caso especial k=n
    if (k == n) {
        for (int var : variables) {
            clauses.push_back({var});
        }
        return;
    }

    // L
    int rL = n - k + 1;
    std::vector<std::vector<int>> combinationsL;
    std::vector<int> currentL;
    generateCombinations(variables, rL, combinationsL, currentL, 0);
    for (const auto& combination : combinationsL) {
        clauses.push_back(combination);
    }

    // U
    int rU = k + 1;
    std::vector<std::vector<int>> combinationsU;
    std::vector<int> currentU;
    generateCombinations(variables, rU, combinationsU, currentU, 0);
    for (const auto& combination : combinationsU) {
        std::vector<int> negatedCombination;
        for (int var : combination) {
            negatedCombination.push_back(-var);
        }
        clauses.push_back(negatedCombination);
    }
  }
};

// Função geradora de combinações
void generateCombinations(const std::vector<int>& variables, int r, std::vector<std::vector<int>>& result, std::vector<int>& current, int start) {
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
