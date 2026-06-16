#include "providers/MediaProvider.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QStringList>
#include <QVariantMap>

namespace neobar {

namespace {

constexpr char kPropsIface[] = "org.freedesktop.DBus.Properties";
constexpr char kPlayerIface[] = "org.mpris.MediaPlayer2.Player";
constexpr char kObjectPath[] = "/org/mpris/MediaPlayer2";

QVariant getProperty(const QDBusConnection &bus, const QString &service,
                     const QString &name) {
  QDBusInterface props(service, kObjectPath, kPropsIface, bus);
  if (!props.isValid()) return {};
  const QDBusReply<QVariant> reply =
      props.call("Get", QString(kPlayerIface), name);
  return reply.isValid() ? reply.value() : QVariant();
}

QString playbackStatus(const QDBusConnection &bus, const QString &service) {
  return getProperty(bus, service, "PlaybackStatus").toString();
}

void readMetadata(const QDBusConnection &bus, const QString &service,
                  MediaInfo *info) {
  const QVariant raw = getProperty(bus, service, "Metadata");
  QVariantMap map;
  if (raw.canConvert<QDBusArgument>()) {
    QDBusArgument arg = raw.value<QDBusArgument>();
    arg >> map;
  } else {
    map = raw.toMap();
  }

  info->title = map.value("xesam:title").toString();

  const QVariant artistVar = map.value("xesam:artist");
  QStringList artists;
  if (artistVar.canConvert<QDBusArgument>()) {
    QDBusArgument arg = artistVar.value<QDBusArgument>();
    arg >> artists;
  } else {
    artists = artistVar.toStringList();
  }
  info->artist = artists.join(", ");
}

}  // namespace

void MediaProvider::refresh() {
  MediaInfo info;
  QDBusConnection bus = QDBusConnection::sessionBus();
  if (!bus.isConnected()) {
    info_ = info;
    return;
  }

  QDBusInterface dbus("org.freedesktop.DBus", "/org/freedesktop/DBus",
                     "org.freedesktop.DBus", bus);
  const QDBusReply<QStringList> names = dbus.call("ListNames");
  if (!names.isValid()) {
    info_ = info;
    return;
  }

  QString chosen;
  for (const QString &name : names.value()) {
    if (!name.startsWith("org.mpris.MediaPlayer2.")) continue;
    if (chosen.isEmpty()) chosen = name;  // fallback: first available
    if (playbackStatus(bus, name) == "Playing") {
      chosen = name;
      break;
    }
  }

  if (!chosen.isEmpty()) {
    info.hasPlayer = true;
    info.playing = playbackStatus(bus, chosen) == "Playing";
    readMetadata(bus, chosen, &info);
  }

  info_ = info;
}

}  // namespace neobar
