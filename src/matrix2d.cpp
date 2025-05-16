#pragma once
#include "vector2.cpp"
#include <vector>

template <typename T> class Matrix2D {
  std::vector<std::vector<T>> data;

public:
  Matrix2D(int sizeX, int sizeY, T defaultVal = T())
      : data(sizeX, std::vector<T>(sizeY, defaultVal)) {}

  std::vector<T> &operator[](int x) {
    return data.at(x); // throws std::out_of_range if x is invalid
  }

  const std::vector<T> &operator[](int x) const { return data.at(x); }

  T &operator[](const Vector2 &v) { return data.at(v.x).at(v.y); }

  const T &operator[](const Vector2 &v) const { return data.at(v.x).at(v.y); }
};
