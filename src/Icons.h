#pragma once

#include <QFont>
#include <QHash>
#include <QString>

namespace neobar {

// Loads the bundled Tabler icon webfonts and resolves icon names (as used in
// the upstream Svelte components and `icon_map.json`) to glyph characters. The
// outline and filled variants ship as separate fonts; names ending in
// "-filled" resolve against the filled font.
class Icons {
 public:
  static Icons &instance();

  // Resolves an icon name (with or without a leading "ti-") to a single-glyph
  // string. Returns the "ti-background" glyph when unknown.
  QString glyph(const QString &name) const;

  // Returns a QFont for the given icon name at `pixelSize`, selecting the
  // outline or filled family as appropriate.
  QFont font(const QString &name, int pixelSize) const;

 private:
  Icons();
  void loadFonts();
  void loadMap();

  QString outlineFamily_;
  QString filledFamily_;
  QHash<QString, uint> outline_;
  QHash<QString, uint> filled_;
};

}  // namespace neobar
