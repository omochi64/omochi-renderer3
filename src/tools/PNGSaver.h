#pragma once

#include <ctime>

#include "FileSaver.h"
#include "ImageHandler.h"

namespace OmochiRenderer {

  class PNGSaver : public FileSaver {
  public:
    explicit PNGSaver(std::shared_ptr<Settings> settings)
      : FileSaver(settings)
    {
    }
    virtual ~PNGSaver()
    {
    }

    virtual void Save(int samples, int saveCount, const Color *img, double accumulatedPastTime);
  };

}