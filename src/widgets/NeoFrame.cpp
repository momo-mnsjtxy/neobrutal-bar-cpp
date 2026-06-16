#include "widgets/NeoFrame.h"

#include <QHBoxLayout>
#include <QPainter>

namespace neobar {

namespace {
constexpr int kPad = 4;  // tailwind p-1
}

NeoFrame::NeoFrame(const Theme &theme, QWidget *parent)
    : QWidget(parent), theme_(theme) {
  setAttribute(Qt::WA_TranslucentBackground, true);
  setFixedHeight(theme_.height + theme_.shadowSizeBar);

  const int inset = theme_.borderSize + kPad;
  body_ = new QHBoxLayout(this);
  body_->setContentsMargins(inset, inset, inset + theme_.shadowSizeBar,
                            inset + theme_.shadowSizeBar);
  body_->setSpacing(12);  // tailwind gap-3
}

void NeoFrame::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const int s = theme_.shadowSizeBar;
  const int r = theme_.radius;
  const QRectF face(0, 0, width() - s, height() - s);

  // hard offset shadow
  p.setPen(Qt::NoPen);
  p.setBrush(theme_.shadow);
  p.drawRoundedRect(QRectF(s, s, width() - s, height() - s), r, r);

  // face
  QPen border(theme_.border);
  border.setWidth(theme_.borderSize);
  p.setPen(border);
  p.setBrush(theme_.base);
  const double inset = theme_.borderSize / 2.0;
  p.drawRoundedRect(face.adjusted(inset, inset, -inset, -inset), r, r);
}

}  // namespace neobar
