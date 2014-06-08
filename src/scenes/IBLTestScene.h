#pragma once

#include "Scene.h"
#include "SceneFactory.h"

namespace OmochiRenderer {
  class IBLTestScene : public Scene {
  public:
    IBLTestScene();
  };

  class IBLTestSceneFactory : public SceneFactory {
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const {
      return std::make_shared<IBLTestScene>();
    }
  };
}
