#include "widgets/NowPlaying.h"

#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>

#include "Icons.h"
#include "widgets/UiHelpers.h"

namespace neobar {

NowPlaying::NowPlaying(const Theme &theme, QWidget *parent)
    : QWidget(parent), theme_(theme) {
  auto *row = new QHBoxLayout(this);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(4);  // tailwind gap-1

  icon_ = makeIconLabel("ti-music", theme_.nowPlaying, theme_.fontSize, this);
  title_ = makeTextLabel("", theme_.displayed, theme_.fontSize,
                         theme_.fontWeight, this);
  row->addWidget(icon_);
  row->addWidget(title_);
}

void NowPlaying::setData(const MediaInfo &info) {
  if (!info.hasPlayer) {
    setVisible(false);
    return;
  }
  setVisible(true);

  if (info.playing) {
    icon_->setFont(Icons::instance().font("ti-music", theme_.fontSize));
    icon_->setText(Icons::instance().glyph("ti-music"));
    icon_->setStyleSheet(QString("color:%1; background:transparent;")
                             .arg(cssColor(theme_.nowPlaying)));
    QString text = info.title;
    if (!info.artist.isEmpty()) text = info.artist + " - " + info.title;
    const QFontMetrics fm(title_->font());
    title_->setText(fm.elidedText(text, Qt::ElideRight, 320));
    title_->setVisible(true);
  } else {
    icon_->setFont(Icons::instance().font("ti-music-off", theme_.fontSize));
    icon_->setText(Icons::instance().glyph("ti-music-off"));
    icon_->setStyleSheet(QString("color:%1; background:transparent;")
                             .arg(cssColor(theme_.notPlaying)));
    title_->setText("nothing is playing");
    title_->setVisible(true);
  }
}

}  // namespace neobar
