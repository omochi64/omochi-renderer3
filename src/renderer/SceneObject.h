#pragma once

#include <vector>

#include "Color.h"
#include "Material.h"
#include "BoundingBox.h"

namespace OmochiRenderer {

class Ray;
class HitInformation;

class SceneObject {
public:
  // 複数マテリアルを扱う際の1マテリアルとブレンド割合
  struct MultiMaterialElem {
    Material material_;
    double rate_;
    MultiMaterialElem()
      : material_()
      , rate_(1.0)
    {
    }

    MultiMaterialElem(const Material &mat, double rate = 1.0)
      : material_(mat)
      , rate_(rate)
    {}
  };

public:
  explicit SceneObject(const Material &material)
    : materials_()
    , position_(0,0,0)
  {
    AddMaterial(material, 1.0f, false);
  }
  explicit SceneObject()
    : materials_()
    , position_(0, 0, 0)
  {
  }
  virtual ~SceneObject() {}

  virtual bool CheckIntersection(const Ray &ray, HitInformation &hit) const = 0;

  void AddMaterial(const Material &material, double rate, bool doNormalizeRate = true)
  {
    MultiMaterialElem mat(material, rate);
    materials_.push_back(mat);

    // 既存のマテリアルとのrate合計が1になるように正規化する
    if (doNormalizeRate)
    {
      double total = 0;
      for (auto &mat : materials_)
      {
        total += mat.rate_;
      }
      for (auto &mat : materials_)
      {
        mat.rate_ /= total;
      }
    }

  }

  std::vector<MultiMaterialElem> materials_;

  //Material material;
  Vector3 position_;
  BoundingBox boundingBox_;
};

}
