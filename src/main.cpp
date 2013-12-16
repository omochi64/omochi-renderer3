#include <iostream>
#include <sstream>
#include <ctime>

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

#include <omp.h>

using namespace std;
using namespace OmochiRenderer;

static Settings settings;

class SavePPM_callback : public PathTracer::RenderingFinishCallback {
  int w,h;
  double accumulatedRenderingTime;
public:
  SavePPM_callback(int width, int height):w(width),h(height),accumulatedRenderingTime(0){};
  void operator()(int samples, const Color *img, double renderingDiffTimeInMinutes) {
    accumulatedRenderingTime += renderingDiffTimeInMinutes;
  	cerr << "save ppm file for sample " << samples << " ..." << endl;
    char name[1024];
    if (settings.GetRawSetting("save filename format").empty()) {
      sprintf_s(name, 1024, "result_ibl_test_w%d_h%d_%04d_%dx%d_%.2fmin.ppm",
        settings.GetWidth(), settings.GetHeight(), samples, settings.GetSuperSamples(), settings.GetSuperSamples(), accumulatedRenderingTime);
    } else {
      sprintf_s(name, 1024, settings.GetRawSetting("save filename format").c_str(),
        settings.GetWidth(), settings.GetHeight(), samples, settings.GetSuperSamples(), settings.GetSuperSamples(), accumulatedRenderingTime);
    }
    clock_t begin,end;
    begin = clock();
    PPM::Save(name, img, w, h);
    end = clock();
    cerr << "saving time = " << (double)(end - begin)/CLOCKS_PER_SEC << endl;
    cerr << "rendering time (diff) = " << renderingDiffTimeInMinutes << " min. Total rendering time = " << accumulatedRenderingTime << " min." << endl;
  };
};

int main(int argc, char *argv[]) {

  // set renderer and scene
  std::string settingfile = "settings.txt";
  if (argc >= 2) { settingfile = argv[1]; }
  if (!settings.LoadFromFile(settingfile)) {
    std::cerr << "Failed to load " << settingfile << std::endl;
    return -1;
  }

  SavePPM_callback callback(settings.GetWidth(), settings.GetHeight());
  Camera camera(settings.GetWidth(), settings.GetHeight(), settings.GetCameraPosition(), settings.GetCameraDirection(),
    settings.GetCameraUp(), settings.GetScreenHeightInWorldCoordinate(), settings.GetDistanceFromCameraToScreen());

  PathTracer renderer(camera, settings.GetSampleStart(), settings.GetSampleEnd(), settings.GetSampleStep(), settings.GetSuperSamples(), &callback);
  renderer.EnableNextEventEstimation(Utils::parseBoolean(settings.GetRawSetting("next event estimation")));

  //TestScene scene;
  //IBLTestScene scene;
  SceneFromExternalFile scene(settings.GetSceneFile());
  if (!scene.IsValid()) {
    cerr << "faild to load scene: " << settings.GetSceneFile() << endl;
    return -1;
  }
  //CornellBoxScene scene;

  omp_set_num_threads(settings.GetNumberOfThreads());

  clock_t startTime;

  // set window viewer
  LinearGammaToonMapper mapper;
  WindowViewer viewer("OmochiRenderer", camera, renderer, mapper);
  if (settings.DoShowPreview()) {
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

  if (settings.DoShowPreview()) {
    viewer.WaitWindowFinish();
  }
	return 0;
}
