#include "Icons.h"

#include <QFile>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>

namespace neobar {

Icons &Icons::instance() {
  static Icons inst;
  return inst;
}

Icons::Icons() {
  loadFonts();
  loadMap();
}

static QString addFont(const QString &resource) {
  QFile f(resource);
  if (!f.open(QIODevice::ReadOnly)) return {};
  const int id = QFontDatabase::addApplicationFontFromData(f.readAll());
  if (id < 0) return {};
  const QStringList fams = QFontDatabase::applicationFontFamilies(id);
  return fams.isEmpty() ? QString() : fams.first();
}

void Icons::loadFonts() {
  outlineFamily_ = addFont(":/fonts/tabler-icons.ttf");
  filledFamily_ = addFont(":/fonts/tabler-icons-filled.ttf");
}

static QHash<QString, uint> readTable(const QJsonObject &obj) {
  QHash<QString, uint> table;
  for (auto it = obj.begin(); it != obj.end(); ++it)
    table.insert(it.key(), static_cast<uint>(it.value().toDouble()));
  return table;
}

void Icons::loadMap() {
  QFile f(":/data/tabler_glyphs.json");
  if (!f.open(QIODevice::ReadOnly)) return;
  const QJsonObject root = QJsonDocument::fromJson(f.readAll()).object();
  outline_ = readTable(root.value("outline").toObject());
  filled_ = readTable(root.value("filled").toObject());
}

// Normalises a raw icon name: strips a leading "ti-" and reports whether the
// name requested the filled variant.
static QString normalise(const QString &raw, bool *filled) {
  QString name = raw.trimmed();
  if (name.startsWith("ti-")) name = name.mid(3);
  *filled = false;
  if (name.endsWith("-filled")) {
    name.chop(7);
    *filled = true;
  }
  return name;
}

QString Icons::glyph(const QString &raw) const {
  bool filled = false;
  const QString name = normalise(raw, &filled);

  if (filled && filled_.contains(name))
    return QString(QChar(filled_.value(name)));
  if (outline_.contains(name)) return QString(QChar(outline_.value(name)));
  if (filled_.contains(name)) return QString(QChar(filled_.value(name)));

  // upstream fallback glyph
  if (outline_.contains("background"))
    return QString(QChar(outline_.value("background")));
  return QString();
}

QFont Icons::font(const QString &raw, int pixelSize) const {
  bool filled = false;
  const QString name = normalise(raw, &filled);
  const bool useFilled = filled && filled_.contains(name);
  QFont f(useFilled ? filledFamily_ : outlineFamily_);
  f.setPixelSize(pixelSize);
  return f;
}

}  // namespace neobar
