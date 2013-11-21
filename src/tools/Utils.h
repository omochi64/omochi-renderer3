#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace OmochiRenderer {

class Utils {
public:
  static std::vector<std::string> split(const std::string &str, char delim) {
    std::istringstream iss(str); std::string tmp; std::vector<std::string> res;
    while (std::getline(iss, tmp, delim)) res.push_back(tmp);
    return res;
  }

  static std::string ltrim(const std::string &target, const std::string &delimiters = std::string(" \t")) {
    int index = 0;
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
};

}
