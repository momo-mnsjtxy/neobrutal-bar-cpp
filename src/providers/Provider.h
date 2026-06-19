#pragma once

#include <QObject>
#include <QTimer>

namespace neobar {

// Base class for all data providers — the C++ analogue of a Zebar provider.
// Subclasses implement refresh() to update their cached state; the base class
// drives periodic polling and emits updated() after every refresh.
class Provider : public QObject {
  Q_OBJECT
 public:
  explicit Provider(QObject *parent = nullptr) : QObject(parent) {
    connect(&timer_, &QTimer::timeout, this, &Provider::tick);
  }

  // Begins polling every `intervalMs` milliseconds (after an immediate first
  // refresh).
  void start(int intervalMs) {
    tick();
    timer_.start(intervalMs);
  }

 signals:
  void updated();

 protected:
  virtual void refresh() = 0;

 private:
  void tick() {
    refresh();
    emit updated();
  }

  QTimer timer_;
};

}  // namespace neobar
