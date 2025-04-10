#include <iostream>
#include <vector>

using namespace std;

struct Vector2 {
  int x, y;

  Vector2() = default;
  Vector2(int x, int y) : x(x), y(y) {}
  Vector2(Vector2& other) : x(other.x), y(other.y) {}

  Vector2 operator+(const Vector2& other) {
    return Vector2(x+other.x, y+other.y);
  }

  Vector2 operator-(const Vector2& other) {
    return Vector2(x-other.x, y-other.y);
  }

  Vector2 operator*(const int other) {
    return Vector2(x*other, y*other);
  }

  void operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
  }

  void operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
  }

  void operator *=(const int other) {
    x *= other;
    y *= other;
  }

  bool operator==(const Vector2& other) const {
      return x == other.x && y == other.y;
  }

  bool operator!=(const Vector2& other) const {
      return !(*this == other);
  }


  static const vector<Vector2>& AllDirections() {
      static const vector<Vector2> directions = {
          {-1, -1}, {-1, 0}, {-1, 1},
          { 0, -1},          { 0, 1},
          { 1, -1}, { 1, 0}, { 1, 1}
      };
      return directions;
  }
};

int main (int argc, char *argv[]) {
  std::cout << "Hello";
  return 0;
}
