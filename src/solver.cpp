#pragma once
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

constexpr int UNSAT = 20;

// Clasp wrapper, similar to how minisat's solver class
class Solver {
private:
  std::string command;
  int variableCount;
  std::vector<std::vector<int>> clauses;

  std::string toDimacs(const std::vector<int> &additionalClause) const {
    std::ostringstream text;
    text << "p cnf " << variableCount << ' ' << clauses.size() + 1 << '\n';
    for (const auto &clause : clauses) {
      for (int var : clause)
        text << var << ' ';
      text << "0\n";
    }
    for (int var : additionalClause)
      text << var << ' ';
    text << "0\n";
    return text.str();
  }

  std::string toDimacs() const {
    std::ostringstream text;
    text << "p cnf " << variableCount << ' ' << clauses.size() << '\n';
    for (const auto &clause : clauses) {
      for (int var : clause)
        text << var << ' ';
      text << "0\n";
    }
    return text.str();
  }

  bool isSatisfiable(const std::string &dimacsCnf) const {
    FILE *claspIn = popen(command.c_str(), "w");
    if (!claspIn)
      throw std::runtime_error("Failed to start clasp.");

    fwrite(dimacsCnf.c_str(), 1, dimacsCnf.size(), claspIn);
    int status = pclose(claspIn);
    if (WIFEXITED(status)) {
      int code = WEXITSTATUS(status);
      return code != UNSAT;
    }
    throw std::runtime_error("Clasp failed to execute.");
  }

public:
  Solver(bool debug = false) {
    if (system("which clasp > /dev/null 2>&1"))
      throw std::runtime_error(
          "Could not find clasp command, are you sure it is installed?");
    if (debug)
      command = "clasp -";
    else
      command = "clasp - > /dev/null";
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
    os << "p cnf " << s.variableCount << ' ' << s.clauses.size() << '\n';
    for (auto &clause : s.clauses) {
      for (auto &variable : clause) {
        os << variable << ' ';
      }
      os << "0\n";
    }
    return os;
  }
};
