#pragma once

#include <QString>

#include "Theme.h"

namespace neobar {

// Runtime configuration, the C++ analogue of the upstream `config.css`. Loaded
// from a JSON file (default: $XDG_CONFIG_HOME/neobrutal-bar/config.json). Every
// field is optional; missing fields keep their defaults.
struct Config {
  QString themeName = "rose-pine";
  Theme theme = Theme::builtin("rose-pine");

  int screen = 0;  // monitor index to place the bar on

  // Weather location. When unset, the weather provider geolocates via IP.
  bool hasLocation = false;
  double latitude = 0.0;
  double longitude = 0.0;

  // Returns the default config path and ensures its directory exists.
  static QString defaultPath();

  // Loads configuration from `path`. Returns a Config with defaults applied for
  // anything missing or when the file does not exist.
  static Config load(const QString &path);
};

}  // namespace neobar
