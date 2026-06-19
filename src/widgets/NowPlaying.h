#pragma once

#include <QWidget>

#include "Theme.h"
#include "Types.h"

class QLabel;

namespace neobar {

// Now-playing indicator (NowPlaying.svelte): a music icon plus the current
// track title, hidden entirely when no MPRIS player is present.
class NowPlaying : public QWidget {
  Q_OBJECT
 public:
  explicit NowPlaying(const Theme &theme, QWidget *parent = nullptr);

  void setData(const MediaInfo &info);

 private:
  Theme theme_;
  QLabel *icon_;
  QLabel *title_;
};

}  // namespace neobar
