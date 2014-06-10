#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <algorithm>

namespace OmochiRenderer {

class Utils {
public:
  static std::vector<std::string> split(const std::string &str, char delim) {
    std::istringstream iss(str); std::string tmp; std::vector<std::string> res;
    while (std::getline(iss, tmp, delim)) res.push_back(tmp);
    return res;
  }

  static std::string ltrim(const std::string &target, const std::string &delimiters = std::string(" \t")) {
    size_t index = 0;
    while (target.length() > index && delimiters.find(target[index]) != std::string::npos) {
      index++;
    }
    if (target.length() <= index) return "";
    return target.substr(index);
  }

  static std::string rtrim(const std::string &target, const std::string &delimiters = std::string(" \t")) {
    int index = target.length()-1;
    while (index >= 0 && delimiters.find(target[index]) != std::string::npos) {
      index--;
    }
    if (index < 0) return "";
    return target.substr(0, index+1);
  }

  static std::string trim(const std::string &target, const std::string &delimiters = std::string(" \t")) {
    return ltrim(rtrim(target, delimiters), delimiters);
  }

  static std::string toupper(const std::string &str) {
    std::string newstr(str);
    std::transform(newstr.begin(), newstr.end(), newstr.begin(), std::toupper);
    return newstr;
  }

  static std::string tolower(const std::string &str) {
    std::string newstr(str);
    std::transform(newstr.begin(), newstr.end(), newstr.begin(), std::tolower);
    return newstr;
  }

  static Vector3 splitVector(const std::string &str) {
    std::vector<std::string> data(split(str, ','));
    Vector3 vec;

    if (data.size() >= 1) vec.x = atof(trim(data[0]).c_str());
    if (data.size() >= 2) vec.y = atof(trim(data[1]).c_str());
    if (data.size() >= 3) vec.z = atof(trim(data[2]).c_str());

    return vec;
  }

  static bool parseBoolean(const std::string &str) {
    if (Utils::tolower(str) == "false" || str == "0" || str.empty()) {
      return false;
    } else {
      return true;
    }
  }

  static double Clamp(double x) {
    if (x<0.0) return 0.0;
    if (x>1.0) return 1.0;
    return x;
  }

  static double GammaRev(double x) {
    return pow(Utils::Clamp(x), 1.0 / 2.2);
  }

  static double InvGammaRev(double x) {
    return pow(Utils::Clamp(x), 2.2);
  }

  static int ToRgb(double v_0_1) {
    return static_cast<int>(v_0_1 * 255 + 0.5);
  }
};

}
