#include "IBLTestScene.h"
#include "renderer/AxisAlignedPlane.h"
#include "renderer/Model.h"
#include "renderer/Sphere.h"
#include "renderer/HDRImage.h"
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

    const Vector3 objectsCenter(50, 0, 0);

    m_ibl.reset(new IBL("input_data/Alexs_Apt_2k.hdr", 1000.0, objectsCenter));

    AddObject(new Sphere(20, Vector3(65, 20, -30), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.25, 0.75, 0.25))));    // —Î‹…
    AddObject(new Sphere(16.5, Vector3(27, 16.5, -3), Material(Material::REFLECTION_TYPE_SPECULAR, Color(), Color(0.99, 0.99, 0.99))));   // ‹¾
    AddObject(new Sphere(16.5, Vector3(77, 16.5, 28), Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ƒKƒ‰ƒX

    // floor
    AddFloor(100.0, 100.0, objectsCenter,
      Material(Material::REFLECTION_TYPE_LAMBERT, Vector3::Zero(), Vector3(0.7, 0.7, 0.7)));

    //AddObject(new AxisAlignedPlane(Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.7, 0.7, 0.7)), AxisAlignedPlane::PLANE_XZ), true, false);


    //ConstructQBVH();
  }
}
