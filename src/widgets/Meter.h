#pragma once

#include <QColor>
#include <QWidget>

namespace neobar {

// A small bordered usage bar (memory/cpu/battery), mirroring the upstream
// Meter.svelte: a filled track that, on hover, reveals the numeric percentage.
class Meter : public QWidget {
  Q_OBJECT
 public:
  Meter(const QColor &fill, const QColor &border, const QColor &text,
        QWidget *parent = nullptr);

  void setPercent(int percent);
  void setFill(const QColor &fill);

 protected:
  void paintEvent(QPaintEvent *) override;
  void enterEvent(QEnterEvent *) override;
  void leaveEvent(QEvent *) override;

 private:
  QColor fill_;
  QColor border_;
  QColor text_;
  int percent_ = 0;
  bool hovered_ = false;
};

}  // namespace neobar
