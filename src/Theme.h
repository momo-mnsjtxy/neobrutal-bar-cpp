#pragma once

#include <QColor>
#include <QString>
#include <array>

namespace neobar {

// A Theme is the C++ analogue of the original project's `config.css` +
// `themes/*.css`: a palette of role -> colour plus the neobrutalist sizing
// knobs. Colours are ported directly from the upstream HSL definitions.
struct Theme {
  // role colours
  QColor text;
  QColor base;  // bar/group background (--bg)
  QColor border;
  QColor shadow;
  QColor icon;
  QColor memory;
  QColor cpu;
  QColor cpuHighUsage;
  QColor batteryGood;
  QColor batteryMid;
  QColor batteryLow;
  QColor focusedProcess;
  QColor process;
  QColor displayed;
  std::array<QColor, 5> ws;
  QColor tilingDirection;
  QColor nowPlaying;
  QColor notPlaying;
  QColor network;
  QColor weather;

  // sizing (pixels, assuming a 16px rem base — see config.css)
  int height = 40;          // --height: 2.5rem
  int barMarginX = 20;      // --bar-margin-x: 1.25rem
  int barMarginY = 10;      // --bar-margin-y: 0.625rem
  int fontSize = 16;        // --font-size
  int fontWeight = 500;     // --font-weight
  int borderSize = 2;       // --border-size: 0.125rem
  int radius = 4;           // --radius: 0.25rem
  int shadowSizeBar = 4;    // --shadow-size-bar: 0.25rem
  int shadowSizeButton = 2; // --shadow-size-button: 0.125rem
  QString fontFamily = "SF Pro Display";

  // Returns one of the built-in themes by (case-insensitive) name. Falls back
  // to Rosé Pine — the upstream default — for unknown names.
  static Theme builtin(const QString &name);

  // Parses a "H S% L%" CSS HSL triple (the format used in the upstream theme
  // files) into a QColor.
  static QColor hsl(const QString &triple);
};

}  // namespace neobar
