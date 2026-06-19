#pragma once

#include <QString>
#include <QVector>

namespace neobar {

enum class NetType { None, Ethernet, Wifi };

struct NetworkInfo {
  NetType type = NetType::None;
  int signalStrength = 0;  // 0..100, only meaningful for wifi
  QString ssid;
};

enum class WeatherStatus {
  Unknown,
  ClearDay,
  ClearNight,
  CloudyDay,
  CloudyNight,
  LightRainDay,
  LightRainNight,
  HeavyRainDay,
  HeavyRainNight,
  SnowDay,
  SnowNight,
  ThunderDay,
  ThunderNight,
};

struct WeatherInfo {
  bool valid = false;
  double celsius = 0.0;
  WeatherStatus status = WeatherStatus::Unknown;
};

struct WindowInfo {
  QString processName;
  QString title;
  bool hasFocus = false;
  bool minimized = false;
};

struct WorkspaceInfo {
  int index = 0;  // 0-based position among current workspaces
  QString name;
  bool hasFocus = false;
};

struct WorkspaceData {
  QVector<WorkspaceInfo> workspaces;
  QVector<WindowInfo> focusedWindows;
};

struct MediaInfo {
  bool hasPlayer = false;
  bool playing = false;
  QString title;
  QString artist;
};

}  // namespace neobar
