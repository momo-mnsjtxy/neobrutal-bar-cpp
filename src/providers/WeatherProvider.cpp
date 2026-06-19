#include "providers/WeatherProvider.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace neobar {

WeatherProvider::WeatherProvider(bool hasLocation, double lat, double lon,
                                 QObject *parent)
    : Provider(parent), hasLocation_(hasLocation), lat_(lat), lon_(lon) {}

void WeatherProvider::refresh() {
  if (hasLocation_)
    requestWeather();
  else
    requestGeo();
}

void WeatherProvider::requestGeo() {
  QNetworkRequest req(QUrl("http://ip-api.com/json/?fields=lat,lon,status"));
  QNetworkReply *reply = net_.get(req);
  connect(reply, &QNetworkReply::finished, this, [this, reply] {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) return;
    const QJsonObject o =
        QJsonDocument::fromJson(reply->readAll()).object();
    if (o.value("status").toString() != "success") return;
    lat_ = o.value("lat").toDouble();
    lon_ = o.value("lon").toDouble();
    hasLocation_ = true;
    requestWeather();
  });
}

void WeatherProvider::requestWeather() {
  const QString url =
      QString(
          "https://api.open-meteo.com/v1/forecast?latitude=%1&longitude=%2"
          "&current=temperature_2m,weather_code,is_day")
          .arg(lat_)
          .arg(lon_);
  QNetworkReply *reply = net_.get(QNetworkRequest(QUrl(url)));
  connect(reply, &QNetworkReply::finished, this, [this, reply] {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) return;
    const QJsonObject root =
        QJsonDocument::fromJson(reply->readAll()).object();
    const QJsonObject cur = root.value("current").toObject();
    if (cur.isEmpty()) return;

    WeatherInfo info;
    info.valid = true;
    info.celsius = cur.value("temperature_2m").toDouble();
    info.status = mapCode(cur.value("weather_code").toInt(),
                          cur.value("is_day").toInt() == 1);
    info_ = info;
    emit updated();
  });
}

WeatherStatus WeatherProvider::mapCode(int code, bool day) {
  // WMO weather interpretation codes -> the upstream status buckets.
  auto pick = [day](WeatherStatus d, WeatherStatus n) { return day ? d : n; };
  if (code == 0 || code == 1)
    return pick(WeatherStatus::ClearDay, WeatherStatus::ClearNight);
  if (code == 2 || code == 3 || code == 45 || code == 48)
    return pick(WeatherStatus::CloudyDay, WeatherStatus::CloudyNight);
  if ((code >= 51 && code <= 57) || code == 61 || code == 80)
    return pick(WeatherStatus::LightRainDay, WeatherStatus::LightRainNight);
  if (code == 63 || code == 65 || code == 66 || code == 67 || code == 81 ||
      code == 82)
    return pick(WeatherStatus::HeavyRainDay, WeatherStatus::HeavyRainNight);
  if ((code >= 71 && code <= 77) || code == 85 || code == 86)
    return pick(WeatherStatus::SnowDay, WeatherStatus::SnowNight);
  if (code == 95 || code == 96 || code == 99)
    return pick(WeatherStatus::ThunderDay, WeatherStatus::ThunderNight);
  return WeatherStatus::Unknown;
}

}  // namespace neobar
