#pragma once

#include <QColor>
#include <QString>

class QLabel;
class QWidget;

namespace neobar {

// Returns an "rgba(r,g,b,a)" string usable in a Qt stylesheet.
QString cssColor(const QColor &c);

// Builds a QLabel rendering the Tabler icon `name` in `color` at `pixelSize`.
QLabel *makeIconLabel(const QString &name, const QColor &color, int pixelSize,
                      QWidget *parent = nullptr);

// Builds a plain text QLabel in `color` at `pixelSize` with `weight`.
QLabel *makeTextLabel(const QString &text, const QColor &color, int pixelSize,
                      int weight, QWidget *parent = nullptr);

// Updates an existing QLabel to render the Tabler icon `name` in `color`.
void setIconLabel(QLabel *label, const QString &name, const QColor &color,
                  int pixelSize);

}  // namespace neobar
