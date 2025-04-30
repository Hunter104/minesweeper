#include "agent.cpp"
#include "level.cpp"
#include <argp.h>
#include <bits/getopt_core.h>
#include <cerrno>
#include <csignal>
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

void printKb(Agent &kb) { std::cout << kb; }

void printLevel(ILevel *level) { std::cout << *level; }

volatile std::sig_atomic_t timeout_flag = false;
void timeout_handler(int sig) {
  (void)sig;
  std::cout << "0";
  exit(0);
}

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  Arguments args = parse_args(argc, argv);

  std::signal(SIGALRM, timeout_handler);

  alarm(9);
  ILevel *level;

  if (args.generate)
    level = new GeneratedLevel(args.size, args.bombs);
  else
    level = InputLevel::create();

  int step = 1;
  if (args.test)
    std::cout << *level;

  Agent agent(level);
  while (!timeout_flag) {
    agent.decide();
    if (!level->update())
      return EXIT_SUCCESS;
    if (args.test)
      std::cout << "step: " << ++step << '\n' << *level;
  }

  return EXIT_SUCCESS;
}
