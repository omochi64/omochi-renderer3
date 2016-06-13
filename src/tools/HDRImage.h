#pragma once

#include "Image.h"

namespace OmochiRenderer {

  // reference: http://www.graphics.cornell.edu/online/formats/rgbe/
  class HDRImage : public Image {
  public:
    static void Color2RGBE(unsigned char rgbe[4],
      const Color &c) {
      float v;
      int e;
      v = static_cast<float>(std::max(std::max(c.x, c.y), c.z));

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
        float f = static_cast<float>(ldexp(1.0, rgbe[3] - (128 + 8)));
        c.x = rgbe[0] * f;
        c.y = rgbe[1] * f;
        c.z = rgbe[2] * f;
      }
      else {
        c = Color::Zero();
      }
    }

    // HDR 画像の読み込み
    bool ReadFromRadianceFile(const std::string &file);
    // HDR 画像の書き込み
    bool WriteToRadianceFile(const std::string &file);

   
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

    // 読み込みのための内部関数
    bool ReadHeaderFromRadianceFile(std::ifstream &ifs, RGBE_Header &header_result);
    bool ReadPixelsFromRadianceFile(std::ifstream &ifs, int pixel_count, int startoffset = 0);
    bool ReadRLEPixelsFromRadianceFile(std::ifstream &ifs, int width, int height);

    // 書き込みのための内部関数
    bool WriteHeaderToRadianceFile(std::ofstream &ifs, const RGBE_Header &header);
    bool WritePixelsToRadianceFile(std::ofstream &ofs, const Color *data, int numpixels);
    bool WriteBytes_RLE(std::ofstream &ofs, unsigned char *data, int numbytes);
    bool WritePixels_RLE(std::ofstream &ofs, const Color *data, int scanline_width, int num_scanlines);

  private:
    RGBE_Header m_imageInfo;
  };

}
