#include <cstdio>
#include <cstdlib>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include "vector2.cpp"
#pragma once

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
        inverseLookup[variableCount+1] =  {x, y};
        variableCount++;
      }
    }
  }

  bool query(int queryVariable) {
    char filename[] = "/tmp/minesweeper.XXXXXX";
    int fd = mkstemp(filename);
    if (fd < 0)
      throw std::runtime_error("Could not create tempfile.");
    std::ostringstream text;
    text << "p cnf " << variableCount << clauses.size() << '\n';
    for (auto& clause : clauses) {
      for (auto& variable : clause) {
        text << variable << ' ';
      }
      text << "0\n";
    }

    text << queryVariable << " 0\n";

    std::string result = text.str();
    const char *cText = result.c_str();
    write(fd, cText, strlen(cText)); // Non buffered, may cause slowdown
    close(fd);

    std::string command = "clasp ";
    command += filename;
    command += " > /dev/null";

    int rc = std::system(command.c_str());

    unlink(filename);
    return rc == 20;
  }
};
