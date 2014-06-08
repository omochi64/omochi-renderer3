#include "stdafx.h"

#include "TestScene.h"
#include "renderer/Sphere.h"
#include "renderer/Polygon.h"
#include "renderer/Model.h"
#include "renderer/BVH.h"
#include "renderer/SphereLight.h"

namespace OmochiRenderer {

TestScene::TestScene()
{
  AddObject(new Sphere(1e5, Vector3(1e5 + 1, 40.8, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.25, 0.25))), true, false);  // 左
  AddObject(new Sphere(1e5, Vector3(-1e5 + 99, 40.8, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.25, 0.25, 0.75))), true, false);  // 右
  AddObject(new Sphere(1e5, Vector3(50, 40.8, 1e5), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75))), true, false);  // 奥
  AddObject(new Sphere(1e5, Vector3(50, 40.8, -1e5 + 250), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color())), true, false);                  // 手前
  AddObject(new Sphere(1e5, Vector3(50, 1e5, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75))), true, false);  // 下
  AddObject(new Sphere(1e5, Vector3(50, -1e5 + 81.6, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75))), true, false);  // 上

  //addObject(new Sphere(1e5, Vector3( 1e5+1, 40.8, 81.6),  Color(), Color(0.75, 0.75, 0.75), SceneObject::REFLECTION_TYPE_SPECULAR));  // 左
  //addObject(new Sphere(1e5, Vector3(-1e5+99, 40.8, 81.6), Color(), Color(0.75, 0.75, 0.75), SceneObject::REFLECTION_TYPE_SPECULAR));  // 右
  //addObject(new Sphere(1e5, Vector3(50, 40.8,  1e5),      Color(), Color(0.75, 0.75, 0.75), SceneObject::REFLECTION_TYPE_SPECULAR));  // 奥
  //addObject(new Sphere(1e5, Vector3(50, 40.8, -1e250),    Color(), Color(0.75, 0.75, 0.75), SceneObject::REFLECTION_TYPE_SPECULAR));  // 手前
  
  Model *cube = new Model;
  if (!cube->ReadFromObj("input_data/shrine.obj")) {
    std::cerr << "failed to load cube.obj!!!" << std::endl;
    getchar();
    exit(-1);
  }
  cube->SetTransform(Vector3(50, 10, 50), Vector3(0.08, 0.08, 0.08), Matrix::RotateAroundVector(Vector3(0, 1, 0), 135.0 / 180 * PI));
  AddModel(cube);
  
  //addObject(new Sphere(10,Vector3(),           Material(Material::REFLECTION_TYPE_LAMBERT,    Color(), Color(0.25, 0.75, 0.25))));    // 緑球
  //addObject(new Sphere(16.5,Vector3(27, 16.5, 47),       Material(Material::REFLECTION_TYPE_SPECULAR,   Color(), Color(0.99, 0.99, 0.99))));   // 鏡
  //addObject(new Sphere(16.5,Vector3(77, 16.5, 78),       Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ガラス


  SphereLight *sphereLight = new SphereLight(7.5, Vector3(50.0, 72.5, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(26, 26, 26), Color()));
  //SphereLight *sphereLight = new SphereLight(15, Vector3(50.0, 90, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(36, 36, 36), Color()));
  AddObject(sphereLight, true, false);    // 照明

  //ConstructBVH();
  ConstructQBVH();
}

}
