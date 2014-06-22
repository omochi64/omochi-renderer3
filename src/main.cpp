#include "stdafx.h"

#include "renderer/PathTracer.h"
#include "renderer/Camera.h"
#include "renderer/LinearGammaToonMapper.h"
#include "scenes/CornellBoxScene.h"
#include "scenes/TestScene.h"
#include "scenes/IBLTestScene.h"
#include "scenes/SceneFromExternalFile.h"
#include "scenes/SceneFromExternalFileFactory.h"
#include "tools/Vector.h"
#include "viewer/WindowViewer.h"
#include "renderer/Settings.h"
#include "tools/PPMSaver.h"
#include "tools/PNGSaver.h"
#include "tools/RadianceSaver.h"
#include "tools/FileSaverCallerWithTimer.h"
#include "tools/StopRendererWithTimer.h"
#include "renderer/Aperture.h"

#include <omp.h>

using namespace std;
using namespace OmochiRenderer;

void initSceneFactories()
{
  auto &factoryManager = SceneFactoryManager::GetInstance();

  factoryManager.Register("CornellBoxScene", std::make_shared<CornellBoxSceneFactory>());
  factoryManager.Register("IBLTestScene", std::make_shared<IBLTestSceneFactory>());
  factoryManager.Register("SceneFromExternalFile", std::make_shared<SceneFromExternalFileFactory>());
  factoryManager.Register("TestScene", std::make_shared<TestSceneFactory>());
}

int main(int argc, char *argv[]) {

  initSceneFactories();

  std::shared_ptr<Settings> settings = std::make_shared<Settings>();

  // set renderer and scene
  std::string settingfile = "settings.txt";
  if (argc >= 2) { settingfile = argv[1]; }
  if (!settings->LoadFromFile(settingfile)) {
    std::cerr << "Failed to load " << settingfile << std::endl;
    return -1;
  }

  // ファイル保存用インスタンス
  auto saver = std::make_shared<RadianceSaver>(settings);
  auto saver2 = std::make_shared<PNGSaver>(settings);

  PathTracer::RenderingFinishCallbackFunction callback([&saver, &saver2](int samples, const Color *img, double accumulatedRenderingTime) {
      // レンダリング完了時に呼ばれるコールバックメソッド
      cerr << "save ppm file for sample " << samples << " ..." << endl;
      saver->Save(samples, 9999999, img, accumulatedRenderingTime);
      saver2->Save(samples, 9999999, img, accumulatedRenderingTime);
      cerr << "Total rendering time = " << accumulatedRenderingTime << " min." << endl;
  });

  if (!settings->DoSaveOnEachSampleEnded()) {
    callback = nullptr;
  }

  // カメラ設定
  Camera camera(settings->GetWidth(), settings->GetHeight(), settings->GetCameraPosition(), settings->GetCameraDirection(),
    settings->GetCameraUp(), settings->GetScreenHeightInWorldCoordinate(), settings->GetDistanceFromCameraToScreen(), 165);
  camera.SetAperture(std::shared_ptr<Aperture>(new CircleAperture(10)));

  // レンダラ生成
  std::shared_ptr<PathTracer> renderer = std::make_shared<PathTracer>(
    camera, settings->GetSampleStart(), settings->GetSampleEnd(), settings->GetSampleStep(), settings->GetSuperSamples(), callback);
  renderer->EnableNextEventEstimation(Utils::parseBoolean(settings->GetRawSetting("next event estimation")));

  // 時間監視してファイルを保存するインスタンス
  FileSaverCallerWithTimer timeSaver(renderer, saver);
  timeSaver.AddSaver(saver2);
  timeSaver.SetSaveTimerInformation(settings->GetSaveSpan());
  timeSaver.SetMaxSaveCount(settings->GetMaxSaveCountForPeriodicSave());
  timeSaver.SetAimTimeToSaveFile(1.5);
  timeSaver.StartTimer();

  // 時間監視してレンダラをストップするインスタンス
  StopRendererWithTimer stopTimer(renderer);
  if (settings->GetTimeToStopRenderer() > 0) {
    stopTimer.SetTimer(settings->GetTimeToStopRenderer());
    stopTimer.StartTimer();
  }

  // シーン生成
  auto sceneFactory = SceneFactoryManager::GetInstance().Get(settings->GetSceneType());
  if (sceneFactory == nullptr) {
    cerr << "Scene type: " << settings->GetSceneType() << " is invalid!!!" << endl;
    return -1;
  }
  std::shared_ptr<Scene> scene = sceneFactory->Create(settings->GetSceneInformation());

  omp_set_num_threads(settings->GetNumberOfThreads());

  clock_t startTime;

  // set window viewer
  LinearGammaToonMapper mapper;
  WindowViewer viewer("OmochiRenderer", camera, *renderer, mapper);
  if (settings->DoShowPreview()) {
    viewer.StartViewerOnNewThread();
    viewer.SetCallbackFunctionWhenWindowClosed(std::function<void(void)>(
      [&startTime]{
        cerr << "total time = " << (1.0 / 60 * (clock() - startTime) / CLOCKS_PER_SEC) << " (min)." << endl;
        exit(0);
      }
    ));
  }

  // start
  cerr << "begin rendering..." << endl;
  startTime = clock();
	renderer->RenderScene(*scene);
  cerr << "total time = " << (1.0 / 60 * (clock() - startTime) / CLOCKS_PER_SEC) << " (min)." << endl;

  // wait renderer, window, saver
  if (settings->DoShowPreview()) {
    viewer.WaitWindowFinish();
  }
  timeSaver.StopAndWaitStopping();

  renderer.reset();

  return 0;
}
