#include "knowledgebase.cpp"
#include "level.cpp"
#include <argp.h>
#include <bits/getopt_core.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unistd.h>

// Optimizations:
// Fix passing arguments passing by value everywhere
struct Arguments {
  bool test = false;
  bool generate = false;
  int size = 10;
  int bombs = 20;
};

Arguments parse_args(int argc, char *argv[]) {
  Arguments args;

  int opt;
  while ((opt = getopt(argc, argv, "htgs:b:")) != -1) {
    switch (opt) {
    case 't':
      args.test = true;
      break;
    case 'g':
      args.generate = true;
      break;
    case 's':
      args.size = std::atoi(optarg);
      break;
    case 'b':
      args.bombs = std::atoi(optarg);
      break;
    case 'h':
    default:
      std::cerr << "Usage: " << argv[0] << " [-t] [-g] [-s size] [-b bombs]\n";
      std::exit(EXIT_FAILURE);
    }
  }

  return args;
}

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  Arguments args = parse_args(argc, argv);
  ILevel *level; 

  if (args.generate) 
    level = new GeneratedLevel(args.size, args.bombs);
  else
    level = InputLevel::create();

  if (args.test)
    std::cout << *level;

  KnowledgeBase kb(level->getSize());

  kb.feedNewInfo(level);
  if (args.test)
    std::cout << kb;

  return EXIT_SUCCESS;
}
