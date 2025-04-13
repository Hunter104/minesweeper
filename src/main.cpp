#include "knowledgebase.cpp"
#include "level.cpp"
#include <argp.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>

// Testa funcao que gera clausulas
void testGenerateClauses() {
  KnowledgeBase kb(5);
  std::vector<int> variables = {1, 2, 3, 4, 5, 6, 7, 8};
  int k = 4;
  kb.generateClauses(variables, k);
  std::cout << kb;
}

int main(int argc, char *argv[]) {
  srand(time(nullptr));
  // Test flag check

  ILevel *level;
  if (argc > 1) {
    std::string first = std::string(argv[1]);
    if (first == "--test") {
      testGenerateClauses();
      return EXIT_SUCCESS;
    }
    if (first == "-g")
      level = new GeneratedLevel(14, 25);
    else
      level = InputLevel::create(std::cin, std::cout);
  }
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
