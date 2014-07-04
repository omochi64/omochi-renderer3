#include "stdafx.h"

#include "CornellBoxScene.h"
#include "renderer/Sphere.h"
#include "renderer/Polygon.h"
#include "renderer/SphereLight.h"

namespace OmochiRenderer {

CornellBoxScene::CornellBoxScene()
{
  AddFloorYZ_xUp(200, 200, Vector3(1, 40.8, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.25, 0.25), 0.0)); // 左
  AddFloorYZ_xDown(200, 200, Vector3(99, 40.8, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.25, 0.25, 0.75), 0.0)); // 右
  AddFloorXY_zUp(150, 150 / 1.77777777777, Vector3(50, 40.8, 0), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(1, 1, 1), 0.0));  // 奥
  AddFloorXY_zDown(150, 150 / 1.77777777777, Vector3(50, 40.8, 250), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75), 0.0));  // 手前
  AddFloorXZ_yUp(2000, 2000, Vector3(50, 0, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75)));
  AddFloorXZ_yDown(200, 200, Vector3(50, 81.6, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(), Color(0.75, 0.75, 0.75)));

  AddObject(new Sphere(20,Vector3(50, 20, 50),           Material(Material::REFLECTION_TYPE_LAMBERT,    Color(), Color(0.25, 0.75, 0.25))));    // 緑球
  AddObject(new Sphere(16.5,Vector3(19, 16.5, 25),       Material(Material::REFLECTION_TYPE_SPECULAR,   Color(), Color(0.99, 0.99, 0.99))));   // 鏡
  AddObject(new Sphere(16.5, Vector3(77, 16.5, 78), Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ガラス
  //SphereLight *sphereLight = new SphereLight(150, Vector3(50.0, 1900, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(26, 26, 26), Color()));
  SphereLight *sphereLight = new SphereLight(15, Vector3(50.0, 90, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(36, 36, 36), Color()));
  AddObject(sphereLight);    // 照明

  //AddObject(new Sphere(16.5, Vector3(27, 16.5, 47), Material(Material::REFLECTION_TYPE_SPECULAR, Color(), Color(0.99, 0.99, 0.99))));   // 鏡
  //AddObject(new Sphere(16.5, Vector3(73, 16.5, 78), Material(Material::REFLECTION_TYPE_REFRACTION, Color(), Color(0.99, 0.99, 0.99), REFRACTIVE_INDEX_OBJECT))); // ガラス
  //SphereLight *sphereLight = new SphereLight(5, Vector3(50.0, 75, 81.6), Material(Material::REFLECTION_TYPE_LAMBERT, Color(12, 12, 12), Color()));
  //AddObject(sphereLight);    // 照明

  //ConstructQBVH();
}

}
