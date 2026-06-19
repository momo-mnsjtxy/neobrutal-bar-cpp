#include "providers/SystemInfo.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
// iphlpapi/wlanapi must come after the Winsock + Windows base headers.
#include <iphlpapi.h>
#include <ipifcons.h>  // IF_TYPE_* interface type constants
#include <wlanapi.h>

#include <vector>

namespace neobar {

namespace {

unsigned long long fileTimeToULL(const FILETIME &ft) {
  ULARGE_INTEGER u;
  u.LowPart = ft.dwLowDateTime;
  u.HighPart = ft.dwHighDateTime;
  return u.QuadPart;
}

// Fills wifi SSID/signal from the first connected WLAN interface, if any.
bool queryWifi(NetworkInfo *info) {
  HANDLE handle = nullptr;
  DWORD negotiated = 0;
  if (WlanOpenHandle(2, nullptr, &negotiated, &handle) != ERROR_SUCCESS)
    return false;

  bool found = false;
  PWLAN_INTERFACE_INFO_LIST list = nullptr;
  if (WlanEnumInterfaces(handle, nullptr, &list) == ERROR_SUCCESS && list) {
    for (DWORD i = 0; i < list->dwNumberOfItems && !found; ++i) {
      const WLAN_INTERFACE_INFO &itf = list->InterfaceInfo[i];
      if (itf.isState != wlan_interface_state_connected) continue;

      PWLAN_CONNECTION_ATTRIBUTES attr = nullptr;
      DWORD size = 0;
      WLAN_OPCODE_VALUE_TYPE opType;
      if (WlanQueryInterface(handle, &itf.InterfaceGuid,
                             wlan_intf_opcode_current_connection, nullptr, &size,
                             reinterpret_cast<PVOID *>(&attr),
                             &opType) == ERROR_SUCCESS &&
          attr) {
        info->type = NetType::Wifi;
        const DOT11_SSID &ssid = attr->wlanAssociationAttributes.dot11Ssid;
        info->ssid = QString::fromUtf8(
            reinterpret_cast<const char *>(ssid.ucSSID),
            static_cast<int>(ssid.uSSIDLength));
        info->signalStrength = static_cast<int>(
            attr->wlanAssociationAttributes.wlanSignalQuality);  // 0..100
        found = true;
      }
      if (attr) WlanFreeMemory(attr);
    }
  }
  if (list) WlanFreeMemory(list);
  WlanCloseHandle(handle, nullptr);
  return found;
}

// Detects the active interface backing the default route via its gateway.
void queryAdapter(NetworkInfo *info) {
  ULONG size = 15000;
  std::vector<BYTE> buffer(size);
  const ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                      GAA_FLAG_SKIP_DNS_SERVER;

  auto *addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
  ULONG ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, addrs, &size);
  if (ret == ERROR_BUFFER_OVERFLOW) {
    buffer.resize(size);
    addrs = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
    ret = GetAdaptersAddresses(AF_UNSPEC, flags, nullptr, addrs, &size);
  }
  if (ret != NO_ERROR) return;

  for (auto *a = addrs; a; a = a->Next) {
    if (a->OperStatus != IfOperStatusUp) continue;
    if (a->IfType == IF_TYPE_SOFTWARE_LOOPBACK) continue;
    if (a->IfType == IF_TYPE_TUNNEL) continue;
    if (!a->FirstGatewayAddress) continue;  // no default gateway -> skip

    info->type =
        a->IfType == IF_TYPE_IEEE80211 ? NetType::Wifi : NetType::Ethernet;
    return;
  }
}

}  // namespace

void CpuProvider::refresh() {
  FILETIME idleFt, kernelFt, userFt;
  if (!GetSystemTimes(&idleFt, &kernelFt, &userFt)) return;

  // kernelFt already includes idle time, so total = kernel + user.
  const unsigned long long idle = fileTimeToULL(idleFt);
  const unsigned long long total =
      fileTimeToULL(kernelFt) + fileTimeToULL(userFt);

  const unsigned long long dTotal = total - lastTotal_;
  const unsigned long long dIdle = idle - lastIdle_;
  if (lastTotal_ != 0 && dTotal > 0)
    usage_ = 100.0 * static_cast<double>(dTotal - dIdle) / dTotal;
  lastTotal_ = total;
  lastIdle_ = idle;
}

void MemoryProvider::refresh() {
  MEMORYSTATUSEX status;
  status.dwLength = sizeof(status);
  if (GlobalMemoryStatusEx(&status))
    usage_ = static_cast<double>(status.dwMemoryLoad);
}

void BatteryProvider::refresh() {
  present_ = false;
  SYSTEM_POWER_STATUS ps;
  if (!GetSystemPowerStatus(&ps)) return;
  if (ps.BatteryFlag & 128) return;          // no system battery
  if (ps.BatteryLifePercent == 255) return;  // unknown charge
  present_ = true;
  charge_ = ps.BatteryLifePercent;
  charging_ = (ps.ACLineStatus == 1) || (ps.BatteryFlag & 8);  // online/charging
}

void NetworkProvider::refresh() {
  NetworkInfo info;
  if (!queryWifi(&info)) queryAdapter(&info);
  info_ = info;
}

}  // namespace neobar
