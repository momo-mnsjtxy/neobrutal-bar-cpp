#pragma once

#include <QNetworkAccessManager>

#include "Types.h"
#include "providers/Provider.h"

namespace neobar {

// Current weather from the Open-Meteo API (no key required). When no location
// is configured it is resolved once via IP geolocation. Network access is
// asynchronous; updated() fires again once a response arrives.
class WeatherProvider : public Provider {
  Q_OBJECT
 public:
  WeatherProvider(bool hasLocation, double lat, double lon,
                  QObject *parent = nullptr);

  WeatherInfo info() const { return info_; }

 protected:
  void refresh() override;

 private:
  void requestGeo();
  void requestWeather();
  static WeatherStatus mapCode(int code, bool day);

  QNetworkAccessManager net_;
  WeatherInfo info_;
  bool hasLocation_ = false;
  double lat_ = 0.0;
  double lon_ = 0.0;
};

}  // namespace neobar
