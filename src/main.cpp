#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "level.cpp"
#include "knowledgebase.cpp"

int main (int argc, char *argv[]) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }
  srand(time(nullptr));
  ILevel *level; 
  if (argc > 1 && std::string(argv[1]) == "-g")
    level = new GeneratedLevel(14, 25);
  else
    level = InputLevel::create(std::cin, std::cout);
  KnowledgeBase kb = KnowledgeBase(level->getSize());
  // while (true) {
  //   kb.getInfo(level)
  //   vector<Vector2, Action> actions = kb.query(level)
  //   for (auto& action : results)
  //     if (action.second == mark)
  //      level.mark(action.first)
  //     else 
  //      level.probe(action.second)
  //   if (! level.update())
  //    break;
  // }

  delete level;

  return EXIT_SUCCESS;
}
