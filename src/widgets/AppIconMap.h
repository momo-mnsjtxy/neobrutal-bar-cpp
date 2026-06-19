#pragma once

#include <QHash>
#include <QSet>
#include <QString>

namespace neobar {

// Maps application names to Tabler icon names, ported from the upstream
// `icon_map.json` / `ignored_apps.json`. Used to pick the process icon for a
// window in the active workspace.
class AppIconMap {
 public:
  static const AppIconMap &instance();

  // Returns the icon name for a window, or an empty string when the app is on
  // the ignore list. Falls back to "ti-background" (as upstream does) when no
  // mapping matches. `processName` and `title` are matched case-insensitively.
  QString iconFor(const QString &processName, const QString &title) const;

 private:
  AppIconMap();

  QHash<QString, QString> byAppName_;  // lower-cased app name -> icon name
  QSet<QString> ignored_;              // lower-cased ignored app names
};

}  // namespace neobar
