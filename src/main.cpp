#include "agent.cpp"
#include "generated-level.cpp"
#include "input-level.cpp"
#include "level.cpp"
#include <algorithm>
#include <argp.h>
#include <bits/getopt_core.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <ios>
#include <iostream>
#include <memory>
#include <unistd.h>

// Optimizations:
// Fix passing arguments passing by value everywhere
struct Arguments {
  bool test = false;
  bool generate = false;
  int size = 10;
  int bombs = 15;
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

void timeout_handler(int sig) {
  (void)sig;
  std::cout << "0";
  exit(0);
}

int main(int argc, char *argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  Arguments args = parse_args(argc, argv);

  std::signal(SIGALRM, timeout_handler);

  alarm(9);
  std::unique_ptr<Level> level;
  if (args.generate)
    level = std::make_unique<GeneratedLevel>(args.size, args.bombs);
  else
    level = std::unique_ptr<Level>(InputLevel::create());

  int step = 1;
  if (args.test)
    std::cout << *level;

  Agent agent(level.get());
  while (1) {
    if (!agent.decide()) {
      std::cout << "0";
      return EXIT_SUCCESS;
    }
    level->update();
    if (args.test)
      std::cout << "step: " << ++step << '\n' << *level;
  }

  return EXIT_SUCCESS;
}
