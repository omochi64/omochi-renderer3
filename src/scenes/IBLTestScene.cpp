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
    //HDRImage hdr;
    //hdr.ReadFromRadianceFile("input_data/grace_probe.hdr");

    /*Model *cube = new Model;
    if (!cube->ReadFromObj("input_data/torii.obj")) {
      std::cerr << "failed to load cube.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    cube->SetTransform(Vector3(50, 0, 0), Vector3(1,1,1));
    AddModel(cube);*/

    const Vector3 objectsCenter(0, 0, 0);

    AddFloorXZ_yUp(10000, 10000, Vector3::Zero(), Material(Material::REFLECTION_TYPE_LAMBERT, Vector3::Zero(), Vector3(1, 1, 1)));

    m_ibl.reset(new IBL("input_data/Barce_Rooftop_C_Env.hdr", 1000000.0, objectsCenter));

    //AddObject(new Sphere(20, Vector3(65, 42, 230), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.25, 0.75, 0.25))));    // —Î‹…
    //AddObject(new Sphere(16.5, Vector3(27, 42, 197), Material(Material::REFLECTION_TYPE_SPECULAR, Color(), Color(0.99, 0.99, 0.99))));   // ‹¾
    //AddObject(new Sphere(16.5, Vector3(127, 42, 172), Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ƒKƒ‰ƒX

    Model *cube = new Model;
    if (!cube->ReadFromObj("input_data/table/Table and Glasses.obj", true)) {
      std::cerr << "failed to load cube.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    //cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    cube->Transform(Vector3(85, -10, 172), Vector3(160, 160, 160), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI));
    AddModel(cube);

    cube = new Model;
    if (!cube->ReadFromObj("input_data/Revolver/Revolver.obj", true)) {
      std::cerr << "failed to load Revolver.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    //cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    cube->Transform(Vector3(85, 30, 142), Vector3(140, 140, 140), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI) * Matrix::RotateAroundVector(Vector3(0, 0, 1), -90.0 / 180 * PI));
    AddModel(cube);

    cube = new Model;
    if (!cube->ReadFromObj("input_data/bullet/bullet.obj", true)) {
      std::cerr << "failed to load bullet.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    //cube->SetTransform(Vector3(90, 30, 30), Vector3(15, 15, 15), Matrix::RotateAroundVector(Vector3(0, 1, 0), 45.0 / 180 * PI));
    cube->Transform(Vector3(65, 30, 172), Vector3(1, 1, 1), Matrix::RotateAroundVector(Vector3(0, 1, 0), 15.0 / 180 * PI));
    AddModel(cube);

    // floor
    //AddFloorXZ_yUp(100.0, 100.0, objectsCenter,
    //  Material(Material::REFLECTION_TYPE_LAMBERT, Vector3::Zero(), Vector3(0.7, 0.7, 0.7)));

    //AddObject(new AxisAlignedPlane(Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.7, 0.7, 0.7)), AxisAlignedPlane::PLANE_XZ), true, false);


    ConstructQBVH();
  }
}
