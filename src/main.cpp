#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "level.cpp"
#include "knowledgebase.cpp"

// Testa funcao que gera clausulas
void testGenerateClauses() {
    KnowledgeBase kb(5);
    std::vector<int> variables = {1, 2, 3, 4, 5, 6, 7, 8};
    int k = 4;
    kb.generateClauses(variables, k);
    std::cout << kb;
}

int main (int argc, char *argv[]) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }

  // Test flag check
  if (argc > 1 && std::string(argv[1]) == "--test") {
    testGenerateClauses();
    return EXIT_SUCCESS;
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
