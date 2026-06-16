#include "widgets/UiHelpers.h"

#include <QLabel>

#include "Icons.h"

namespace neobar {

QString cssColor(const QColor &c) {
  return QString("rgba(%1,%2,%3,%4)")
      .arg(c.red())
      .arg(c.green())
      .arg(c.blue())
      .arg(c.alpha());
}

QLabel *makeIconLabel(const QString &name, const QColor &color, int pixelSize,
                      QWidget *parent) {
  auto *label = new QLabel(parent);
  label->setFont(Icons::instance().font(name, pixelSize));
  label->setText(Icons::instance().glyph(name));
  label->setStyleSheet(
      QString("color:%1; background:transparent;").arg(cssColor(color)));
  label->setAlignment(Qt::AlignCenter);
  return label;
}

void setIconLabel(QLabel *label, const QString &name, const QColor &color,
                  int pixelSize) {
  label->setFont(Icons::instance().font(name, pixelSize));
  label->setText(Icons::instance().glyph(name));
  label->setStyleSheet(
      QString("color:%1; background:transparent;").arg(cssColor(color)));
}

QLabel *makeTextLabel(const QString &text, const QColor &color, int pixelSize,
                      int weight, QWidget *parent) {
  auto *label = new QLabel(text, parent);
  QFont f = label->font();
  f.setPixelSize(pixelSize);
  f.setWeight(static_cast<QFont::Weight>(weight));
  label->setFont(f);
  label->setStyleSheet(
      QString("color:%1; background:transparent;").arg(cssColor(color)));
  return label;
}

}  // namespace neobar
