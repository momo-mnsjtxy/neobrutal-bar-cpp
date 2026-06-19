#include "Config.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

namespace neobar {

QString Config::defaultPath() {
  QString dir =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  if (dir.isEmpty()) dir = QDir::homePath() + "/.config";
  dir += "/neobrutal-bar";
  QDir().mkpath(dir);
  return dir + "/config.json";
}

static void applySizes(Theme &t, const QJsonObject &s) {
  if (s.contains("height")) t.height = s.value("height").toInt(t.height);
  if (s.contains("fontSize")) t.fontSize = s.value("fontSize").toInt(t.fontSize);
  if (s.contains("fontWeight"))
    t.fontWeight = s.value("fontWeight").toInt(t.fontWeight);
  if (s.contains("borderSize"))
    t.borderSize = s.value("borderSize").toInt(t.borderSize);
  if (s.contains("radius")) t.radius = s.value("radius").toInt(t.radius);
  if (s.contains("shadowSizeBar"))
    t.shadowSizeBar = s.value("shadowSizeBar").toInt(t.shadowSizeBar);
  if (s.contains("shadowSizeButton"))
    t.shadowSizeButton = s.value("shadowSizeButton").toInt(t.shadowSizeButton);
  if (s.contains("barMarginX"))
    t.barMarginX = s.value("barMarginX").toInt(t.barMarginX);
  if (s.contains("barMarginY"))
    t.barMarginY = s.value("barMarginY").toInt(t.barMarginY);
  if (s.contains("fontFamily"))
    t.fontFamily = s.value("fontFamily").toString(t.fontFamily);
}

Config Config::load(const QString &path) {
  Config cfg;
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly)) return cfg;

  const QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();

  if (root.contains("theme")) cfg.themeName = root.value("theme").toString();
  cfg.theme = Theme::builtin(cfg.themeName);

  if (root.contains("screen")) cfg.screen = root.value("screen").toInt(0);

  if (root.contains("sizes") && root.value("sizes").isObject())
    applySizes(cfg.theme, root.value("sizes").toObject());

  if (root.contains("weather") && root.value("weather").isObject()) {
    const QJsonObject w = root.value("weather").toObject();
    if (w.contains("latitude") && w.contains("longitude")) {
      cfg.hasLocation = true;
      cfg.latitude = w.value("latitude").toDouble();
      cfg.longitude = w.value("longitude").toDouble();
    }
  }

  return cfg;
}

}  // namespace neobar
