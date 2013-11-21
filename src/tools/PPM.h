#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <sstream>
#include "renderer/Color.h"

using namespace std;

namespace OmochiRenderer {

class PPM {
private:
	static double Clamp(double x) {
		if (x<0.0) return 0.0;
		if (x>1.0) return 1.0;
		return x;
	}
	static double GammaRev(double x) {
		return pow(Clamp(x), 1.0/2.2);
	}
	static int ToRgb(double v_0_1) {
		return static_cast<int>(v_0_1 * 255 + 0.5);
	}
public:
	static void Save(const std::string &filename, const Color *image, const int width, const int height) {
		std::ofstream ofs(filename.c_str());
		ofs << "P3" << std::endl;
		ofs << width << " " << height << std::endl;
		ofs << 255 << std::endl;
    vector<string> lines(height);

    {
#pragma omp parallel for schedule(dynamic, 1)
      for (int y = 0; y < height; y++) {
        std::stringstream ss;
        for (int x = 0; x < width; x++) {
          int index = x + y*width;
          ss << ToRgb(GammaRev(image[index].x)) << " " << ToRgb(GammaRev(image[index].y)) << " " << ToRgb(GammaRev(image[index].z)) << std::endl;
        }
        lines[y] = ss.str();
      }
    }

    for (size_t i = 0; i < height; i++) {
      ofs << lines[i];
    }
	}
};

}
