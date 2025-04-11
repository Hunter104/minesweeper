#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "level.cpp"

int main (int argc, char *argv[]) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }
  srand(time(nullptr));
  // Initialize
  ILevel *level; 
  if (argc > 1 && std::string(argv[1]) == "-g")
    level = new GeneratedLevel(14, 25);
  else
    level = InputLevel::create(std::cin, std::cout);
  std::cout << *level;
  // while (true) {
  //   auto results = kb.query(level.getOpenCells())
  //   if (results.size == 0)
  //    break
  //   for (auto& action : results)
  //     if (action.second == mark)
  //      level.mark(action.first)
  //     else 
  //      level.mark(action.second)
  //   level.update()
  // }

  delete level;
}
