#include "stdafx.h"

#include "SceneForRaytracingCamp3.h"
#include "renderer/AxisAlignedPlane.h"
#include "renderer/Model.h"
#include "renderer/Sphere.h"
#include "tools/HDRImage.h"
#include "renderer/IBL.h"

#include <iostream>

namespace OmochiRenderer {
  SceneForRaytracingCamp3::SceneForRaytracingCamp3() {
    const Vector3 objectsCenter(0, 0, 0);

    std::vector<std::string> files;
    files.push_back("input_data/marble1.png");
    files.push_back("input_data/marble2.jpg");
    files.push_back("input_data/marble3.jpg");
    files.push_back("input_data/marble4.jpg");

    double refraction_rate = 2.0;
    double radius = 17.0;
    double haba = 5;

    Vector3 posOffset(50 - (radius+haba) * 2 * 5, 0, -150);

    int size = files.size();
    for (int texIdx = -10; texIdx < size; texIdx++)
    {
      double z = texIdx * (radius + haba) * 2.0 * 2.0;
      int trueTexIdx = texIdx;
      while (trueTexIdx < 0) trueTexIdx += size;
      trueTexIdx %= size;

      ImageHandler::IMAGE_ID tex_id = ImageHandler::GetInstance().LoadFromFile(files[trueTexIdx]);
      for (int rateIdx = -15; rateIdx <= 25; rateIdx++)
      {
        double rate1 = rateIdx*0.1;
        if (rate1 < 0) rate1 = -rate1;
        if (rate1 > 1) rate1 = 2 - rate1;
        rate1 = std::max(std::min(rate1, 1.0), 0.0);
        double rate2 = 1 - rate1;

        double x = rateIdx * (radius + haba) * 2.0;

        Vector3 pos1(x, radius, z), pos2(x, radius, z + (radius + haba) * 2);

        // Diffuse + Refraction
        {
          Material mat1(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(1, 1, 1), 0, tex_id);
          Material mat2(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(1, 1, 1), refraction_rate, ImageHandler::INVALID_IMAGE_ID);

          auto obj = new Sphere(radius, pos1 + posOffset, mat1);
          obj->GetMaterial(0)->rate_ = rate1;
          obj->AddMaterial(mat2, rate2);
          AddObject(obj, true, true);
        }

        // Diffuse + Reflection
        {
          Material mat1(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(1, 1, 1), 0, tex_id);
          Material mat2(Material::REFLECTION_TYPE_SPECULAR, Color(), Color(1, 1, 1), refraction_rate, ImageHandler::INVALID_IMAGE_ID);

          auto obj = new Sphere(radius, pos2 + posOffset, mat1);
          obj->GetMaterial(0)->rate_ = rate1;
          obj->AddMaterial(mat2, rate2);
          AddObject(obj, true, true);
        }

      }
    }

    AddInfiniteFLoor(0, Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(1, 1, 1)));

    m_ibl.reset(new IBL("input_data/Barce_Rooftop_C_Env.hdr", 1000000.0, objectsCenter));


    ConstructQBVH();
  }
}
