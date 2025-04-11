#include <cstdio>
#include <cstdlib>
#include <exception>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
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

  bool query(int queryVariable) {
    std::string dimacs = toDimacsWithClause({-queryVariable});
    if (!isSatisfiable(dimacs)) {
      clauses.push_back({queryVariable});
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
