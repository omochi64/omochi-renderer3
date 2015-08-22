#include "stdafx.h"

#include "IBLTestScene.h"
#include "renderer/AxisAlignedPlane.h"
#include "renderer/Model.h"
#include "renderer/Sphere.h"
#include "tools//HDRImage.h"
#include "renderer/IBL.h"

#include <iostream>

namespace OmochiRenderer {
  IBLTestScene::IBLTestScene() {
    /*Model *cube = new Model;
    if (!cube->ReadFromObj("input_data/torii.obj")) {
      std::cerr << "failed to load cube.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    cube->SetTransform(Vector3(50, 0, 0), Vector3(1,1,1));
    AddModel(cube);*/

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

    //AddFloorXZ_yUp(10000, 10000, Vector3::Zero(), Material(Material::REFLECTION_TYPE_LAMBERT, Vector3::Zero(), Vector3(1, 1, 1)));

    m_ibl.reset(new IBL("input_data/Barce_Rooftop_C_Env.hdr", 1000000.0, objectsCenter));

    ////AddObject(new Sphere(20, Vector3(65, 42, 230), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.25, 0.75, 0.25))));    // —Î‹…
    ////AddObject(new Sphere(16.5, Vector3(27, 42, 197), Material(Material::REFLECTION_TYPE_SPECULAR, Color(), Color(0.99, 0.99, 0.99))));   // ‹¾
    ////AddObject(new Sphere(16.5, Vector3(127, 42, 172), Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ƒKƒ‰ƒX

    //Model *cube = new Model;
    //if (!cube->ReadFromObj("input_data/table/Table and Glasses.obj", true)) {
    //  std::cerr << "failed to load cube.obj!!!" << std::endl;
    //  getchar();
    //  exit(-1);
    //}
    ////cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    //cube->Transform(Vector3(85, -10, 172), Vector3(160, 160, 160), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI));
    //AddModel(cube);

    //cube = new Model;
    //if (!cube->ReadFromObj("input_data/Revolver/Revolver.obj", true)) {
    //  std::cerr << "failed to load Revolver.obj!!!" << std::endl;
    //  getchar();
    //  exit(-1);
    //}
    ////cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    //cube->Transform(Vector3(85, 30, 142), Vector3(140, 140, 140), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI) * Matrix::RotateAroundVector(Vector3(0, 0, 1), -90.0 / 180 * PI));
    //AddModel(cube);

    //cube = new Model;
    //if (!cube->ReadFromObj("input_data/bullet/bullet.obj", true)) {
    //  std::cerr << "failed to load bullet.obj!!!" << std::endl;
    //  getchar();
    //  exit(-1);
    //}
    ////cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    //cube->Transform(Vector3(65, 25, 172), Vector3(1, 1, 1), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI));
    //AddModel(cube);

    //// floor
    ////AddFloorXZ_yUp(100.0, 100.0, objectsCenter,
    ////  Material(Material::REFLECTION_TYPE_LAMBERT, Vector3::Zero(), Vector3(0.7, 0.7, 0.7)));

    ////AddObject(new AxisAlignedPlane(Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.7, 0.7, 0.7)), AxisAlignedPlane::PLANE_XZ), true, false);


    ConstructQBVH();
  }
}
