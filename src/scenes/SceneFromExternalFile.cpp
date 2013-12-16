#include "SceneFromExternalFile.h"
#include "tools/Utils.h"
#include "renderer/Sphere.h"
#include "renderer/SphereLight.h"

#include <fstream>

using namespace std;

namespace OmochiRenderer {
  SceneFromExternalFile::SceneFromExternalFile(const string &file)
    : m_isValid(false)
    , m_baseDir()
    , m_iblFileName()
    , m_spacePartitioningMethod()
  {
    m_isValid = ReadFromFile(file);

    if (m_spacePartitioningMethod == "BVH") {
      ConstructBVH();
    } else if (m_spacePartitioningMethod == "QBVH") {
      ConstructQBVH();
    }
  }
  
  SceneFromExternalFile::~SceneFromExternalFile()
  {
    m_isValid = false;
  }

  bool SceneFromExternalFile::ReadFromFile(const std::string &file) {
    ifstream ifs(file.c_str());

    if (!ifs || ifs.bad() || ifs.eof()) return false;

    const char DataDelimiter(':');
    const std::string BeginObjectIdentifier("Objects:");
    const char CommentDelimiter('#');
    int line_number = 1;

    std::vector<LinePair> readLines;
    //
    // Reading header area
    // 
    do {
      string line;
      std::getline(ifs, line);
      line = Utils::ltrim(line);

      if (line == BeginObjectIdentifier) {
        break;
      }
      if (!line.empty() && line.find(CommentDelimiter) != 0) {
        vector<string> data(Utils::split(line, DataDelimiter));
        if (data.size() == 1) readLines.push_back(std::pair<string, string>(Utils::trim(data[0]), ""));
        else if (data.size() >= 2)
          readLines.push_back(std::pair<string, string>(Utils::trim(data[0]), Utils::trim(data[1])));
      }
      line_number++;
    } while (!ifs.eof());

    if (ifs.eof()) return false;
    if (!ReadHeader(readLines)) return false;


    //
    // Read Objects in this scene
    //
    enum OBJ_TYPE {
      MESH, SPHERE, FLOOR, SPHERE_LIGHT, NONE
    };
    OBJ_TYPE type = NONE;
    bool isInObjectDefineSection = false;
    const char ObjectDefineBeginDelimiter('{');
    const char ObjectDefineEndDelimiter('}');
    readLines.clear();
    do {
      string line;
      std::getline(ifs, line);
      line = Utils::ltrim(line);
      if (line.empty()) {
        line_number++;
        if (ifs.eof()) break;
        continue;
      }

      if (line.find(ObjectDefineBeginDelimiter) != string::npos) {
        if (isInObjectDefineSection) {
          cerr << "Nested Object Definition is not allowded!!: line " << line_number << endl;
          return false;
        }
        // get the object type
        string delim; delim.resize(1,ObjectDefineBeginDelimiter);
        string type_str = Utils::rtrim(Utils::rtrim(line, delim));
        if (type_str == "Obj Mesh") type = MESH;
        else if (type_str == "Sphere") type = SPHERE;
        else if (type_str == "Floor") type = FLOOR;
        else if (type_str == "SphereLight") type = SPHERE_LIGHT;

        if (type == NONE) {
          cerr << "Undefined object type: " << type_str << " line " << line_number << endl;
          return false;
        }
        isInObjectDefineSection = true;
      } else if (line.find(ObjectDefineEndDelimiter) == 0) {
        if (!isInObjectDefineSection) {
          cerr << "Inconsistent delimiter: line " << line_number << endl;
          return false;
        }

        bool ret = false;
        switch (type) {
        case MESH:
          ret = ReadMesh(readLines); break;
        case FLOOR:
          ret = ReadFloor(readLines); break;
        case SPHERE:
        case SPHERE_LIGHT:
          ret = ReadSphere(readLines, type == SPHERE_LIGHT); break;
        }
        if (!ret) {
          cerr << "failed to load object: line " << line_number << endl;
          return false;
        }
        type = NONE;
        isInObjectDefineSection = false;
        readLines.clear();
      }
      else {
        if (isInObjectDefineSection) {
          vector<string> data(Utils::split(line, DataDelimiter));
          if (data.size() == 1) readLines.push_back(std::pair<string, string>(Utils::trim(data[0]), ""));
          else if (data.size() >= 2)
            readLines.push_back(std::pair<string, string>(Utils::trim(data[0]), Utils::trim(data[1])));
        }
      }

      line_number++;
    } while (!ifs.eof());


    return true;
  }

  bool SceneFromExternalFile::ReadHeader(const std::vector <LinePair> &lines) {
    // IBL
    // Construct BVH/QBVH

    m_iblFileName = m_spacePartitioningMethod = "";

    std::vector<LinePair>::const_iterator it, end = lines.end();
    for (it = lines.begin(); it != end; it++) {
      if (it->first == "Base Dir" || it->first == "BaseDir") {
        m_baseDir = it->second;
        if (m_baseDir.length() > 0 && m_baseDir.back() != '/') {
          m_baseDir.push_back('/');
        }
      } else if (it->first == "IBL") {
        m_iblFileName = m_baseDir + it->second;
        m_ibl.reset(new IBL(m_iblFileName));
      } else if (it->first == "Space Partitioning") {
        m_spacePartitioningMethod = it->second;
      }
      else {
        cerr << "undefined header information: " << it->first << endl;
        return false;
      }
    }
    return true;
  }

  bool SceneFromExternalFile::ParseMaterial(const std::string &data_, Material &mat) {
    // emission, REFLECTION_TYPE, color(, refraction_rate)
    vector<string> data(Utils::split(data_, ','));

    if (data.size() < 7) {
      cerr << "insufficient material definition: " << data_ << endl;
      return false;
    }

    Vector3 emission;
    emission.x = atof(Utils::trim(data[0]).c_str());
    emission.y = atof(Utils::trim(data[1]).c_str());
    emission.z = atof(Utils::trim(data[2]).c_str());

    string type = Utils::trim(data[3]);
    Material::REFLECTION_TYPE reflection_type = Material::REFLECTION_TYPE_LAMBERT;
    if (type == "LAMBERT") {
      reflection_type = Material::REFLECTION_TYPE_LAMBERT;
    } else if (type == "SPECULAR") {
      reflection_type = Material::REFLECTION_TYPE_SPECULAR;
    } else if (type == "REFRACTION") {
      reflection_type = Material::REFLECTION_TYPE_REFRACTION;
    } else {
      cerr << "undefined reflection type: " << type << endl;
      return false;
    }

    Vector3 color;
    color.x = atof(Utils::trim(data[4]).c_str());
    color.y = atof(Utils::trim(data[5]).c_str());
    color.z = atof(Utils::trim(data[6]).c_str());

    double refract_rate = 0.0;
    if (reflection_type == Material::REFLECTION_TYPE_REFRACTION) {
      if (data.size() < 8) {
        cerr << "reflectance is not defined: " << data_ << endl;
        return false;
      }
      refract_rate = atof(Utils::trim(data[7]).c_str());
    }

    mat.color = color;
    mat.emission = emission;
    mat.reflection_type = reflection_type;
    mat.refraction_rate = refract_rate;

    return true;
  }

  bool SceneFromExternalFile::ReadSphere(const std::vector<LinePair> &lines, bool isLight) {
    double radius = 0.0;
    Vector3 position;
    bool valid = true;
    bool inSpacePartitioning = true;
    Material newMat;

    std::for_each(lines.begin(), lines.end(), [&] (const LinePair &it) {
      if (it.first == "Radius") {
        radius = atof(it.second.c_str());
      } else if (it.first == "Position") {
        vector<string> pos_str(Utils::split(it.second, ','));
        if (pos_str.size() != 3) {
          valid = false; return;
        }
        position.x = atof(Utils::trim(pos_str[0]).c_str());
        position.y = atof(Utils::trim(pos_str[1]).c_str());
        position.z = atof(Utils::trim(pos_str[2]).c_str());
      } else if (it.first == "Material") {
        if (!ParseMaterial(it.second, newMat)) {
          valid = false;
        }
      } else if (it.first == "Space Partitioning") {
        if (it.second == "True") {
          inSpacePartitioning = true;
        } else if (it.second == "False") {
          inSpacePartitioning = false;
        }
      }
    });

    if (!valid) return false;

    if (isLight) {
      AddObject(new SphereLight(radius, position, newMat), true, inSpacePartitioning);
    } else {
      AddObject(new Sphere(radius, position, newMat), true, inSpacePartitioning);
    }
    return true;
  }

  bool SceneFromExternalFile::ReadFloor(const std::vector<LinePair> &lines) {
    double size_x = 0, size_z = 0;
    bool valid = true;
    Vector3 position;
    bool inSpacePartitioning = true;
    Material newMat;

    std::for_each(lines.begin(), lines.end(), [&](const LinePair &it) {
      if (it.first == "Size") {
        vector<string> pos_str(Utils::split(it.second, ','));
        if (pos_str.size() != 2) {
          valid = false; return;
        }
        size_x = atof(Utils::trim(pos_str[0]).c_str());
        size_z = atof(Utils::trim(pos_str[1]).c_str());
      } else if (it.first == "Position") {
        vector<string> pos_str(Utils::split(it.second, ','));
        if (pos_str.size() != 3) {
          valid = false; return;
        }
        position.x = atof(Utils::trim(pos_str[0]).c_str());
        position.y = atof(Utils::trim(pos_str[1]).c_str());
        position.z = atof(Utils::trim(pos_str[2]).c_str());
      } else if (it.first == "Material") {
        if (!ParseMaterial(it.second, newMat)) {
          valid = false;
        }
      } else if (it.first == "Space Partitioning") {
        if (it.second == "True") {
          inSpacePartitioning = true;
        } else if (it.second == "False") {
          inSpacePartitioning = false;
        }
      }
    });

    if (!valid) return false;

    AddFloor(size_x, size_z, position, newMat);

    return true;
  }

  bool SceneFromExternalFile::ReadMesh(const std::vector<LinePair> &lines) {
    return false;
  }
}
