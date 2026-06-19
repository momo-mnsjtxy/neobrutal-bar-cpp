#include <QApplication>
#include <QCommandLineParser>

#include "Config.h"
#include "Theme.h"
#include "widgets/Bar.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  app.setApplicationName("neobrutal-bar");
  app.setApplicationDisplayName("neobrutal-bar");

  QCommandLineParser parser;
  parser.setApplicationDescription(
      "A neobrutalist desktop status bar (C++/Qt port of neobrutal-zebar).");
  parser.addHelpOption();
  const QCommandLineOption configOpt(
      "config", "Path to the JSON config file.", "path");
  const QCommandLineOption themeOpt(
      "theme", "Theme name: rose-pine, catppuccin, nord, material.", "name");
  const QCommandLineOption screenOpt("screen", "Monitor index.", "index");
  parser.addOption(configOpt);
  parser.addOption(themeOpt);
  parser.addOption(screenOpt);
  parser.process(app);

  const QString path = parser.isSet(configOpt)
                           ? parser.value(configOpt)
                           : neobar::Config::defaultPath();
  neobar::Config cfg = neobar::Config::load(path);

  if (parser.isSet(themeOpt)) {
    cfg.themeName = parser.value(themeOpt);
    cfg.theme = neobar::Theme::builtin(cfg.themeName);
  }
  if (parser.isSet(screenOpt)) cfg.screen = parser.value(screenOpt).toInt();

  neobar::Bar bar(cfg);
  bar.show();
  return app.exec();
}
