#pragma once

#include "Scene.h"
#include "SceneFactory.h"

namespace OmochiRenderer {

  class CornellBoxScene : public Scene {
  public:
	  CornellBoxScene();
  };

  class CornellBoxSceneFactory : public SceneFactory {
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const {
      return std::make_shared<CornellBoxScene>();
    }
  };

}
