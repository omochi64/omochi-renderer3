#include "stdafx.h"

#include "renderer/PathTracer.h"
#include "renderer/Camera.h"
#include "renderer/LinearGammaToonMapper.h"
#include "scenes/CornellBoxScene.h"
#include "scenes/TestScene.h"
#include "scenes/IBLTestScene.h"
#include "scenes/SceneFromExternalFile.h"
#include "tools/Vector.h"
#include "tools/PPM.h"
#include "viewer/WindowViewer.h"
#include "renderer/Settings.h"
#include "tools/PPMSaver.h"

#include <omp.h>

using namespace std;
using namespace OmochiRenderer;

int main(int argc, char *argv[]) {

  std::shared_ptr<Settings> settings = std::make_shared<Settings>();

  // set renderer and scene
  std::string settingfile = "settings.txt";
  if (argc >= 2) { settingfile = argv[1]; }
  if (!settings->LoadFromFile(settingfile)) {
    std::cerr << "Failed to load " << settingfile << std::endl;
    return -1;
  }

  // ファイル保存用インスタンス
  PPMSaver saver(settings);

  PathTracer::RenderingFinishCallbackFunction callback([&saver](int samples, const Color *img, double accumulatedRenderingTime) {
      // レンダリング完了時に呼ばれるコールバックメソッド
      cerr << "save ppm file for sample " << samples << " ..." << endl;
      saver.Save(samples, img, accumulatedRenderingTime);
      cerr << "Total rendering time = " << accumulatedRenderingTime << " min." << endl;
  });

  Camera camera(settings->GetWidth(), settings->GetHeight(), settings->GetCameraPosition(), settings->GetCameraDirection(),
    settings->GetCameraUp(), settings->GetScreenHeightInWorldCoordinate(), settings->GetDistanceFromCameraToScreen());

  PathTracer renderer(camera, settings->GetSampleStart(), settings->GetSampleEnd(), settings->GetSampleStep(), settings->GetSuperSamples(), callback);
  renderer.EnableNextEventEstimation(Utils::parseBoolean(settings->GetRawSetting("next event estimation")));

  //TestScene scene;
  //IBLTestScene scene;
  SceneFromExternalFile scene(settings->GetSceneFile());
  if (!scene.IsValid()) {
    cerr << "faild to load scene: " << settings->GetSceneFile() << endl;
    return -1;
  }
  //CornellBoxScene scene;

  omp_set_num_threads(settings->GetNumberOfThreads());

  clock_t startTime;

  // set window viewer
  LinearGammaToonMapper mapper;
  WindowViewer viewer("OmochiRenderer", camera, renderer, mapper);
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
	renderer.RenderScene(scene);
  cerr << "total time = " << (1.0 / 60 * (clock() - startTime) / CLOCKS_PER_SEC) << " (min)." << endl;

  if (settings->DoShowPreview()) {
    viewer.WaitWindowFinish();
  }
	return 0;
}
