#pragma once

#include "Types.h"
#include "providers/Provider.h"

#ifdef Q_OS_WIN
#include <QAbstractSocket>
#include <QWebSocket>
#endif

namespace neobar {

// Workspaces and their windows. On Linux this reads an EWMH-compliant X11
// window manager; on Windows it talks to GlazeWM over its WebSocket IPC (the
// same window manager the upstream neobrutal-zebar targets). Exposes the list
// of workspaces and the windows on the active one, and can switch workspaces.
class WorkspaceProvider : public Provider {
  Q_OBJECT
 public:
  explicit WorkspaceProvider(QObject *parent = nullptr);
  ~WorkspaceProvider() override;

  bool available() const;
  WorkspaceData data() const { return data_; }

  // Asks the window manager to activate the workspace at `index` (0-based).
  void switchToDesktop(int index);

 protected:
  void refresh() override;

 private:
  WorkspaceData data_;

#ifdef Q_OS_WIN
  void ensureConnected();
  void query();
  void onTextMessage(const QString &message);

  QWebSocket socket_;
  quint16 port_ = 6123;
  bool connected_ = false;
#else
  void *display_ = nullptr;  // Xlib Display* (kept opaque to avoid X11 in hdr)
#endif
};

}  // namespace neobar
