#pragma once

#include <QWidget>

#include "Theme.h"

class QHBoxLayout;

namespace neobar {

// A neobrutalist "card" container (Group.svelte): rounded, bordered, filled
// with the base colour and casting a hard offset shadow. Children are laid out
// horizontally inside the face area.
class NeoFrame : public QWidget {
  Q_OBJECT
 public:
  explicit NeoFrame(const Theme &theme, QWidget *parent = nullptr);

  // The horizontal layout for the frame's contents.
  QHBoxLayout *body() const { return body_; }

 protected:
  void paintEvent(QPaintEvent *) override;

 private:
  Theme theme_;
  QHBoxLayout *body_;
};

}  // namespace neobar
