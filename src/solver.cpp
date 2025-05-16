#pragma once
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#define SOLVER "minisat"

enum class SolverStatus : int { SATISFIABLE = 10, UNSATISFIABLE = 20 };

class Solver {
private:
  int variableCount = 0;
  int clauseCount = 0;
  std::string clauses = "";
  ;

  std::string clauseToString(const std::vector<int> &clause) const {
    std::string result;
    result.reserve(clause.size() * 12 + 2);
    for (int variable : clause) {
      result.append(std::to_string(variable));
      result.push_back(' ');
    }
    result.append("0\n");
    return result;
  }

public:
  Solver() {
    if (system("which " SOLVER " > /dev/null 2>&1") != 0)
      throw std::runtime_error(SOLVER " not found");
    clauses.reserve(1024);
  }

  // Clasp starts indexing from 1
  [[nodiscard]] int addVariable() { return ++variableCount; }

  template <
      typename... Ints,
      std::enable_if_t<(std::is_convertible_v<Ints, int> && ...), int> = 0>
  void addClause(Ints... ints) {
    static_assert((std::is_convertible_v<Ints, int> && ...),
                  "All arguments must be convertible to int");
    std::vector<int> clause = {static_cast<int>(ints)...};
    addClause(clause);
  }

  void addClause(const std::vector<int> &clause) {
    if (clause.empty())
      throw std::logic_error(
          "Trying to insert empty clause, empty clauses are unsatisfiable.");
    clauseCount++;
    clauses += clauseToString(clause);
  }

  bool solve(const std::vector<int> &assumption) const {
#ifdef DEBUG
    constexpr char command[] = SOLVER;
#else
    constexpr char command[] = SOLVER " > /dev/null";
#endif

    FILE *solverIn = popen(command, "w");
    std::fputs(clauses.c_str(), solverIn);

    if (!assumption.empty()) {
      std::fputs(clauseToString(assumption).c_str(), solverIn);
    }

    std::fflush(solverIn);

    const int status = pclose(solverIn);
    if (!WIFEXITED(status)) {
      throw std::runtime_error(SOLVER " failed to execute with status code: " +
                               std::to_string(status));
    }

    int exitCode = WEXITSTATUS(status);

    if (exitCode == static_cast<int>(SolverStatus::UNSATISFIABLE)) {
      return false;
    } else if (exitCode == static_cast<int>(SolverStatus::SATISFIABLE)) {
      return true;
    } else {
      throw std::runtime_error(SOLVER "failed with unexpected exit code: " +
                               std::to_string(exitCode));
    }
  }

  bool solve() const { return solve({}); }

  template <
      typename... Ints,
      std::enable_if_t<(std::is_convertible_v<Ints, int> && ...), int> = 0>
  bool solve(Ints... ints) {
    static_assert((std::is_convertible_v<Ints, int> && ...),
                  "All arguments must be convertible to int");
    std::vector<int> clause = {static_cast<int>(ints)...};
    return solve(clause);
  }

  friend std::ostream &operator<<(std::ostream &os, const Solver &s) {
    os << s.clauses;
    return os;
  }
};
