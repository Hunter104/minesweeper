#include <cerrno>
#include <list>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
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
};

class KnowledgeBase {
private:
  std::vector<std::vector<int>> hasBombVariables;
  std::map<int, Vector2> inverseLookupMap;
  std::list<std::list<int>> clauses;

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

int main(void) {
  if (system("which clasp > /dev/null 2>&1")) {
    std::cerr << "Could not find clasp command, are you sure it is installed?\n";
    return 1;
  }
  return 0;
}
