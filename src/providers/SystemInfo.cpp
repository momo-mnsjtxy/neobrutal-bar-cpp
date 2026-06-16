#include "providers/SystemInfo.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace neobar {

static QString readFirstLine(const QString &path) {
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
  return QString::fromUtf8(f.readLine()).trimmed();
}

void CpuProvider::refresh() {
  const QString line = readFirstLine("/proc/stat");  // "cpu  u n s i io irq..."
  const QStringList parts = line.split(' ', Qt::SkipEmptyParts);
  if (parts.size() < 5 || parts[0] != "cpu") return;

  unsigned long long total = 0;
  unsigned long long idle = 0;
  for (int i = 1; i < parts.size(); ++i) {
    const unsigned long long v = parts[i].toULongLong();
    total += v;
    if (i == 4 || i == 5) idle += v;  // idle + iowait
  }

  const unsigned long long dTotal = total - lastTotal_;
  const unsigned long long dIdle = idle - lastIdle_;
  if (lastTotal_ != 0 && dTotal > 0)
    usage_ = 100.0 * static_cast<double>(dTotal - dIdle) / dTotal;
  lastTotal_ = total;
  lastIdle_ = idle;
}

void MemoryProvider::refresh() {
  QFile f("/proc/meminfo");
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  unsigned long long total = 0;
  unsigned long long available = 0;
  QTextStream in(&f);
  for (QString l = in.readLine(); !l.isNull(); l = in.readLine()) {
    if (l.startsWith("MemTotal:"))
      total = l.split(' ', Qt::SkipEmptyParts).value(1).toULongLong();
    else if (l.startsWith("MemAvailable:"))
      available = l.split(' ', Qt::SkipEmptyParts).value(1).toULongLong();
    if (total && available) break;
  }
  if (total > 0)
    usage_ = 100.0 * static_cast<double>(total - available) / total;
}

void BatteryProvider::refresh() {
  present_ = false;
  const QDir dir("/sys/class/power_supply");
  const auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &name : entries) {
    const QString base = dir.absoluteFilePath(name);
    if (readFirstLine(base + "/type") != "Battery") continue;
    const QString cap = readFirstLine(base + "/capacity");
    if (cap.isEmpty()) continue;
    present_ = true;
    charge_ = cap.toInt();
    charging_ = readFirstLine(base + "/status") == "Charging";
    return;
  }
}

void NetworkProvider::refresh() {
  NetworkInfo info;

  // Find the interface backing the default route via /proc/net/route.
  QString defaultIface;
  QFile route("/proc/net/route");
  if (route.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&route);
    in.readLine();  // header
    for (QString l = in.readLine(); !l.isNull(); l = in.readLine()) {
      const QStringList c = l.split('\t', Qt::SkipEmptyParts);
      if (c.size() >= 2 && c[1] == "00000000") {  // destination 0.0.0.0
        defaultIface = c[0];
        break;
      }
    }
  }

  if (!defaultIface.isEmpty()) {
    const bool wireless =
        QFileInfo::exists("/sys/class/net/" + defaultIface + "/wireless");
    info.type = wireless ? NetType::Wifi : NetType::Ethernet;

    if (wireless) {
      // Link quality (col 3 of /proc/net/wireless, out of ~70) -> percent.
      QFile w("/proc/net/wireless");
      if (w.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&w);
        in.readLine();
        in.readLine();  // two header lines
        for (QString l = in.readLine(); !l.isNull(); l = in.readLine()) {
          if (!l.trimmed().startsWith(defaultIface)) continue;
          const QStringList c = l.split(' ', Qt::SkipEmptyParts);
          if (c.size() >= 3) {
            const double q = QString(c[2]).remove('.').toDouble();
            info.signalStrength =
                qBound(0, static_cast<int>(q / 70.0 * 100.0), 100);
          }
          break;
        }
      }
    }
  }

  info_ = info;
}

void DateProvider::refresh() {
  formatted_ = QDateTime::currentDateTime().toString(format_);
}

}  // namespace neobar
