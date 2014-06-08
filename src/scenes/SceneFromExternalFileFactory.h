#pragma once

#include "SceneFactory.h"
#include "SceneFromExternalFile.h"

namespace OmochiRenderer {

  class SceneFromExternalFileFactory : public SceneFactory {
  public:
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const {
      std::shared_ptr<SceneFromExternalFile> scene = std::make_shared<SceneFromExternalFile>(sceneInformation);
      return scene->IsValid() ? scene : nullptr;
    }
  };
}