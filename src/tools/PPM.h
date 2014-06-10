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
          ss << Utils::ToRgb(Utils::GammaRev(image[index].x)) << " " <<
            Utils::ToRgb(Utils::GammaRev(image[index].y)) << " " <<
            Utils::ToRgb(Utils::GammaRev(image[index].z)) << std::endl;
        }
        lines[y] = ss.str();
      }
    }

    for (int i = 0; i < height; i++) {
      ofs << lines[i];
    }
	}
};

}
