#include "providers/MediaProvider.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>

#include <chrono>

namespace neobar {

namespace {

QString toQString(const winrt::hstring &s) {
  return QString::fromWCharArray(s.c_str(), static_cast<int>(s.size()));
}

}  // namespace

MediaProvider::MediaProvider(QObject *parent) : Provider(parent) {
  worker_ = std::thread([this] { pollLoop(); });
}

MediaProvider::~MediaProvider() {
  stop_.store(true);
  if (worker_.joinable()) worker_.join();
}

MediaInfo MediaProvider::info() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return shared_;
}

void MediaProvider::refresh() {
  // The worker thread keeps shared_ fresh; info() returns the latest snapshot.
}

void MediaProvider::pollLoop() {
  using namespace winrt::Windows::Media::Control;
  using Status = GlobalSystemMediaTransportControlsSessionPlaybackStatus;

  winrt::init_apartment(winrt::apartment_type::multi_threaded);

  while (!stop_.load()) {
    MediaInfo info;
    try {
      auto manager =
          GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
      auto session = manager.GetCurrentSession();
      if (session) {
        info.hasPlayer = true;
        info.playing =
            session.GetPlaybackInfo().PlaybackStatus() == Status::Playing;
        const auto props = session.TryGetMediaPropertiesAsync().get();
        info.title = toQString(props.Title());
        info.artist = toQString(props.Artist());
      }
    } catch (...) {
      info = MediaInfo{};  // SMTC can throw transiently; report no player.
    }

    {
      std::lock_guard<std::mutex> lock(mutex_);
      shared_ = info;
    }

    for (int i = 0; i < 10 && !stop_.load(); ++i)
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  winrt::uninit_apartment();
}

}  // namespace neobar
