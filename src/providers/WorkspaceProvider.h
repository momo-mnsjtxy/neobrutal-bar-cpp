#pragma once

#include "Types.h"
#include "providers/Provider.h"

namespace neobar {

// Workspaces and their windows, read from an EWMH-compliant X11 window manager
// (the Linux analogue of the upstream GlazeWM provider). Exposes the list of
// virtual desktops and the windows on the active one, and can switch desktops.
class WorkspaceProvider : public Provider {
  Q_OBJECT
 public:
  explicit WorkspaceProvider(QObject *parent = nullptr);
  ~WorkspaceProvider() override;

  bool available() const { return display_ != nullptr; }
  WorkspaceData data() const { return data_; }

  // Asks the window manager to activate the desktop at `index` (0-based).
  void switchToDesktop(int index);

 protected:
  void refresh() override;

 private:
  void *display_ = nullptr;  // Xlib Display* (kept opaque to avoid X11 in hdr)
  WorkspaceData data_;
};

}  // namespace neobar
