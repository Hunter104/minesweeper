#include <iostream>
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
int main(int argc, char *argv[]) {
  std::cout << "Hello";
  return 0;
}
