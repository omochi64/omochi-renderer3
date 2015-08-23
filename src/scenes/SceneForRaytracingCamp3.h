#pragma once

#include "Scene.h"
#include "SceneFactory.h"

namespace OmochiRenderer {
  class SceneForRaytracingCamp3 : public Scene {
  public:
    SceneForRaytracingCamp3();
  };

  class SceneForRaytracingCamp3Factory : public SceneFactory {
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const {
      return std::make_shared<SceneForRaytracingCamp3>();
    }
  };
}
