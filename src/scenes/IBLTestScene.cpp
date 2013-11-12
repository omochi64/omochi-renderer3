#include "IBLTestScene.h"
#include "renderer/AxisAlignedPlane.h"
#include "renderer/Model.h"
#include "renderer/Sphere.h"
#include "renderer/HDRImage.h"

#include <iostream>

namespace OmochiRenderer {
  IBLTestScene::IBLTestScene() {
    //AddObject(new Sphere(5.0, Vector3(50.0, 90.0, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(36, 36, 36), Color())), true, false);    // Æ–¾

    //HDRImage hdr;
    //hdr.ReadFromRadianceFile("input_data/grace_probe.hdr");

    Model *cube = new Model;
    if (!cube->ReadFromObj("input_data/torii.obj")) {
      std::cerr << "failed to load cube.obj!!!" << std::endl;
      getchar();
      exit(-1);
    }
    cube->SetTransform(Vector3(50, 0, 0), Vector3(1,1,1));
    AddModel(cube);

    //AddObject(new AxisAlignedPlane(Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.7, 0.7, 0.7)), AxisAlignedPlane::PLANE_XZ), true, false);


    ConstructQBVH();
  }
}
