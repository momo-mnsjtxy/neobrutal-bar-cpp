#include "widgets/Bar.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QScreen>

#include "providers/MediaProvider.h"
#include "providers/SystemInfo.h"
#include "providers/WeatherProvider.h"
#include "providers/WorkspaceProvider.h"
#include "widgets/Meter.h"
#include "widgets/NeoButton.h"
#include "widgets/NeoFrame.h"
#include "widgets/NowPlaying.h"
#include "widgets/UiHelpers.h"
#include "widgets/Workspaces.h"

namespace neobar {

namespace {

QString weatherIconName(WeatherStatus s) {
  switch (s) {
    case WeatherStatus::ClearDay:
      return "ti-sun";
    case WeatherStatus::ClearNight:
      return "ti-moon";
    case WeatherStatus::CloudyDay:
    case WeatherStatus::CloudyNight:
      return "ti-cloud";
    case WeatherStatus::LightRainDay:
    case WeatherStatus::LightRainNight:
    case WeatherStatus::HeavyRainDay:
    case WeatherStatus::HeavyRainNight:
      return "ti-cloud-rain";
    case WeatherStatus::SnowDay:
    case WeatherStatus::SnowNight:
      return "ti-snowflake";
    case WeatherStatus::ThunderDay:
    case WeatherStatus::ThunderNight:
      return "ti-bolt";
    default:
      return {};
  }
}

QWidget *iconValueBlock(const QString &iconName, const QColor &color, int size,
                        QLabel **outIcon, QLabel **outText, int weight,
                        QWidget *parent) {
  auto *block = new QWidget(parent);
  auto *h = new QHBoxLayout(block);
  h->setContentsMargins(0, 0, 0, 0);
  h->setSpacing(4);
  *outIcon = makeIconLabel(iconName, color, size, block);
  *outText = makeTextLabel("", color, size, weight, block);
  h->addWidget(*outIcon);
  h->addWidget(*outText);
  return block;
}

}  // namespace

Bar::Bar(const Config &config, QWidget *parent)
    : QWidget(parent), config_(config), theme_(config.theme) {
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                 Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setWindowTitle("neobrutal-bar");

  cpu_ = new CpuProvider(this);
  mem_ = new MemoryProvider(this);
  bat_ = new BatteryProvider(this);
  net_ = new NetworkProvider(this);
  date_ = new DateProvider("HH:mm", this);
  weather_ = new WeatherProvider(config_.hasLocation, config_.latitude,
                                config_.longitude, this);
  ws_ = new WorkspaceProvider(this);
  media_ = new MediaProvider(this);

  auto *root = new QHBoxLayout(this);
  root->setContentsMargins(theme_.barMarginX, theme_.barMarginY,
                           theme_.barMarginX, theme_.barMarginY);
  root->setSpacing(0);
  root->addWidget(buildLeftGroup(), 0, Qt::AlignVCenter | Qt::AlignLeft);
  root->addStretch(1);
  root->addWidget(buildCenterGroup(), 0, Qt::AlignCenter);
  root->addStretch(1);
  root->addWidget(buildRightGroup(), 0, Qt::AlignVCenter | Qt::AlignRight);

  connect(cpu_, &Provider::updated, this, [this] { updateCpu(); });
  connect(mem_, &Provider::updated, this,
          [this] { memMeter_->setPercent(qRound(mem_->usage())); });
  connect(bat_, &Provider::updated, this, [this] { updateBattery(); });
  connect(net_, &Provider::updated, this, [this] { updateNetwork(); });
  connect(date_, &Provider::updated, this, [this] { updateClock(); });
  connect(weather_, &Provider::updated, this, [this] { updateWeather(); });
  connect(ws_, &Provider::updated, this, [this] { updateWorkspaces(); });
  connect(media_, &Provider::updated, this, [this] { updateMedia(); });

  positionOnScreen();

  cpu_->start(1500);
  mem_->start(2000);
  bat_->start(5000);
  net_->start(3000);
  date_->start(1000);
  weather_->start(10 * 60 * 1000);
  ws_->start(400);
  media_->start(1000);
}

QWidget *Bar::buildLeftGroup() {
  auto *frame = new NeoFrame(theme_, this);
  auto *body = frame->body();

  body->addWidget(
      new NeoButton("ti-heart-filled", theme_.icon, theme_, theme_.fontSize,
                    frame));

  auto meterBlock = [&](const QString &icon, Meter *meter) {
    auto *block = new QWidget(frame);
    auto *h = new QHBoxLayout(block);
    h->setContentsMargins(0, 0, 0, 0);
    h->setSpacing(4);
    h->addWidget(makeIconLabel(icon, theme_.text, theme_.fontSize, block));
    h->addWidget(meter);
    return block;
  };

  memMeter_ = new Meter(theme_.memory, theme_.border, theme_.text, frame);
  cpuMeter_ = new Meter(theme_.cpu, theme_.border, theme_.text, frame);
  batMeter_ = new Meter(theme_.batteryGood, theme_.border, theme_.text, frame);

  body->addWidget(meterBlock("ti-ruler-2", memMeter_));
  body->addWidget(meterBlock("ti-cpu", cpuMeter_));
  batBlock_ = meterBlock("ti-bolt", batMeter_);
  body->addWidget(batBlock_);

  return frame;
}

QWidget *Bar::buildCenterGroup() {
  auto *frame = new NeoFrame(theme_, this);
  workspaces_ = new Workspaces(theme_, frame);
  connect(workspaces_, &Workspaces::switchRequested, this, [this](int idx) {
    if (ws_) ws_->switchToDesktop(idx);
  });
  frame->body()->addWidget(workspaces_);
  return frame;
}

QWidget *Bar::buildRightGroup() {
  auto *frame = new NeoFrame(theme_, this);
  auto *body = frame->body();

  nowPlaying_ = new NowPlaying(theme_, frame);
  body->addWidget(nowPlaying_);

  body->addWidget(iconValueBlock("ti-wifi-off", theme_.network, theme_.fontSize,
                                 &netIcon_, &netText_, theme_.fontWeight,
                                 frame));

  weatherBlock_ =
      iconValueBlock("ti-sun", theme_.weather, theme_.fontSize, &weatherIcon_,
                     &weatherText_, theme_.fontWeight, frame);
  weatherBlock_->setVisible(false);
  body->addWidget(weatherBlock_);

  body->addWidget(
      makeIconLabel("ti-point-filled", theme_.text, theme_.fontSize, frame));

  clock_ = makeTextLabel("--:--", theme_.text, theme_.fontSize,
                         theme_.fontWeight, frame);
  body->addWidget(clock_);

  return frame;
}

void Bar::positionOnScreen() {
  const QList<QScreen *> screens = QGuiApplication::screens();
  QScreen *screen = QApplication::primaryScreen();
  if (config_.screen >= 0 && config_.screen < screens.size())
    screen = screens.at(config_.screen);
  const QRect g = screen ? screen->geometry() : QRect(0, 0, 1920, 1080);

  const int h = theme_.height + theme_.shadowSizeBar + 2 * theme_.barMarginY;
  setFixedHeight(h);
  setGeometry(g.x(), g.y(), g.width(), h);
}

void Bar::updateCpu() {
  const int pct = qRound(cpu_->usage());
  cpuMeter_->setFill(pct >= 80 ? theme_.cpuHighUsage : theme_.cpu);
  cpuMeter_->setPercent(pct);
}

void Bar::updateBattery() {
  if (!bat_->present()) {
    batBlock_->setVisible(false);
    return;
  }
  batBlock_->setVisible(true);
  const int pct = bat_->chargePercent();
  QColor c = theme_.batteryGood;
  if (pct < 20)
    c = theme_.batteryLow;
  else if (pct < 50)
    c = theme_.batteryMid;
  batMeter_->setFill(c);
  batMeter_->setPercent(pct);
}

void Bar::updateNetwork() {
  const NetworkInfo info = net_->info();
  QString icon = "ti-wifi-off";
  QString text;

  if (info.type == NetType::Ethernet) {
    icon = "ti-network";
  } else if (info.type == NetType::Wifi) {
    if (info.signalStrength >= 75)
      icon = "ti-wifi";
    else if (info.signalStrength >= 50)
      icon = "ti-wifi-2";
    else if (info.signalStrength >= 25)
      icon = "ti-wifi-1";
    else
      icon = "ti-wifi-off";
    text = info.ssid;
  }

  setIconLabel(netIcon_, icon, theme_.network, theme_.fontSize);
  netText_->setText(text);
  netText_->setVisible(!text.isEmpty());
}

void Bar::updateWeather() {
  const WeatherInfo info = weather_->info();
  const QString icon = info.valid ? weatherIconName(info.status) : QString();
  if (!info.valid || icon.isEmpty()) {
    weatherBlock_->setVisible(false);
    return;
  }
  weatherBlock_->setVisible(true);
  setIconLabel(weatherIcon_, icon, theme_.weather, theme_.fontSize);
  weatherText_->setText(QString("%1°").arg(qRound(info.celsius)));
}

void Bar::updateClock() { clock_->setText(date_->formatted()); }

void Bar::updateMedia() { nowPlaying_->setData(media_->info()); }

void Bar::updateWorkspaces() { workspaces_->setData(ws_->data()); }

}  // namespace neobar
