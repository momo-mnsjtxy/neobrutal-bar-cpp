#pragma once

#include "Types.h"
#include "providers/Provider.h"

#ifdef Q_OS_WIN
#include <atomic>
#include <mutex>
#include <thread>
#endif

namespace neobar {

// Currently playing media. On Linux this reads MPRIS over D-Bus; on Windows it
// reads the System Media Transport Controls (Windows.Media.Control) — the OS
// "now playing" session shared by Spotify, browsers, etc. Prefers an actively
// playing session and otherwise reports the current one.
class MediaProvider : public Provider {
  Q_OBJECT
 public:
  explicit MediaProvider(QObject *parent = nullptr);
  ~MediaProvider() override;

  MediaInfo info() const;

 protected:
  void refresh() override;

 private:
#ifdef Q_OS_WIN
  // WinRT async APIs must run in a multithreaded apartment, so a worker thread
  // polls the session and publishes results under a mutex; refresh() is a no-op
  // and info() returns the latest snapshot.
  void pollLoop();

  mutable std::mutex mutex_;
  MediaInfo shared_;
  std::thread worker_;
  std::atomic<bool> stop_{false};
#else
  MediaInfo info_;
#endif
};

}  // namespace neobar
