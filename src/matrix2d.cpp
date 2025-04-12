#pragma once
#include "vector2.cpp"
#include <vector>

template <typename T> class Matrix2D {
  std::vector<std::vector<T>> data;

public:
  Matrix2D(int sizeX, int sizeY, T defaultVal = T())
      : data(sizeX, std::vector<T>(sizeY, defaultVal)) {}

  std::vector<T> &operator[](int x) { return data[x]; }

  const std::vector<T> &operator[](int x) const { return data[x]; }

  T &operator[](const Vector2 &v) { return data[v.x][v.y]; }

  const T &operator[](const Vector2 &v) const { return data[v.x][v.y]; }
};
