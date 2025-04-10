#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;
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

  static const vector<Vector2> &AllDirections() {
    static const vector<Vector2> directions = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    return directions;
  }

  friend ostream &operator<<(ostream &os, const Vector2 &v) {
    return os << "(" << v.x << ", " << v.y << ")";
  }
};

class KnowledgeBase {
private:
  vector<vector<int>> hasBombVariables;
  map<int, Vector2> inverseLookupMap;
  int knowledgeBaseFile;
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

    char filename[] = "Minesweeper.XXXXXX";
    knowledgeBaseFile = mkstemp(filename);
    if (knowledgeBaseFile < 0) {
      string msg = "Error while creating temporary file: ";
      string error_msg = strerror(errno);
      throw runtime_error(msg+error_msg);
    }
  }

  KnowledgeBase(Vector2 mapSize) {
    KnowledgeBase(mapSize.x, mapSize.y);
  }

  ~KnowledgeBase() {
    close(knowledgeBaseFile);
  }
};

int main(void) {
  std::cout << "Hello";
  return 0;
}
