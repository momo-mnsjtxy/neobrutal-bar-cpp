#pragma once

#include "Types.h"
#include "providers/Provider.h"

namespace neobar {

// Currently playing media, read over MPRIS (D-Bus) — the Linux analogue of the
// upstream Spotify/now-playing widget. Prefers an actively playing player and
// otherwise reports the first available one.
class MediaProvider : public Provider {
  Q_OBJECT
 public:
  using Provider::Provider;
  MediaInfo info() const { return info_; }

 protected:
  void refresh() override;

 private:
  MediaInfo info_;
};

}  // namespace neobar
