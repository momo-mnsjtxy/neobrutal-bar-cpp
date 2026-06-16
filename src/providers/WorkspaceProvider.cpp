#include "providers/WorkspaceProvider.h"

// X11 headers come last: they #define None/Bool/Status as macros which would
// otherwise clobber identifiers in the project/Qt headers above.
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace neobar {

namespace {

Atom atom(Display *dpy, const char *name) {
  return XInternAtom(dpy, name, False);
}

// Reads a property as a raw buffer. Caller must XFree() *data when non-null.
bool getProp(Display *dpy, Window win, Atom prop, Atom type, unsigned char **data,
             unsigned long *nitems) {
  Atom actualType = 0;
  int actualFormat = 0;
  unsigned long bytesAfter = 0;
  *data = nullptr;
  *nitems = 0;
  const int rc =
      XGetWindowProperty(dpy, win, prop, 0, 1 << 20, False, type, &actualType,
                         &actualFormat, nitems, &bytesAfter, data);
  return rc == Success && *data != nullptr;
}

long getCardinal(Display *dpy, Window win, Atom prop, long fallback) {
  unsigned char *data = nullptr;
  unsigned long n = 0;
  long value = fallback;
  if (getProp(dpy, win, prop, XA_CARDINAL, &data, &n) && n >= 1)
    value = *reinterpret_cast<long *>(data);
  if (data) XFree(data);
  return value;
}

QVector<Window> getWindows(Display *dpy, Window root, Atom prop) {
  QVector<Window> out;
  unsigned char *data = nullptr;
  unsigned long n = 0;
  if (getProp(dpy, root, prop, XA_WINDOW, &data, &n)) {
    auto *wins = reinterpret_cast<Window *>(data);
    for (unsigned long i = 0; i < n; ++i) out.push_back(wins[i]);
  }
  if (data) XFree(data);
  return out;
}

QString getUtf8(Display *dpy, Window win, Atom prop, Atom utf8) {
  unsigned char *data = nullptr;
  unsigned long n = 0;
  QString out;
  if (getProp(dpy, win, prop, utf8, &data, &n) && data)
    out = QString::fromUtf8(reinterpret_cast<char *>(data));
  if (data) XFree(data);
  return out;
}

QStringList getUtf8List(Display *dpy, Window win, Atom prop, Atom utf8) {
  unsigned char *data = nullptr;
  unsigned long n = 0;
  QStringList out;
  if (getProp(dpy, win, prop, utf8, &data, &n) && data) {
    // Null-separated UTF-8 strings.
    const char *p = reinterpret_cast<char *>(data);
    const char *end = p + n;
    while (p < end) {
      out << QString::fromUtf8(p);
      p += qstrlen(p) + 1;
    }
  }
  if (data) XFree(data);
  return out;
}

QString windowProcessName(Display *dpy, Window win) {
  XClassHint hint;
  if (XGetClassHint(dpy, win, &hint)) {
    QString cls = QString::fromUtf8(hint.res_class).toLower();
    if (hint.res_name) XFree(hint.res_name);
    if (hint.res_class) XFree(hint.res_class);
    return cls;
  }
  return {};
}

bool windowMinimized(Display *dpy, Window win, Atom stateAtom, Atom hiddenAtom) {
  unsigned char *data = nullptr;
  unsigned long n = 0;
  bool hidden = false;
  if (getProp(dpy, win, stateAtom, XA_ATOM, &data, &n)) {
    auto *atoms = reinterpret_cast<Atom *>(data);
    for (unsigned long i = 0; i < n; ++i)
      if (atoms[i] == hiddenAtom) hidden = true;
  }
  if (data) XFree(data);
  return hidden;
}

}  // namespace

WorkspaceProvider::WorkspaceProvider(QObject *parent) : Provider(parent) {
  display_ = XOpenDisplay(nullptr);
}

WorkspaceProvider::~WorkspaceProvider() {
  if (display_) XCloseDisplay(static_cast<Display *>(display_));
}

void WorkspaceProvider::switchToDesktop(int index) {
  if (!display_) return;
  auto *dpy = static_cast<Display *>(display_);
  Window root = DefaultRootWindow(dpy);

  XEvent ev{};
  ev.xclient.type = ClientMessage;
  ev.xclient.message_type = atom(dpy, "_NET_CURRENT_DESKTOP");
  ev.xclient.display = dpy;
  ev.xclient.window = root;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = index;
  ev.xclient.data.l[1] = CurrentTime;
  XSendEvent(dpy, root, False, SubstructureNotifyMask | SubstructureRedirectMask,
             &ev);
  XFlush(dpy);
}

void WorkspaceProvider::refresh() {
  if (!display_) {
    data_ = {};
    return;
  }
  auto *dpy = static_cast<Display *>(display_);
  Window root = DefaultRootWindow(dpy);

  const Atom utf8 = atom(dpy, "UTF8_STRING");
  const Atom netName = atom(dpy, "_NET_WM_NAME");
  const Atom netDesktop = atom(dpy, "_NET_WM_DESKTOP");
  const Atom netState = atom(dpy, "_NET_WM_STATE");
  const Atom hidden = atom(dpy, "_NET_WM_STATE_HIDDEN");

  const long count =
      getCardinal(dpy, root, atom(dpy, "_NET_NUMBER_OF_DESKTOPS"), 0);
  const long current =
      getCardinal(dpy, root, atom(dpy, "_NET_CURRENT_DESKTOP"), 0);
  const QStringList names =
      getUtf8List(dpy, root, atom(dpy, "_NET_DESKTOP_NAMES"), utf8);

  WorkspaceData data;
  for (long i = 0; i < count; ++i) {
    WorkspaceInfo ws;
    ws.index = static_cast<int>(i);
    ws.name = i < names.size() ? names.at(static_cast<int>(i))
                               : QString::number(i + 1);
    ws.hasFocus = (i == current);
    data.workspaces.push_back(ws);
  }

  Window active = static_cast<Window>(
      getCardinal(dpy, root, atom(dpy, "_NET_ACTIVE_WINDOW"), 0));

  const QVector<Window> clients =
      getWindows(dpy, root, atom(dpy, "_NET_CLIENT_LIST"));
  for (Window w : clients) {
    const long desk = getCardinal(dpy, w, netDesktop, -1);
    if (desk != current && desk != -1) continue;  // -1 == sticky/all desktops
    WindowInfo info;
    info.processName = windowProcessName(dpy, w);
    info.title = getUtf8(dpy, w, netName, utf8);
    info.hasFocus = (w == active);
    info.minimized = windowMinimized(dpy, w, netState, hidden);
    data.focusedWindows.push_back(info);
  }

  data_ = data;
}

}  // namespace neobar
