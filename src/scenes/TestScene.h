#pragma once

#include "Scene.h"
#include "SceneFactory.h"

namespace OmochiRenderer {

  class TestScene : public Scene {
  public:
	  TestScene();
  };

  class TestSceneFactory : public SceneFactory {
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const {
      return std::make_shared<TestScene>();
    }
  };

}

