#pragma once

#include "Scene.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace OmochiRenderer {
  class SceneFromExternalFile : public Scene {
  public:
    explicit SceneFromExternalFile(const std::string &sceneFile = "input_data/default.scene");
    virtual ~SceneFromExternalFile();

    virtual bool IsValid() const { return m_isValid; }

  private:

    typedef std::pair<std::string, std::string> LinePair;

    bool ReadFromFile(const std::string &file);
    bool ReadHeader(const std::vector<LinePair> &lines);
    bool ReadMesh(const std::vector<LinePair> &lines);
    bool ReadSphere(const std::vector<LinePair> &lines);
    bool ReadFloor(const std::vector<LinePair> &lines);

    bool ParseMaterial(const std::string &data, Material &mat);

  private:
    bool m_isValid;

    std::string m_baseDir;
    std::string m_iblFileName;
    std::string m_spacePartitioningMethod;
  };
}