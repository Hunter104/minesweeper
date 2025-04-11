#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "level.cpp"
#include "knowledgebase.cpp"

int main(void) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }

  // Initialize
  Level level = Level(std::cin, std::cout);
  KnowledgeBase kb(level.getSize());
  while (true) {
    // auto results = kb.query(level.getOpenCells())
    // if (results.size == 0)
    //  break
    // for (auto& action : results)
    //   if (action.second == mark)
    //    level.mark(action.first)
    //   else 
    //    level.mark(action.second)
    // level.update()
  }
}
