#pragma once

#include "Image.h"

namespace OmochiRenderer
{
  class ExternalFileImageHandler
  {
    ExternalFileImageHandler() {}
    ~ExternalFileImageHandler() {}
  public:
    static ExternalFileImageHandler & GetInstance() {
      static ExternalFileImageHandler s;
      return s;
    }

    Image * LoadFromFile(const std::string &fname, bool doReverseGamma = true);
    void ReleaseImage(Image *p);
    bool SaveToFile(const std::string &fname);
  };
}