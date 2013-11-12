#pragma once

#include "Color.h"
#include <vector>
#include <algorithm>

namespace OmochiRenderer {

  class HDRImage {
  public:
    static void Color2RGBE(unsigned char rgbe[4],
      const Color &c) {
      float v;
      int e;
      v = std::max(std::max(c.x, c.y), c.z);

      if (v < 1e-32) {
        for (int i = 0; i < 4; i++) rgbe[i] = 0;
      } else {
        v = frexp(v, &e) * 256.0f/v;
        rgbe[0] = static_cast<unsigned char>(c.x * v);
        rgbe[1] = static_cast<unsigned char>(c.y * v);
        rgbe[2] = static_cast<unsigned char>(c.z * v);
        rgbe[3] = static_cast<unsigned char>(e + 128);
      }
    }

    static void RGBE2Color(unsigned char rgbe[4], Color &c) {
      if (rgbe[3]) {
        float f = ldexp(1.0, rgbe[3] - (128 + 8));
        c.x = rgbe[0] * f;
        c.y = rgbe[1] * f;
        c.z = rgbe[2] * f;
      }
      else {
        c = Color::Zero();
      }
    }

    bool ReadFromRadianceFile(const std::string &file);

    size_t GetHeight() const;
    size_t GetWidth() const;
    const Color &GetPixel(size_t x, size_t y) const;

  private:
    struct RGBE_Header {
      int valid;
      std::string programtype;
      float gamma;
      float exposure;
      int height, width;
      RGBE_Header()
        : valid(0)
        , programtype()
        , gamma(.0f)
        , exposure(.0f)
        , height(0), width(0)
      {
      }

      const static int RGBE_VALID_PROGRAMTYPE = 0x01;
      const static int RGBE_VALID_GAMMA = 0x02;
      const static int RGBE_VALID_EXPOSURE = 0x04;
    };

    bool ReadHeaderFromRadianceFile(std::ifstream &ifs, RGBE_Header &header_result);
    bool ReadPixelsFromRadianceFile(std::ifstream &ifs, int pixel_count, int startoffset = 0);
    bool ReadRLEPixelsFromRadianceFile(std::ifstream &ifs, int width, int height);

  private:
    std::vector<Color> m_image;
    RGBE_Header m_imageInfo;
  };

}