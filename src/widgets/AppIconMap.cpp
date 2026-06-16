#include "widgets/AppIconMap.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace neobar {

const AppIconMap &AppIconMap::instance() {
  static AppIconMap inst;
  return inst;
}

AppIconMap::AppIconMap() {
  QFile mapFile(":/data/icon_map.json");
  if (mapFile.open(QIODevice::ReadOnly)) {
    const QJsonArray arr = QJsonDocument::fromJson(mapFile.readAll()).array();
    for (const QJsonValue &v : arr) {
      const QJsonObject o = v.toObject();
      const QString icon = o.value("iconName").toString();
      for (const QJsonValue &n : o.value("appNames").toArray())
        byAppName_.insert(n.toString().toLower(), icon);
    }
  }

  QFile ignoreFile(":/data/ignored_apps.json");
  if (ignoreFile.open(QIODevice::ReadOnly)) {
    const QJsonArray arr =
        QJsonDocument::fromJson(ignoreFile.readAll()).array();
    for (const QJsonValue &v : arr)
      ignored_.insert(v.toObject().value("name").toString().toLower());
  }
}

QString AppIconMap::iconFor(const QString &processName,
                            const QString &title) const {
  const QString p = processName.toLower();
  const QString t = title.toLower();

  if (ignored_.contains(p) || ignored_.contains(t)) return {};

  if (byAppName_.contains(p)) return byAppName_.value(p);
  if (byAppName_.contains(t)) return byAppName_.value(t);

  return "ti-background";  // upstream default
}

}  // namespace neobar
