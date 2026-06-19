#include "providers/WorkspaceProvider.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QUrl>

namespace neobar {

namespace {

// GlazeWM windows can be nested inside split containers, so collect them
// recursively. Each window node carries its process name, title and focus.
void collectWindows(const QJsonValue &node, QVector<WindowInfo> *out) {
  if (node.isArray()) {
    for (const QJsonValue &child : node.toArray()) collectWindows(child, out);
    return;
  }
  if (!node.isObject()) return;
  const QJsonObject obj = node.toObject();

  if (obj.value("type").toString() == "window") {
    WindowInfo info;
    info.processName = obj.value("processName").toString().toLower();
    info.title = obj.value("title").toString();
    info.hasFocus = obj.value("hasFocus").toBool();

    const QJsonValue state = obj.value("state");
    const QString stateType =
        state.isObject() ? state.toObject().value("type").toString()
                         : state.toString();
    info.minimized = stateType.compare("minimized", Qt::CaseInsensitive) == 0;
    out->push_back(info);
  }

  collectWindows(obj.value("children"), out);
}

}  // namespace

WorkspaceProvider::WorkspaceProvider(QObject *parent) : Provider(parent) {
  connect(&socket_, &QWebSocket::textMessageReceived, this,
          &WorkspaceProvider::onTextMessage);
  connect(&socket_, &QWebSocket::connected, this, [this] {
    connected_ = true;
    socket_.sendTextMessage(
        "sub --events workspace_activated workspace_deactivated "
        "focus_changed window_managed window_unmanaged");
    query();
  });
  connect(&socket_, &QWebSocket::disconnected, this, [this] {
    connected_ = false;
    data_ = {};
  });
  connect(&socket_, &QWebSocket::errorOccurred, this,
          [this](QAbstractSocket::SocketError) { connected_ = false; });
}

WorkspaceProvider::~WorkspaceProvider() { socket_.close(); }

bool WorkspaceProvider::available() const { return connected_; }

void WorkspaceProvider::ensureConnected() {
  const QAbstractSocket::SocketState state = socket_.state();
  if (state == QAbstractSocket::ConnectingState ||
      state == QAbstractSocket::ConnectedState)
    return;
  socket_.open(QUrl(QString("ws://localhost:%1").arg(port_)));
}

void WorkspaceProvider::query() {
  if (socket_.state() == QAbstractSocket::ConnectedState)
    socket_.sendTextMessage("query workspaces");
}

void WorkspaceProvider::refresh() {
  // Reconnect if needed, then re-query. Replies arrive asynchronously and
  // update data_; the polling tick re-reads it via data().
  ensureConnected();
  query();
}

void WorkspaceProvider::onTextMessage(const QString &message) {
  const QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  if (!doc.isObject()) return;
  const QJsonObject root = doc.toObject();
  const QString msgType = root.value("messageType").toString();

  // Subscribed events just nudge us to re-read the workspace tree.
  if (msgType == "event") {
    query();
    return;
  }
  if (msgType != "client_response") return;
  if (root.value("clientMessage").toString() != "query workspaces") return;

  const QJsonObject data = root.value("data").toObject();
  const QJsonArray wss = data.value("workspaces").toArray();

  WorkspaceData parsed;
  int idx = 0;
  for (const QJsonValue &v : wss) {
    const QJsonObject ws = v.toObject();
    WorkspaceInfo info;
    info.index = idx++;
    info.name = ws.value("name").toString(QString::number(info.index + 1));
    info.hasFocus = ws.value("hasFocus").toBool();
    parsed.workspaces.push_back(info);

    if (info.hasFocus)
      collectWindows(ws.value("children"), &parsed.focusedWindows);
  }

  data_ = parsed;
}

void WorkspaceProvider::switchToDesktop(int index) {
  if (index < 0 || index >= data_.workspaces.size()) return;
  if (socket_.state() != QAbstractSocket::ConnectedState) return;
  socket_.sendTextMessage(
      QString("command focus --workspace %1").arg(data_.workspaces.at(index).name));
}

}  // namespace neobar
