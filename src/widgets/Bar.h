#pragma once

#include <QWidget>

#include "Config.h"
#include "Theme.h"

class QLabel;

namespace neobar {

class CpuProvider;
class MemoryProvider;
class BatteryProvider;
class NetworkProvider;
class DateProvider;
class WeatherProvider;
class WorkspaceProvider;
class MediaProvider;

class Meter;
class Workspaces;
class NowPlaying;

// The top-level status bar window: a frameless, always-on-top strip across the
// top of the screen holding the left (system), centre (workspaces) and right
// (now-playing / network / weather / clock) groups. Mirrors +page.svelte.
class Bar : public QWidget {
  Q_OBJECT
 public:
  explicit Bar(const Config &config, QWidget *parent = nullptr);

 private:
  QWidget *buildLeftGroup();
  QWidget *buildCenterGroup();
  QWidget *buildRightGroup();
  void positionOnScreen();

  void updateBattery();
  void updateCpu();
  void updateNetwork();
  void updateWeather();
  void updateClock();
  void updateMedia();
  void updateWorkspaces();

  Config config_;
  Theme theme_;

  // providers
  CpuProvider *cpu_ = nullptr;
  MemoryProvider *mem_ = nullptr;
  BatteryProvider *bat_ = nullptr;
  NetworkProvider *net_ = nullptr;
  DateProvider *date_ = nullptr;
  WeatherProvider *weather_ = nullptr;
  WorkspaceProvider *ws_ = nullptr;
  MediaProvider *media_ = nullptr;

  // dynamic widgets
  Meter *memMeter_ = nullptr;
  Meter *cpuMeter_ = nullptr;
  Meter *batMeter_ = nullptr;
  QWidget *batBlock_ = nullptr;
  Workspaces *workspaces_ = nullptr;
  NowPlaying *nowPlaying_ = nullptr;
  QLabel *netIcon_ = nullptr;
  QLabel *netText_ = nullptr;
  QWidget *weatherBlock_ = nullptr;
  QLabel *weatherIcon_ = nullptr;
  QLabel *weatherText_ = nullptr;
  QLabel *clock_ = nullptr;
};

}  // namespace neobar
