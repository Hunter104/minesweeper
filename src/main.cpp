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

void playAutomatedGame(ILevel* level, KnowledgeBase& kb) {
    int score = 0;
    int turns = 0;

    while (true) {
        turns++;
        kb.feedNewInfo(level);

        // Verifica células seguras para abrir ou marcar como bombas
        std::vector<Vector2> toProbe, toMark;

        for (int y = 0; y < level->getSize(); y++) {
            for (int x = 0; x < level->getSize(); x++) {
                Vector2 pos(x, y);
                if (level->getCell(pos) == TILE_UNKOWN && !level->isMarked(pos)) {
                    if (kb.checkBomb(pos)) {
                        toMark.push_back(pos);
                        score += 5; // +5 pontos por bomba correta
                    } 
                    else if (kb.checkBomb(pos, false)) {
                        toProbe.push_back(pos);
                    }
                }
            }
        }

        // Executa as ações
        for (auto& pos : toMark) level->mark(pos);
        for (auto& pos : toProbe) level->probe(pos);

        // Se não houver ações lógicas, faz uma jogada aleatória
        if (toMark.empty() && toProbe.empty()) {
            std::vector<Vector2> unknowns;
            for (int y = 0; y < level->getSize(); y++) {
                for (int x = 0; x < level->getSize(); x++) {
                    Vector2 pos(x, y);
                    if (level->getCell(pos) == TILE_UNKOWN && !level->isMarked(pos)) {
                        unknowns.push_back(pos);
                    }
                }
            }
            if (!unknowns.empty()) {
                level->probe(unknowns[rand() % unknowns.size()]);
            } else {
                break; // Fim do jogo
            }
        }

        // Atualiza o jogo e verifica se deve continuar
        if (!level->update()) break;

        // Penalidade por turnos extras
        if (turns > 1) score -= 10;
    }

    // (O juiz virtual calcula o score automaticamente)
}

int main(int argc, char *argv[]) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  chdir("/tmp");

  Arguments args = parse_args(argc, argv);
  ILevel *level = nullptr;
  
  try {
      level = args.generate ? 
          new GeneratedLevel(args.size, args.bombs) : 
          InputLevel::create();

      KnowledgeBase kb(level->getSize());

      if (args.test) {
          while (true) {
              std::cout << *level;
              Vector2 pos;
              std::cout << "Query bomb position (y x): ";
              std::cin >> pos.y >> pos.x;

              if (pos.y < 0 || pos.x < 0 || pos.y >= level->getSize() || pos.x >= level->getSize()) {
                  std::cerr << "Invalid position. Try again.\n";
                  continue;
              }

              if (kb.checkBomb(pos)) {
                  std::cout << "Bomb!\n";
                  level->mark(pos);
              }
              else if (kb.checkBomb(pos, false)) {
                  std::cout << "Safe.\n";
                  level->probe(pos);
              }
              else {
                  std::cout << "Unknown.\n";
                  continue;
              }

              if (!level->update()) {
                  std::cout << "Game update failed or game ended.\n";
                  break;
              }
          }
      } else {
          bool gameActive = true;
int turnCount = 0;
const int maxTurnsWithoutProgress = 5;
int unchangedTurns = 0;
int lastOpenCount = level->getOpenCells().size();

while (gameActive && turnCount < 100) {
    turnCount++;
    std::cerr << "--- Turno " << turnCount << " ---\n";
    
    kb.feedNewInfo(level);
    
    int currentOpen = level->getOpenCells().size();
    if (currentOpen == lastOpenCount) {
        unchangedTurns++;
        if (unchangedTurns >= maxTurnsWithoutProgress) {
            std::cerr << "Sem progresso por " << maxTurnsWithoutProgress << " turnos - terminando\n";
            break;
        }
    } else {
        unchangedTurns = 0;
        lastOpenCount = currentOpen;
    }
    
    std::vector<std::pair<Vector2, bool>> moves;
    kb.generateMoves(level, moves);
    
    if (moves.empty()) {
        std::cerr << "Nenhuma jogada válida encontrada - terminando\n";
        gameActive = false;
        continue;
    }
    
    for (const auto& [pos, isMark] : moves) {
        std::cerr << "Jogada: " << pos.y << " " << pos.x << " " << (isMark ? "B" : "A") << "\n";
        isMark ? level->mark(pos) : level->probe(pos);
    }
    
    gameActive = level->update();
    
    if (level->getBombCount().has_value() && 
        level->getMarkedCount() == level->getBombCount().value()) {
        std::cerr << "Todas as bombas foram marcadas - vitória!\n";
        gameActive = false;
    }
}
      }
  } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";
      if (level) delete level;
      return EXIT_FAILURE;
  }

  delete level;
  return EXIT_SUCCESS;
}