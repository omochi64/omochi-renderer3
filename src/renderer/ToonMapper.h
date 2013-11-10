#pragma once

namespace OmochiRenderer {

  class ToonMapper {
  public:
    virtual ~ToonMapper(){};

    virtual unsigned char Map(double value) const = 0;

  protected:
    static double Clamp0_1(double x) {
      if (x<0.0) return 0.0;
      if (x>1.0) return 1.0;
      return x;
    }
  };

}
