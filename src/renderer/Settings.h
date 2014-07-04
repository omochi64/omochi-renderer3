#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "tools/Vector.h"
#include "tools/Utils.h"
#include <omp.h>

namespace OmochiRenderer {
  class Settings {
  public:
    Settings()
      : m_supersamples(2)
      , m_sampleStart(8), m_sampleEnd(8), m_sampleStep(1)
      , m_width(640), m_height(480)
      , m_numThreads(omp_get_num_procs()-1)
      , m_sceneType("SceneFromExternalFile")
      , m_sceneInfo("input_data/default.scene")
      , m_camPos(50.0, 72.0, 220.0)
      , m_camDir(0.0, -0.54, -1.0)
      , m_camUp(0, 1, 0)
      , m_screenHeightInWorldCoordinate(30.0)
      , m_distanceFromCameraToScreen(40.0)
      , m_saveSpan(0)
      , m_doSaveOnEachSampleEnded(false)
      , m_maxSaveCountForPeriodicSave(0)
      , m_timeToStopRenderer(0)
      , m_showPreview(true)
      , m_rawSettings()
    {
    }
    ~Settings() {}

    bool LoadFromFile(const std::string &file) {
      std::ifstream ifs(file.c_str());
      if (!ifs || ifs.bad()) return false;

      if (ifs.eof()) return true;

      std::string line;
      int line_number = 1;

      do {
        std::getline(ifs, line);
        std::vector<std::string> data(Utils::split(Utils::trim(line, " \t\r\n"), '='));
        if (data.size() == 0) continue;
        if (data.size() == 1 && data[0].at(0) == '#') continue;
        if (data.size() != 2) {
          std::cerr << "Failed to parse line " << line_number << ":" << line << std::endl;
          return false;
        }

        std::string keyword(Utils::tolower(Utils::trim(data[0])));
        std::string value(Utils::trim(data[1]));

        if (keyword == "supersamples") {
          m_supersamples = atoi(value.c_str());
        } else if (keyword == "sample start") {
          m_sampleStart = atoi(value.c_str());
        } else if (keyword == "sample end") {
          m_sampleEnd = atoi(value.c_str());
        } else if (keyword == "sample step") {
          m_sampleStep = atoi(value.c_str());
        } else if (keyword == "width") {
          m_width = atoi(value.c_str());
        } else if (keyword == "height") {
          m_height = atoi(value.c_str());
        } else if (keyword == "scene type") {
          m_sceneType = value;
        } else if (keyword == "scene information") {
          m_sceneInfo = value;
        } else if (keyword == "camera position") {
          m_camPos = Utils::splitVector(value);
        } else if (keyword == "camera direction") {
          m_camDir = Utils::splitVector(value);
          m_camDir.normalize();
        } else if (keyword == "camera up") {
          m_camUp = Utils::splitVector(value);
          m_camUp.normalize();
        } else if (keyword == "screen height in world coordinate") {
          m_screenHeightInWorldCoordinate = atof(value.c_str());
        } else if (keyword == "distance from camera to screen") {
          m_distanceFromCameraToScreen = atof(value.c_str());
        } else if (keyword == "number of threads") {
          m_numThreads = atoi(value.c_str());
          if (m_numThreads < 1) m_numThreads = 1;
        } else if (keyword == "show preview") {
          m_showPreview = Utils::parseBoolean(value);
        } else if (keyword == "save span") {
          m_saveSpan = atof(value.c_str());
        } else if (keyword == "save on each sample ended") {
          m_doSaveOnEachSampleEnded = Utils::parseBoolean(value);
        } else if (keyword == "max save count for periodic save") {
          m_maxSaveCountForPeriodicSave = atoi(value.c_str());
        } else if (keyword == "time to stop renderer") {
          m_timeToStopRenderer = atof(value.c_str());
        } else {
          //std::cerr << "Unknown keyword: " << keyword << std::endl;
        }

        m_rawSettings[keyword] = value;

        line_number++;
      } while (!ifs.eof());

      return true;
    }

    int GetSuperSamples() const { return m_supersamples; }
    int GetSampleStart() const { return m_sampleStart; }
    int GetSampleEnd() const { return m_sampleEnd; }
    int GetSampleStep() const { return m_sampleStep; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    const std::string &GetSceneType() const { return m_sceneType; }
    const std::string &GetSceneInformation() const { return m_sceneInfo; }
    const Vector3 &GetCameraPosition() const { return m_camPos; }
    const Vector3 &GetCameraDirection() const { return m_camDir; }
    const Vector3 &GetCameraUp() const { return m_camUp; }
    int GetNumberOfThreads() const { return m_numThreads; }
    bool DoShowPreview() const { return m_showPreview; }

    double GetSaveSpan() const { return m_saveSpan; }
    bool DoSaveOnEachSampleEnded() const { return m_doSaveOnEachSampleEnded; }
    int GetMaxSaveCountForPeriodicSave() const { return m_maxSaveCountForPeriodicSave; }

    double GetTimeToStopRenderer() const { return m_timeToStopRenderer; }

    double GetScreenHeightInWorldCoordinate() const { return m_screenHeightInWorldCoordinate; }
    double GetDistanceFromCameraToScreen() const { return m_distanceFromCameraToScreen; }

    std::string GetRawSetting(const std::string &keyword) const {
      std::string lower(Utils::tolower(keyword));
      if (m_rawSettings.find(lower) != m_rawSettings.end()) {
        return m_rawSettings.find(lower)->second;
      }
      return "";
    }

  private:
    int m_supersamples;
    int m_sampleStart, m_sampleEnd, m_sampleStep;
    int m_width, m_height;
    int m_numThreads;
    std::string m_sceneType, m_sceneInfo;

    Vector3 m_camPos, m_camDir, m_camUp;
    double m_screenHeightInWorldCoordinate;
    double m_distanceFromCameraToScreen;

    double m_saveSpan;
    bool m_doSaveOnEachSampleEnded;
    int m_maxSaveCountForPeriodicSave;

    double m_timeToStopRenderer;

    bool m_showPreview;

    std::map<std::string, std::string> m_rawSettings;
  };
}
