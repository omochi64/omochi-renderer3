#pragma once

#include "ToonMapper.h"
#include <cmath>

namespace OmochiRenderer {

  class LinearGammaToonMapper : public ToonMapper {
  public:
    unsigned char Map(double x) const {
		  return static_cast<unsigned char>(pow(Clamp0_1(x), 1.0/2.2) * 255 + 0.5);
	  }
  };

}
