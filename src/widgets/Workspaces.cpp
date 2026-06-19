#include "widgets/Workspaces.h"

#include <QHBoxLayout>
#include <QLabel>

#include "widgets/AppIconMap.h"
#include "widgets/NeoButton.h"
#include "widgets/UiHelpers.h"

namespace neobar {

Workspaces::Workspaces(const Theme &theme, QWidget *parent)
    : QWidget(parent), theme_(theme) {
  layout_ = new QHBoxLayout(this);
  layout_->setContentsMargins(0, 0, 0, 0);
  layout_->setSpacing(8);  // tailwind gap-2
}

void Workspaces::clear() {
  while (QLayoutItem *item = layout_->takeAt(0)) {
    if (QWidget *w = item->widget()) w->deleteLater();
    delete item;
  }
}

void Workspaces::setData(const WorkspaceData &data) {
  clear();

  const int iconSize = theme_.fontSize;

  for (const WorkspaceInfo &ws : data.workspaces) {
    const QString icon = ws.hasFocus ? "ti-point-filled" : "ti-point";
    const QColor color = theme_.ws[ws.index % 5];
    auto *btn = new NeoButton(icon, color, theme_, iconSize, this);
    const int target = ws.index;
    connect(btn, &NeoButton::clicked, this,
            [this, target] { emit switchRequested(target); });
    layout_->addWidget(btn);
  }

  // Process icons for the windows on the active workspace.
  for (const WindowInfo &win : data.focusedWindows) {
    if (win.minimized) continue;
    const QString icon =
        AppIconMap::instance().iconFor(win.processName, win.title);
    if (icon.isEmpty()) continue;  // app is on the ignore list
    const QColor color = win.hasFocus ? theme_.focusedProcess : theme_.process;
    layout_->addWidget(makeIconLabel(icon, color, iconSize, this));
  }
}

}  // namespace neobar
