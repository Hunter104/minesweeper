#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include "vector2.cpp"
#include "matrix2d.cpp"
#pragma once

constexpr int UNSAT=20;

// Negative variables are false and positive ones are true
class KnowledgeBase {
private:
  Matrix2D<int> hasBombVariables;
  std::map<int, Vector2> inverseLookup;
  std::vector<std::vector<int>> clauses;
  int variableCount = 0;
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
};
