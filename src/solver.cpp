
#pragma once
#include <cstdio>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
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
    char tempfile[] = "/tmp/minisat_input_XXXXXX";
    int fd = mkstemp(tempfile);
    if (fd == -1) {
      throw std::runtime_error("Failed to create temporary file");
    }

    FILE *file = fdopen(fd, "w");
    if (!file) {
      close(fd);
      throw std::runtime_error("Failed to open temporary file");
    }

    const int totalClauses = clauseCount + (assumption.empty() ? 0 : 1);
    fprintf(file, "p cnf %d %d\n", variableCount, totalClauses);
    fputs(clauses.str().c_str(), file);

    if (!assumption.empty()) {
      for (int var : assumption) {
        fprintf(file, "%d ", var);
      }
      fprintf(file, "0\n");
    }

    fclose(file);

    std::string cmd = std::string(COMMAND) + " " + tempfile;
#ifndef DEBUG
    cmd += " > /dev/null";
#endif

    const int status = system(cmd.c_str());

    unlink(tempfile);

    if (!WIFEXITED(status)) {
      throw std::runtime_error(COMMAND " failed to execute with status: " +
                               std::to_string(status));
    }

    const int exitCode = WEXITSTATUS(status);
    switch (exitCode) {
    case SATISFIABLE:
      return true;
    case UNSATISFIABLE:
      return false;
    default:
      throw std::runtime_error(COMMAND " failed with unexpected exit code: " +
                               std::to_string(exitCode));
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
