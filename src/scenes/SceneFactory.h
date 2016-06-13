#pragma once

namespace OmochiRenderer {

  class Scene;

  class SceneFactory {
  public:
    virtual std::shared_ptr<Scene> Create(const std::string &sceneInformation) const = 0;
  };

  // シーンFactoryのマネージャ。シーンファクトリ名からfactoryのインスタンスが引ける
  class SceneFactoryManager {
  public:
    static SceneFactoryManager & GetInstance()
    {
      static SceneFactoryManager s;
      return s;
    }

    void Register(const std::string &name, const std::shared_ptr<SceneFactory> &factory)
    {
      m_factories[name] = factory;
    }

    const std::shared_ptr<SceneFactory> Get(const std::string &name) const
    {
      auto it = m_factories.find(name);
      if (it != m_factories.end())
      {
        return it->second;
      }
      return nullptr;
    }

  private:
    std::unordered_map<std::string, std::shared_ptr<SceneFactory> > m_factories;

    SceneFactoryManager()
      : m_factories()
    {
    }
    ~SceneFactoryManager()
    {
    }


  };
}
