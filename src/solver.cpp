#pragma once
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

enum class SolverStatus : int { SATISFIABLE = 10, UNSATISFIABLE = 20 };
// Clasp wrapper, similar to minisat's solver class
class Solver {
private:
  int variableCount = 0;
  int clauseCount = 0;
  std::string clauses = "";

  class ProcessPipe {
  private:
    FILE *pipe;

  public:
    explicit ProcessPipe(const char *cmd, const char *mode)
        : pipe(popen(cmd, mode)) {
      if (!pipe)
        throw std::runtime_error("Failed to start minisat process");
    }

    ~ProcessPipe() {
      if (pipe)
        pclose(pipe);
    }

    ProcessPipe(const ProcessPipe &) = delete;
    ProcessPipe &operator=(const ProcessPipe &) = delete;

    operator FILE *() const { return pipe; }

    int close() {
      int status = pclose(pipe);
      pipe = nullptr;
      return status;
    }
  };

  inline std::string clauseToString(const std::vector<int> &clause) const {
    std::string result;
    result.reserve(clause.size() * 12 + 2);
    for (int variable : clause)
      result += std::to_string(variable) + " ";
    result += "0\n";
    return result;
  }

  bool isSatisfiable(const std::vector<int> &assumption = {}) const {
#ifdef DEBUG
    constexpr char command[] = "minisat";
#else
    constexpr char command[] = "minisat > /dev/null";
#endif

    ProcessPipe minisatIn(command, "w");
    int realClauseCount = clauseCount + (assumption.empty() ? 0 : 1);

    std::fputs(clauses.c_str(), minisatIn);

    if (!assumption.empty()) {
      std::fputs(clauseToString(assumption).c_str(), minisatIn);
    }

    std::fflush(minisatIn);

    const int status = minisatIn.close();
    if (!WIFEXITED(status)) {
      throw std::runtime_error("Minisat failed to execute with status code: " +
                               std::to_string(status));
    }

    int exitCode = WEXITSTATUS(status);

    if (exitCode == static_cast<int>(SolverStatus::UNSATISFIABLE)) {
      return false;
    } else if (exitCode == static_cast<int>(SolverStatus::SATISFIABLE)) {
      return true;
    } else {
      throw std::runtime_error("Minisat failed with unexpected exit code: " +
                               std::to_string(exitCode));
    }
  }

public:
  Solver() {
    if (system("which minisat > /dev/null 2>&1") != 0)
      throw std::runtime_error("Minisat not found");
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
