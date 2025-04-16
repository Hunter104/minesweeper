#pragma once
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#ifdef DEBUG
constexpr char command[] = "clasp -";
#else
constexpr char command[] = "clasp - > /dev/null";
#endif

constexpr int UNSAT = 20;

// Clasp wrapper, similar to minisat's solver class
class Solver {
private:
  int variableCount = 0;
  int clauseCount = 0;
  std::string clauses = "";

  inline std::string clausetoString(const std::vector<int> &clause) const {
    std::string result;
    result.reserve(clause.size() * 12 + 2);
    for (int variable : clause)
      result += std::to_string(variable) + " ";
    result += "0\n";
    return result;
  }

  bool isSatisfiable(const std::vector<int> &assumption = {}) const {
    FILE *claspIn = popen(command, "w");
    if (!claspIn)
      throw std::runtime_error("Failed to start clasp.");

    int realClauseCount = assumption.empty() ? clauseCount : clauseCount + 1;
    std::fprintf(claspIn, "p cnf %d %d\n%s", variableCount, realClauseCount,
                 clauses.c_str());
    if (!assumption.empty())
      std::fprintf(claspIn, "%s", clausetoString(assumption).c_str());

    const int status = pclose(claspIn);
    if (!WIFEXITED(status)) {
      throw std::runtime_error("Clasp failed to execute with status code: " +
                               std::to_string(status));
    }

    if (WEXITSTATUS(status) == 20)
      return false;
    else if (WEXITSTATUS(status) == 10)
      return true;
    else
      throw std::runtime_error("Clasp failed with status code: " +
                               std::to_string(WEXITSTATUS(status)));
  }

public:
  Solver() {
    if (system("which clasp > /dev/null 2>&1"))
      throw std::runtime_error("Clasp not found");
  }

  // Clasp starts indexing from 1
  inline int addVariable() { return ++variableCount; }

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
    clauseCount++;
    clauses += clausetoString(clause);
  }

  /* True = satisfiable
   * False = unsatisfiable
   */
  bool solve() const { return isSatisfiable(); }

  bool solve(const std::vector<int> &assumption) const {
    return isSatisfiable(assumption);
  }

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
