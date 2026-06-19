#pragma once

#include <QAbstractButton>
#include <QColor>
#include <QString>

#include "Theme.h"

namespace neobar {

// A neobrutalist icon button: a bordered face floating over a hard offset
// shadow that "presses" into the shadow on hover (mirrors Button.svelte's
// hover:translate + shadow-none behaviour).
class NeoButton : public QAbstractButton {
  Q_OBJECT
 public:
  NeoButton(const QString &iconName, const QColor &glyphColor,
            const Theme &theme, int iconSize, QWidget *parent = nullptr);

  void setIconName(const QString &iconName);
  void setGlyphColor(const QColor &color);

  QSize sizeHint() const override;

 protected:
  void paintEvent(QPaintEvent *) override;
  void enterEvent(QEnterEvent *) override;
  void leaveEvent(QEvent *) override;

 private:
  QString iconName_;
  QColor glyphColor_;
  Theme theme_;
  int iconSize_;
  bool hovered_ = false;

  int pad() const { return 4; }
  int faceW() const;
  int faceH() const;
};

}  // namespace neobar
