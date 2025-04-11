#include <cerrno>
#include <istream>
#include <optional>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>

struct Vector2 {
  int x = 0, y = 0;

  Vector2() = default;
  Vector2(int x, int y) : x(x), y(y) {}
  Vector2(const Vector2 &other) = default;

  Vector2 operator+(const Vector2 &other) const {
    return Vector2(x + other.x, y + other.y);
  }

  Vector2 operator-(const Vector2 &other) const {
    return Vector2(x - other.x, y - other.y);
  }

  Vector2 operator*(int scalar) const {
    return Vector2(x * scalar, y * scalar);
  }

  Vector2 &operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2 &operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vector2 &operator*=(int scalar) {
    x *= scalar;
    return *this;
  }

  bool operator==(const Vector2 &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Vector2 &other) const { return !(*this == other); }

  static const std::vector<Vector2> &AllDirections() {
    static const std::vector<Vector2> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    return directions;
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
    return os << "(" << v.x << ", " << v.y << ")";
  }

  friend std::istream &operator>>(std::istream& in, Vector2& v) {
    return in >> v.x >> v.y;
  }
};

class KnowledgeBase {
private:
  std::vector<std::vector<int>> hasBombVariables;
  std::map<int, Vector2> inverseLookupMap;
  std::vector<std::vector<int>> clauses;

public:
  KnowledgeBase(int width, int height) {
    int variableCount = 0;
    hasBombVariables.resize(width);
    for (int i=0; i<width; i++) {
      hasBombVariables[i].resize(height);
      for (int j=0; j<height; j++) {
        hasBombVariables[i][j] = variableCount;
        inverseLookupMap[variableCount++] = {i, j};
      }
    }
  }

  KnowledgeBase(Vector2 mapSize) {
    KnowledgeBase(mapSize.x, mapSize.y);
  }
};

class Level {
private:
  int size;
  std::optional<int> bombCount;
  std::vector<std::vector<int>> playingField;
public:
  Level(int size, int bombCount)
    : size(size),
      bombCount(bombCount >= 0 ? std::optional<int>{bombCount} : std::nullopt),
      playingField(size, std::vector<int>(size, -1))
  {}

  void setCell(Vector2 pos, int value) {
    playingField[pos.x][pos.y] = value;
  }

  void updateFromStream(std::istream& in) {
    int openCellsCount;
    in >> openCellsCount;
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      setCell(pos, num);
    }
  }

  static Level createFromStream(std::istream& in) {
    int size, bombs, openCellsCount;
    in >> size >> bombs >> openCellsCount;

    Level level(size, bombs);
    for (int i=0; i < openCellsCount; i++) {
      Vector2 pos; 
      int num;
      in >> pos >> num;
      level.setCell(pos, num);
    }

    return level;
  }

  friend std::ostream& operator<<(std::ostream& os, const Level& level) {
    for (int i=0; i<level.size; i++) {
      for (int j=0; j<level.size; j++) {
        int tile = level.playingField[i][j];
        if (tile == -1)
          os << "# ";
        else if (tile == 0)
          os << ". ";
        else 
          os << tile << ' ';
      }
      os << '\n';
    }
    return os;
  }
  
};

int main(void) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }

  Level level = Level::createFromStream(std::cin);
  std::cout << level;
}
