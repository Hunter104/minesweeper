#include "agent.cpp"
#include "generatedLevel.cpp"
#include "inputLevel.cpp"
#include "level.cpp"
#include <argp.h>
#include <bits/getopt_core.h>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>

// Optimizations:
// Fix passing arguments passing by value everywhere
struct Arguments {
  bool test = false;
  bool generate = false;
  int size = 9;
  int bombs = 10;
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

void printLevel(Level *level) { std::cout << *level; }

volatile std::sig_atomic_t timeout_flag = false;
void timeout_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "0", 1);
  _exit(0);
}

int main(int argc, char *argv[]) {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  Arguments args = parse_args(argc, argv);

  std::signal(SIGALRM, timeout_handler);

  alarm(9);
  Level *level;

  if (args.generate)
    level = new GeneratedLevel(args.size, args.bombs);
  else
    level = InputLevel::create();

  int step = 1;
  Agent agent(level);
  while (!timeout_flag) {
    if (args.test)
      std::cout << "step: " << step << '\n' << *level << '\n';
    if (!agent.decide())
      break;
    level->update();
    step++;
  }

  std::cout << "0\n";
  delete level;
  return EXIT_SUCCESS;
}
