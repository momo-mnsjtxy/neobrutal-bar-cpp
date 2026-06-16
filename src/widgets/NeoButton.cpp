#include "widgets/NeoButton.h"

#include <QPainter>

#include "Icons.h"

namespace neobar {

NeoButton::NeoButton(const QString &iconName, const QColor &glyphColor,
                     const Theme &theme, int iconSize, QWidget *parent)
    : QAbstractButton(parent),
      iconName_(iconName),
      glyphColor_(glyphColor),
      theme_(theme),
      iconSize_(iconSize) {
  setCursor(Qt::PointingHandCursor);
  setAttribute(Qt::WA_Hover, true);
}

int NeoButton::faceW() const { return iconSize_ + 2 * pad(); }
int NeoButton::faceH() const { return iconSize_ + 2 * pad(); }

QSize NeoButton::sizeHint() const {
  return {faceW() + theme_.shadowSizeButton, faceH() + theme_.shadowSizeButton};
}

void NeoButton::setIconName(const QString &iconName) {
  if (iconName_ == iconName) return;
  iconName_ = iconName;
  update();
}

void NeoButton::setGlyphColor(const QColor &color) {
  glyphColor_ = color;
  update();
}

void NeoButton::enterEvent(QEnterEvent *) {
  hovered_ = true;
  update();
}

void NeoButton::leaveEvent(QEvent *) {
  hovered_ = false;
  update();
}

void NeoButton::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const int s = theme_.shadowSizeButton;
  const int r = theme_.radius;
  const QRectF face = hovered_ ? QRectF(s, s, faceW(), faceH())
                               : QRectF(0, 0, faceW(), faceH());

  if (!hovered_) {
    p.setPen(Qt::NoPen);
    p.setBrush(theme_.shadow);
    p.drawRoundedRect(QRectF(s, s, faceW(), faceH()), r, r);
  }

  QColor bg = glyphColor_;
  bg.setAlphaF(0.30);  // bg-blend-30: currentColor at 30%
  QPen border(theme_.border);
  border.setWidth(theme_.borderSize);
  p.setPen(border);
  p.setBrush(bg);
  const double inset = theme_.borderSize / 2.0;
  p.drawRoundedRect(face.adjusted(inset, inset, -inset, -inset), r, r);

  p.setPen(glyphColor_);
  p.setFont(Icons::instance().font(iconName_, iconSize_));
  p.drawText(face, Qt::AlignCenter, Icons::instance().glyph(iconName_));
}

}  // namespace neobar
