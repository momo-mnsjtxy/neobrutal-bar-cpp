#include "widgets/Meter.h"

#include <QPainter>

namespace neobar {

namespace {
constexpr int kWidth = 48;   // tailwind w-12
constexpr int kHeight = 16;  // tailwind h-4
constexpr int kBorder = 1;
}  // namespace

Meter::Meter(const QColor &fill, const QColor &border, const QColor &text,
             QWidget *parent)
    : QWidget(parent), fill_(fill), border_(border), text_(text) {
  setFixedSize(kWidth, kHeight);
  setAttribute(Qt::WA_Hover, true);
}

void Meter::setPercent(int percent) {
  percent = qBound(0, percent, 100);
  if (percent == percent_) return;
  percent_ = percent;
  update();
}

void Meter::setFill(const QColor &fill) {
  if (fill_ == fill) return;
  fill_ = fill;
  update();
}

void Meter::enterEvent(QEnterEvent *) {
  hovered_ = true;
  update();
}

void Meter::leaveEvent(QEvent *) {
  hovered_ = false;
  update();
}

void Meter::paintEvent(QPaintEvent *) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);

  const QRectF box(kBorder / 2.0, kBorder / 2.0, width() - kBorder,
                  height() - kBorder);

  if (hovered_) {
    p.setPen(text_);
    QFont f = font();
    f.setPixelSize(11);
    p.setFont(f);
    p.drawText(rect(), Qt::AlignCenter, QString("%1%").arg(percent_));
    return;
  }

  // filled portion
  const double fillW = (width() - 2.0 * kBorder) * percent_ / 100.0;
  if (fillW > 0) {
    p.setPen(Qt::NoPen);
    p.setBrush(fill_);
    p.drawRect(QRectF(kBorder, kBorder, fillW, height() - 2.0 * kBorder));
  }

  // border
  QPen pen(border_);
  pen.setWidth(kBorder);
  p.setPen(pen);
  p.setBrush(Qt::NoBrush);
  p.drawRoundedRect(box, 2, 2);
}

}  // namespace neobar
