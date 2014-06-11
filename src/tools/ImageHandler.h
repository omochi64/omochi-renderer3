#pragma once

#include "Image.h"

namespace OmochiRenderer
{
  class ImageHandler
  {
    ImageHandler() {}
    ~ImageHandler() {}
  public:
    static ImageHandler & GetInstance() {
      static ImageHandler s;
      return s;
    }

    Image * LoadFromFile(const std::string &fname, bool doReverseGamma = true) const;
    Image * CreateImage(size_t width, size_t height) const;
    void ReleaseImage(Image *p) const;
    bool SaveToPngFile(const std::string &fname, const Image *image) const;
    bool SaveToPpmFile(const std::string &fname, const Image *image) const;
  };
}
