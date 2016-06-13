#pragma once

#include <iostream>
#include <string>
#include "Color.h"
#include "tools/HDRImage.h"
#include "tools/Constant.h"

namespace OmochiRenderer {

  class IBL {
  public:

    IBL(const std::string &hdr_filename, const double radius = 1000000.0, const Vector3 &centerPosition = Vector3::Zero())
      : m_image()
      , m_radius(radius)
      , m_center(centerPosition)
    {
      if (!m_image.ReadFromRadianceFile(hdr_filename)) {
        std::cerr << "failed to load HDR file: " << hdr_filename << std::endl;
        return;
      }

      //CreateImportanceSamplingMap();
    }

    inline const Color &Sample(const Ray &ray) const {
      return Sample(ConvertRayToNormalizedDir(ray));
    }
    inline const Color &Sample(const Vector3 &dir) const {
      // (x, y, z) = (r*sin(theta)*cos(phi), r*cos(theta), r*sin(theta)*sin(phi))
      const double theta = acos(dir.y);
      double phi = acos(dir.x / sqrt(dir.x*dir.x + dir.z*dir.z));
      if (dir.z < 0.0) {
        phi = 2.0 * PI - phi;
      }

      double u, v;
      PhiThetaToUV(phi, theta, u, v);

      return m_image.GetPixel(
        static_cast<int>(u * m_image.GetWidth()) % m_image.GetWidth(),
        static_cast<int>(v * m_image.GetHeight()) % m_image.GetHeight()
      );
    }

    inline const Color &SampleOriginal(const Vector3 &dir) {
      const float r = static_cast<float>((1.0f / PI) * acos(dir.z) / sqrt(dir.x * dir.x + dir.y * dir.y));

      float u = static_cast<float>((dir.x * r + 1.0f) / 2.0f);
      float v = static_cast<float>((dir.y * r + 1.0f) / 2.0f);
      //float v = 1 - (dir.y * r + 1.0f) / 2.0f;

      if (u < 0.0f)
        u += 1.0f;
      if (v < 0.0f)
        v += 1.0f;

      const int x = (int)(u * m_image.GetWidth()) % m_image.GetWidth();
      const int y = static_cast<int>(m_image.GetHeight()) - 1 - (int)(v * m_image.GetHeight()) % m_image.GetHeight();

      return m_image.GetPixel(x, y);
    }

  private:

    static void PhiThetaToUV(const double phi, const double theta, double &u, double &v) {
      u = phi / (2.0 * PI);
      //const double v = 1.0 - theta / PI;
      v = theta / PI;
    }
    static void UVToPhiTheta(const double u, const double v, double &phi, double &theta) {
      phi = u * (2.0 * PI);
      theta = v * PI;
    }

    Vector3 ConvertRayToNormalizedDir(const Ray &ray) const {
      const Vector3 &x = ray.orig;
      const Vector3 &v = ray.dir;
      const Vector3 &c = m_center;

      // ���ʎ�
      Vector3 c_minus_x = c - x;
      double v_dot_c_minus_x = v.dot(c_minus_x);
      double D1 = v_dot_c_minus_x * v_dot_c_minus_x;
      double D2 = v.lengthSq() * (c_minus_x.lengthSq() - m_radius*m_radius);
      double D = D1 - D2;
      assert(D >= 0);

      double sqrtD = sqrt(D);
      double t1 = sqrtD + v_dot_c_minus_x;
      double t2 = -sqrtD + v_dot_c_minus_x;

      if (t1 <= EPS && t2 <= EPS) return ray.dir;

      double distance;
      if (t2 > EPS) distance = t2;
      else distance = t1;

      Vector3 res(x + v * distance); res.normalize();
      return res;
    }

    void CreateImportanceSamplingMap() {
      return;

      std::vector<Color> xyToEnv(m_image.GetWidth()*m_image.GetHeight());

      size_t width = m_image.GetWidth();
      size_t height = m_image.GetHeight();

      // (x, y) �s�N�Z�� => cos�����l�������T���v���l �ɕϊ�
      // �e�����ւ̕��ˋP�x���Atheta = a (a�͔C��) ���猩�����̕��ˋP�x�ɕϊ����Ă���
      // E_{��=a} = E_��(cos��)*cos(��-a)
      // 2v-1 = cos�� �ƕϐ��ϊ� (pdf ���v�Z����Ƃ��ɂ��̌v�Z��e�Ղɂ��邽��)
      //  E_{��=a} = E_��(2v-1)*cos(��-a)
      // ����� E_��(2v-1) ���e�[�u���ɂ��Ă��� (cos(��-a)�̕����́A���ۂɃT���v�����O����Ƃ��Ɍv�Z����)
      for (size_t yi = 0; yi < height; yi++) {
        for (size_t xi = 0; xi < width; xi++) {
          const double u = (xi + 0.5) / width;
          const double v = (yi + 0.5) / height;

          const double phi = u * 2.0 * PI;
          const double y = 2 * v - 1.0;
          //double phi, theta;
          //UVToPhiTheta(u, v, phi, theta);
          //const double y = cos(theta);

          xyToEnv[yi*width + xi] = Sample(Vector3(
            sqrt(1.0 - y*y) * cos(phi),
            y,
            sqrt(1.0 - y*y) * sin(phi)
            ));
        }
      }

      const double width_scale = 1.0 * width / m_importance_map_size;
      const double height_scale = 1.0 * height / m_importance_map_size;

      m_luminance_map.clear(); m_luminance_map.resize(m_importance_map_size * m_importance_map_size);
      m_direction_map.clear(); m_direction_map.resize(m_importance_map_size * m_importance_map_size);

      // importance map �̍쐬
      // map �� index ���� accumulated luminance �� ray direction ��������悤�ɂ��Ă���
      for (size_t yi = 0; yi < m_importance_map_size; yi++) {
        for (size_t xi = 0; xi < m_importance_map_size; xi++) {
          const double u = (xi + 0.5) / m_importance_map_size;
          const double v = (yi + 0.5) / m_importance_map_size;

          //double phi, theta;
          //UVToPhiTheta(u, v, phi, theta);
          //double y = cos(theta);

          const double phi = u * 2.0 * PI;
          const double y = 2 * v - 1.0;   // cos��
          const Vector3 vec(sqrt(1.0 - y*y)*cos(phi), y, sqrt(1.0 - y*y)*sin(phi));

          size_t index = yi * m_importance_map_size + xi;

          m_direction_map[index] = vec;

          Color tmp;
          for (int yi_ = static_cast<int>(yi * height_scale); yi_ < static_cast<int>((yi + 1)*height_scale); yi_++) {
            for (int xi_ = static_cast<int>(xi * width_scale); xi_ < static_cast<int>((xi + 1)*width_scale); xi_++) {
              tmp += xyToEnv[yi_ * width + xi_];
            }
          }
          m_luminance_map[index] = 0.298912 * tmp.x + 0.586611 * tmp.y + 0.114478 * tmp.z;
        }
      }

      // importance map �� (x,y) ��I�Ԋm�����x�֐���
      //  C * luminance_rate(x,y) (�ȍ~�Aluminance_rate(x,y) = l_{x,y})
      // �Ƃ���B�����ϕ�����
      //  C��l_{x,y}*sin��d��d�� (��:0->PI, ��:0->2PI)
      // �������A��L�ŕϊ����Ă���悤�ɁA
      //  cos�� = 2*v - 1, �� = 2*u*PI
      // �ƕϐ��ϊ�����ƁA
      //  4*PI*C��l_{x,y}*dudv (u:0->1, v:-0.5->0.5)
      // �ƂȂ�B�����ŁAl_{x,y}��u,v�ɂ��āA���ꂼ��1�ӂ� 1/map_size �Ƃ��鐳���`�̑�\�l�Ƃ��Ă���킳��Ă���
      // �܂�A���̗̈���ł͒l�����̂��߁A��L�̐ϕ���
      //  4PI*C��l_{(0.5+xi)/map_size, (0.5+yi)/map_size}*��u��v
      // �̂悤�ɒu����������B��u��v�� l_{x,y} ����\�l���Ƃ��Ă���̈�̖ʐςȂ̂ŁA 1/map_size^2 �ƂȂ�B
      // �܂��Al �͐��K�����ꂽ���̂��g���̂ŁA���̍��v�l��1�ƂȂ�B�܂�A
      //  C��l_{x,y}*sin��d��d�� = 4PI/map_size^2 * C
      // �ƂȂ�̂ŁA C = map_size^2/4PI �ŁA pdf = luminance_rate(x,y) * map_size^2 / 4PI �ƂȂ�B
      // ��L�� map �쐬���A y (=cos��) = 2v - 1 �Ƃ����A�� = v*PI �Ƃ���ƁA�܂��v�Z���قȂ��Ă���

    }

  private:
    HDRImage m_image;
    static const int m_importance_map_size = 64;
    std::vector<double> m_luminance_map;
    std::vector<Vector3> m_direction_map;

    double m_radius;
    Vector3 m_center;
  };
}