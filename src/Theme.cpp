#include "Theme.h"

#include <QStringList>

namespace neobar {

QColor Theme::hsl(const QString &triple) {
  // Format: "H S% L%" e.g. "248 25% 18%"
  const QStringList parts = triple.simplified().split(' ', Qt::SkipEmptyParts);
  if (parts.size() < 3) return QColor(Qt::magenta);
  const double h = parts[0].toDouble();
  const double s = QString(parts[1]).remove('%').toDouble() / 100.0;
  const double l = QString(parts[2]).remove('%').toDouble() / 100.0;
  QColor c;
  c.setHslF(qBound(0.0, h / 360.0, 1.0), qBound(0.0, s, 1.0),
            qBound(0.0, l, 1.0));
  return c;
}

namespace {

// --- Rosé Pine (default, mirrors static/config.css) ---
Theme rosePine() {
  Theme t;
  const QColor rp_text = Theme::hsl("245 50% 91%");
  const QColor rp_muted = Theme::hsl("249 12% 47%");
  const QColor rp_overlay = Theme::hsl("248 25% 18%");
  const QColor rp_highlight_low = Theme::hsl("244 18% 15%");
  const QColor rp_love = Theme::hsl("343 76% 68%");
  const QColor rp_gold = Theme::hsl("35 88% 72%");
  const QColor rp_rose = Theme::hsl("2 55% 83%");
  const QColor rp_pine = Theme::hsl("197 49% 38%");
  const QColor rp_foam = Theme::hsl("189 43% 73%");
  const QColor rp_iris = Theme::hsl("267 57% 78%");

  t.text = rp_text;
  t.base = rp_overlay;
  t.border = rp_highlight_low;
  t.shadow = rp_highlight_low;
  t.icon = rp_love;
  t.memory = rp_iris;
  t.cpu = rp_rose;
  t.cpuHighUsage = rp_love;
  t.batteryGood = rp_pine;
  t.batteryMid = rp_gold;
  t.batteryLow = rp_love;
  t.focusedProcess = rp_text;
  t.process = rp_muted;
  t.displayed = rp_text;
  t.ws = {rp_gold, rp_love, rp_pine, rp_foam, rp_iris};
  t.tilingDirection = rp_rose;
  t.nowPlaying = rp_pine;
  t.notPlaying = rp_love;
  t.network = rp_text;
  t.weather = rp_text;
  return t;
}

// --- Catppuccin Mocha (mirrors the README example) ---
Theme catppuccin() {
  Theme t;
  const QColor text = Theme::hsl("226.154 63.934% 88.039%");
  const QColor surface0 = Theme::hsl("236.842 16.239% 22.941%");
  const QColor surface2 = Theme::hsl("232.5 12% 39.216%");
  const QColor crust = Theme::hsl("240 22.727% 8.627%");
  const QColor mantle = Theme::hsl("240 21.311% 11.961%");
  const QColor red = Theme::hsl("343.269 81.25% 74.902%");
  const QColor mauve = Theme::hsl("267.407 83.505% 80.98%");
  const QColor pink = Theme::hsl("316.471 71.831% 86.078%");
  const QColor green = Theme::hsl("115.455 54.098% 76.078%");
  const QColor peach = Theme::hsl("22.957 92% 75.49%");
  const QColor blue = Theme::hsl("217.168 91.87% 75.882%");
  const QColor lavender = Theme::hsl("231.892 97.368% 85.098%");

  t.text = text;
  t.base = surface0;
  t.border = crust;
  t.shadow = mantle;
  t.icon = red;
  t.memory = mauve;
  t.cpu = pink;
  t.cpuHighUsage = red;
  t.batteryGood = green;
  t.batteryMid = peach;
  t.batteryLow = red;
  t.focusedProcess = text;
  t.process = surface2;
  t.displayed = text;
  t.ws = {peach, red, green, blue, mauve};
  t.tilingDirection = lavender;
  t.nowPlaying = green;
  t.notPlaying = red;
  t.network = text;
  t.weather = text;
  return t;
}

// --- Nord (role mapping crafted for the neobrutalist bar) ---
Theme nord() {
  Theme t;
  const QColor n0 = Theme::hsl("210 20% 20%");
  const QColor n1 = Theme::hsl("210 12% 26%");
  const QColor n3 = Theme::hsl("210 13% 40%");
  const QColor n4 = Theme::hsl("210 36% 91%");
  const QColor n6 = Theme::hsl("210 21% 96%");
  const QColor n8 = Theme::hsl("192 40% 66%");
  const QColor n9 = Theme::hsl("210 28% 58%");
  const QColor n10 = Theme::hsl("210 35% 53%");
  const QColor n11 = Theme::hsl("358 60% 53%");
  const QColor n13 = Theme::hsl("45 81% 70%");
  const QColor n14 = Theme::hsl("104 30% 65%");
  const QColor n15 = Theme::hsl("322 28% 64%");

  t.text = n4;
  t.base = n1;
  t.border = n0;
  t.shadow = n0;
  t.icon = n11;
  t.memory = n15;
  t.cpu = n9;
  t.cpuHighUsage = n11;
  t.batteryGood = n14;
  t.batteryMid = n13;
  t.batteryLow = n11;
  t.focusedProcess = n6;
  t.process = n3;
  t.displayed = n4;
  t.ws = {n13, n11, n14, n8, n15};
  t.tilingDirection = n10;
  t.nowPlaying = n14;
  t.notPlaying = n11;
  t.network = n4;
  t.weather = n4;
  return t;
}

// --- Material (role mapping crafted for the neobrutalist bar) ---
Theme material() {
  Theme t;
  const QColor grey50 = Theme::hsl("0 0% 98%");
  const QColor blueGrey300 = Theme::hsl("210 8% 57%");
  const QColor blueGrey800 = Theme::hsl("210 8% 14%");
  const QColor blueGrey900 = Theme::hsl("210 16% 8%");
  const QColor red400 = Theme::hsl("0 78% 59%");
  const QColor pink300 = Theme::hsl("340 85% 65%");
  const QColor purple300 = Theme::hsl("300 39% 66%");
  const QColor deepPurple300 = Theme::hsl("240 34% 66%");
  const QColor blue400 = Theme::hsl("210 100% 61%");
  const QColor green400 = Theme::hsl("120 100% 61%");
  const QColor amber400 = Theme::hsl("54 100% 61%");

  t.text = grey50;
  t.base = blueGrey800;
  t.border = blueGrey900;
  t.shadow = blueGrey900;
  t.icon = red400;
  t.memory = purple300;
  t.cpu = pink300;
  t.cpuHighUsage = red400;
  t.batteryGood = green400;
  t.batteryMid = amber400;
  t.batteryLow = red400;
  t.focusedProcess = grey50;
  t.process = blueGrey300;
  t.displayed = grey50;
  t.ws = {amber400, red400, green400, blue400, purple300};
  t.tilingDirection = deepPurple300;
  t.nowPlaying = green400;
  t.notPlaying = red400;
  t.network = grey50;
  t.weather = grey50;
  return t;
}

}  // namespace

Theme Theme::builtin(const QString &name) {
  // Each palette is built once and reused (function-local statics are
  // initialised on first use, thread-safely in C++11+).
  const QString n = name.trimmed().toLower();
  if (n == "catppuccin") {
    static const Theme t = catppuccin();
    return t;
  }
  if (n == "nord") {
    static const Theme t = nord();
    return t;
  }
  if (n == "material") {
    static const Theme t = material();
    return t;
  }
  static const Theme t = rosePine();
  return t;
}

}  // namespace neobar
