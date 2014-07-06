#pragma once

#include <cstring>
#include "Vector.h"

namespace OmochiRenderer {

class Matrix {
public:
  Matrix()
  {
    memset(m, 0, sizeof(m));
  }
  explicit Matrix(const double m_[4][4]) {
    memcpy(m, m_, sizeof(m));
  }

  static Matrix RotateAroundVector(const Vector3 &vec_, double shita) {
    Matrix m;
    double cos_shita = cos(shita);
    double sin_shita = sin(shita);
    double one_minus_cos = 1-cos_shita;
    Vector3 vec(vec_); vec.normalize();

    m.m[0][0] = vec.x*vec.x*one_minus_cos + cos_shita; m.m[0][1] = vec.x*vec.y*one_minus_cos - vec.z*sin_shita; m.m[0][2] = vec.z*vec.x*one_minus_cos + vec.y*sin_shita;
    m.m[1][0] = vec.x*vec.y*one_minus_cos + vec.z*sin_shita; m.m[1][1] = vec.y*vec.y*one_minus_cos + cos_shita; m.m[1][2] = vec.z*vec.y*one_minus_cos - vec.x*sin_shita;
    m.m[2][0] = vec.x*vec.z*one_minus_cos - vec.y*sin_shita; m.m[2][1] = vec.z*vec.y*one_minus_cos + vec.x*sin_shita; m.m[2][2] = vec.z*vec.z*one_minus_cos + cos_shita;
    m.m[0][3] = m.m[1][3] = m.m[2][3] = m.m[3][2] = m.m[3][1] = m.m[3][0] = 0.0;
    m.m[3][3] = 1.0;

    return m;
  }

  Vector3 Apply(const Vector3 &vec) const {
    Vector3 ret;

    ret.x = m[0][0]*vec.x + m[0][1]*vec.y + m[0][2]*vec.z;
    ret.y = m[1][0]*vec.x + m[1][1]*vec.y + m[1][2]*vec.z;
    ret.z = m[2][0]*vec.x + m[2][1]*vec.y + m[2][2]*vec.z;

    return ret;
  }

  static const Matrix &Identity() {
    const static double m_[4][4] ={
      {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}
    };
    static Matrix i(m_);
    return i;
  };

  Matrix operator *(const Matrix &rhs) const
  {
    Matrix res;

    for (int row = 0; row < 4; row++)
    {
      for (int column = 0; column < 4; column++)
      {
        res.m[row][column] = 0;
        for (int k = 0; k < 4; k++)
        {
          res.m[row][column] += m[row][k] * rhs.m[k][column];
        }
      }
    }

    return res;
  }

private:
  double m[4][4];
};

}
