
#pragma once
#include <cstdio>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#define COMMAND "minisat"

constexpr int SATISFIABLE = 10;
constexpr int UNSATISFIABLE = 20;

const char *command =
#ifdef DEBUG
    COMMAND;
#else
    COMMAND " > /dev/null";
#endif

class Solver {
private:
  int variableCount = 0;
  int clauseCount = 0;
  std::stringstream clauses;

  bool isSatisfiable(const std::vector<int> &assumption = {}) const {

    FILE *solverIn = popen(command, "w");
    if (!solverIn) {
      throw std::runtime_error("Failed to open pipe to " COMMAND);
    }

    const int totalClauses = clauseCount + (assumption.empty() ? 0 : 1);
    fprintf(solverIn, "p cnf %d %d\n%s", variableCount, totalClauses,
            clauses.str().c_str());

    if (!assumption.empty()) {
      for (int var : assumption) {
        fprintf(solverIn, "%d ", var);
      }
      fprintf(solverIn, "0\n");
    }

    fflush(solverIn);
    const int status = pclose(solverIn);

    if (!WIFEXITED(status))
      throw std::runtime_error(COMMAND " failed to execute with status: " +
                               std::to_string(status));

    const int exitCode = WEXITSTATUS(status);
    switch (exitCode) {
    case SATISFIABLE:
      return true;
      break;
    case UNSATISFIABLE:
      return false;
      break;
    default:
      throw std::runtime_error(COMMAND " failed with unexpected exit code: " +
                               std::to_string(exitCode));
      break;
    }
  }

public:
  Solver() {
    if (system("which " COMMAND " > /dev/null 2>&1") != 0)
      throw std::runtime_error(COMMAND " not found in PATH");
  }

  int addVariable() { return ++variableCount; }

  void addClause(const std::vector<int> &clause) {
    if (clause.empty()) {
      throw std::logic_error("Empty clauses are unsatisfiable");
    }

    clauseCount++;
    for (int literal : clause) {
      clauses << literal << " ";
    }
    clauses << "0\n";
  }

  template <typename... Literals> void addClause(Literals... literals) {
    addClause(std::vector<int>{literals...});
  }

  bool solve() const { return isSatisfiable(); }

  bool solve(const std::vector<int> &assumption) const {
    return isSatisfiable(assumption);
  }

  template <typename... Literals> bool solve(Literals... literals) const {
    return solve(std::vector<int>{literals...});
  }

  friend std::ostream &operator<<(std::ostream &os, const Solver &solver) {
    return os << solver.clauses.str();
  }
};
