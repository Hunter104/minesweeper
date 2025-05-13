#pragma once
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <istream>
#include <unistd.h>
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

  friend std::istream &operator>>(std::istream &in, Vector2 &v) {
    return in >> v.y >> v.x;
  }

  static Vector2 getRandom(int max_x, int max_y) {
    return {rand() % max_x, rand() % max_y};
  }
};

namespace std {
template <> struct hash<Vector2> {
  size_t operator()(const Vector2 &v) const {
    return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
  }
};
}; // namespace std
