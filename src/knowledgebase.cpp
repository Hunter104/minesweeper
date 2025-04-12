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
#pragma once

constexpr int UNSAT=20;

void generateCombinations(const std::vector<int>& variables, int r, std::vector<std::vector<int>>& result, std::vector<int>& current, int start);

// Negative variables are false and positive ones are true
class KnowledgeBase {
private:
  std::vector<std::vector<int>> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  std::vector<std::vector<int>> clauses;
  int variableCount = 0;
public:
  KnowledgeBase(int mapSize) :
  hasBombVariables(mapSize, std::vector<int>(mapSize, -1)){
    for (int x=0; x<mapSize; x++) {
      for (int y=0; y<mapSize; y++) {
        hasBombVariables[x][y] = variableCount+1;
        inverseLookup[variableCount+1] = {x, y};
        variableCount++;
      }
    }
  }

  bool query(int queryVariable) {
    std::ostringstream text;
    text << "p cnf " << variableCount+1 << ' ' << clauses.size()+1 << '\n';
    for (auto& clause : clauses) {
      for (auto& variable : clause) {
        text << variable << ' ';
      }
      text << "0\n";
    }

    text << -1*queryVariable << " 0\n";

    FILE *claspIn = popen("clasp - > /dev/null", "w");
    if (!claspIn) {
      throw std::runtime_error("Failed to open clasp subprocess.");
    }

    std::string cnf = text.str();
    std::cout << "RESULTADO\n" << cnf;
    fwrite(cnf.c_str(), 1, cnf.size(), claspIn);
    if (WEXITSTATUS(pclose(claspIn)) == UNSAT) {
      clauses.push_back({ queryVariable });
      return true;
    }

    return false;
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
