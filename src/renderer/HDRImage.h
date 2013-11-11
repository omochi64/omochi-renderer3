#pragma once

#include "Color.h"
#include <vector>
#include <algorithm>

namespace OmochiRenderer {

  class HDRImage {
  public:
    static void Float2RGBE(unsigned char rgbe[4],
      float r, float g, float b) {
      float v;
      int e;
      v = std::max(std::max(r, g), b);

      if (v < 1e-32) {
        for (int i = 0; i < 4; i++) rgbe[i] = 0;
      } else {
        v = frexp(v, &e) * 256.0/v;
        rgbe[0] = static_cast<unsigned char>(r * v);
        rgbe[1] = static_cast<unsigned char>(g * v);
        rgbe[2] = static_cast<unsigned char>(b * v);
        rgbe[3] = static_cast<unsigned char>(e + 128);
      }
    }

    static void RGBE2Float(float rgb[3], unsigned char rgbe[4]) {
      if (rgbe[3]) {
        float f = ldexp(1.0, rgb[3] - (128 + 8));
        for (int i = 0; i < 3; i++) { rgb[i] = rgbe[i] * f; }
      }
      else {
        rgb[0] = rgb[1] = rgb[2] = 0.0f;
      }
    }

  private:
    std::vector<Color> m_image;
  };

}