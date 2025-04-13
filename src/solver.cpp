#pragma once
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#ifdef DEBUG
constexpr char command[] = "clasp - > /dev/null";
#else
constexpr char command[] = "clasp -";
#endif

constexpr int UNSAT = 20;

// Clasp wrapper, similar to minisat's solver class
class Solver {
private:
  int variableCount;
  std::vector<std::vector<int>> clauses;

  std::string toDimacs(const std::vector<int> &additionalClause = {}) const {
    std::ostringstream text;
    text << "p cnf " << variableCount << ' '
         << clauses.size() + (additionalClause.empty() ? 0 : 1) << '\n';
    for (const auto &clause : clauses) {
      for (int var : clause)
        text << var << ' ';
      text << "0\n";
    }
    if (!additionalClause.empty()) {
      for (int var : additionalClause)
        text << var << ' ';
      text << "0\n";
    }
    return text.str();
  }

  bool isSatisfiable(std::string_view dimacsCnf) const {
    FILE *claspIn = popen(command, "w");
    if (!claspIn)
      throw std::runtime_error("Failed to start clasp.");

    fwrite(dimacsCnf.data(), 1, dimacsCnf.size(), claspIn);
    int status = pclose(claspIn);

    if (WIFEXITED(status))
      return WEXITSTATUS(status) != UNSAT;

    throw std::runtime_error("Clasp failed to execute.");
  }

public:
  Solver() {
    if (system("which clasp > /dev/null 2>&1"))
      throw std::runtime_error("Clasp not found");
  }

  // Clasp starts indexing from 1
  int addVariable() { return ++variableCount; }

  void addClause(const std::vector<int> &clause) { clauses.push_back(clause); }

  /* True = satisfiable
   * False = unsatisfiable
   */
  bool solve() { return isSatisfiable(toDimacs()); }

  bool solve(const std::vector<int> &assumption) {
    return isSatisfiable(toDimacs(assumption));
  }

  friend std::ostream &operator<<(std::ostream &os, const Solver &s) {
    os << s.toDimacs();
    return os;
  }
};
