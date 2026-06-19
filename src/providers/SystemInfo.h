#pragma once

#include <QDateTime>
#include <QString>

#include "Types.h"
#include "providers/Provider.h"

namespace neobar {

// CPU usage (%) derived from successive /proc/stat samples.
class CpuProvider : public Provider {
  Q_OBJECT
 public:
  using Provider::Provider;
  double usage() const { return usage_; }

 protected:
  void refresh() override;

 private:
  double usage_ = 0.0;
  unsigned long long lastIdle_ = 0;
  unsigned long long lastTotal_ = 0;
};

// Memory usage (%) from /proc/meminfo.
class MemoryProvider : public Provider {
  Q_OBJECT
 public:
  using Provider::Provider;
  double usage() const { return usage_; }

 protected:
  void refresh() override;

 private:
  double usage_ = 0.0;
};

// Battery charge (%) and charging state from /sys/class/power_supply.
class BatteryProvider : public Provider {
  Q_OBJECT
 public:
  using Provider::Provider;
  bool present() const { return present_; }
  int chargePercent() const { return charge_; }
  bool charging() const { return charging_; }

 protected:
  void refresh() override;

 private:
  bool present_ = false;
  int charge_ = 100;
  bool charging_ = false;
};

// Default network interface type, wifi signal strength and SSID.
class NetworkProvider : public Provider {
  Q_OBJECT
 public:
  using Provider::Provider;
  NetworkInfo info() const { return info_; }

 protected:
  void refresh() override;

 private:
  NetworkInfo info_;
};

// Local clock, formatted like the upstream `date` provider ("HH:mm").
class DateProvider : public Provider {
  Q_OBJECT
 public:
  explicit DateProvider(QString format = "HH:mm", QObject *parent = nullptr)
      : Provider(parent), format_(std::move(format)) {}
  QString formatted() const { return formatted_; }

 protected:
  void refresh() override {
    formatted_ = QDateTime::currentDateTime().toString(format_);
  }

 private:
  QString format_;
  QString formatted_;
};

}  // namespace neobar
