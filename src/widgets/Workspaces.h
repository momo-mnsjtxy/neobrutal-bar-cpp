#pragma once

#include <QWidget>

#include "Theme.h"
#include "Types.h"

class QHBoxLayout;

namespace neobar {

// The centre group (Workspaces.svelte): a button per workspace with a focus
// indicator, followed by the process icons of the windows on the active
// workspace.
class Workspaces : public QWidget {
  Q_OBJECT
 public:
  explicit Workspaces(const Theme &theme, QWidget *parent = nullptr);

  void setData(const WorkspaceData &data);

 signals:
  void switchRequested(int index);

 private:
  void clear();

  Theme theme_;
  QHBoxLayout *layout_;
};

}  // namespace neobar
