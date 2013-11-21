#include <iostream>
#include <sstream>
#include <ctime>

#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "renderer/LinearGammaToonMapper.h"
#include "scenes/CornellBoxScene.h"
#include "scenes/TestScene.h"
#include "scenes/IBLTestScene.h"
#include "scenes/SceneFromExternalFile.h"
#include "tools/Vector.h"
#include "tools/PPM.h"
#include "viewer/WindowViewer.h"

#include <omp.h>

using namespace std;
using namespace OmochiRenderer;

static const int supersampling = 4;

static const int startSample = 2;
static const int endSample = 32;
static const int stepSample = 2;

static const int width = 1280;
static const int height = 720;

static const int number_of_threads = omp_get_num_procs() - 1; // default setting

class SavePPM_callback : public PathTracer::RenderingFinishCallback {
  int w,h;
public:
  SavePPM_callback(int width, int height):w(width),h(height){};
  void operator()(int samples, const Color *img) {
  	cerr << "save ppm file for sample " << samples << " ..." << endl;
    char name[1024];
    sprintf_s(name, 1024, "result_ibl_test_w%d_h%d_%04d_%dx%d.ppm", 
      width, height,
      samples, supersampling, supersampling);
    clock_t begin,end;
    begin = clock();
    PPM::Save(name, img, w, h);
    end = clock();
    cerr << "saving time = " << (double)(end - begin)/CLOCKS_PER_SEC << endl;
  };
};

int main(int argc, char *argv[]) {

  // set renderer and scene
  SavePPM_callback callback(width, height);
  Camera camera(width, height);
  PathTracer renderer(camera, startSample, endSample, stepSample, supersampling, &callback);
	//TestScene scene;
  IBLTestScene scene;
  //SceneFromExternalFile scene("input_data/cornell_box.scene");
  //if (!scene.IsValid()) {
  //  cerr << "faild to load scene" << endl;
  //  return -1;
  //}
  //CornellBoxScene scene;

  omp_set_num_threads(number_of_threads);

  clock_t startTime;

  // set window viewer
  LinearGammaToonMapper mapper;
  WindowViewer viewer("OmochiRenderer", camera, renderer, mapper);
  viewer.StartViewerOnNewThread();
  viewer.SetCallbackFunctionWhenWindowClosed(std::function<void(void)>(
    [&startTime]{
      cerr << "total time = " << (1.0 / 60 * (clock() - startTime) / CLOCKS_PER_SEC) << " (min)." << endl;
      exit(0);
    }
  ));

  // start
  cerr << "begin rendering..." << endl;
  startTime = clock();
	renderer.RenderScene(scene);
  cerr << "total time = " << (1.0 / 60 * (clock() - startTime) / CLOCKS_PER_SEC) << " (min)." << endl;

	return 0;
}
