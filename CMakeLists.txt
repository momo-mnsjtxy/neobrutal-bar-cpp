cmake_minimum_required(VERSION 3.16)

project(neobrutal-bar
  VERSION 0.1.0
  DESCRIPTION "A neobrutalist desktop status bar (C++/Qt port of neobrutal-zebar)"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(neobar_sources
  src/main.cpp
  src/Theme.cpp
  src/Config.cpp
  src/Icons.cpp
  # Headers whose implementation is split per-platform are listed so AUTOMOC
  # still processes their Q_OBJECT classes (it only auto-pairs headers with a
  # same-named .cpp, which these no longer have).
  src/providers/SystemInfo.h
  src/providers/WorkspaceProvider.h
  src/providers/MediaProvider.h
  src/providers/Provider.cpp
  src/providers/WeatherProvider.cpp
  src/widgets/UiHelpers.cpp
  src/widgets/AppIconMap.cpp
  src/widgets/Meter.cpp
  src/widgets/NeoButton.cpp
  src/widgets/NeoFrame.cpp
  src/widgets/Workspaces.cpp
  src/widgets/NowPlaying.cpp
  src/widgets/Bar.cpp
  resources.qrc
)

if(WIN32)
  list(APPEND neobar_sources
    src/providers/SystemInfo_win.cpp
    src/providers/WorkspaceProvider_win.cpp
    src/providers/MediaProvider_win.cpp
  )
else()
  list(APPEND neobar_sources
    src/providers/SystemInfo_linux.cpp
    src/providers/WorkspaceProvider_linux.cpp
    src/providers/MediaProvider_linux.cpp
  )
endif()

add_executable(neobrutal-bar ${neobar_sources})

target_include_directories(neobrutal-bar PRIVATE src)

if(WIN32)
  # Windows: GlazeWM workspaces speak over a WebSocket; media uses WinRT.
  find_package(Qt6 REQUIRED COMPONENTS Widgets Network WebSockets)
  target_link_libraries(neobrutal-bar PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    iphlpapi   # GetAdaptersAddresses
    wlanapi    # WlanQueryInterface (wifi SSID / signal)
    windowsapp # C++/WinRT (Windows.Media.Control)
  )
  set_target_properties(neobrutal-bar PROPERTIES WIN32_EXECUTABLE ON)
else()
  # Linux: workspaces via EWMH/X11, media via MPRIS over D-Bus.
  find_package(Qt6 REQUIRED COMPONENTS Widgets Network DBus)
  find_package(X11 REQUIRED)
  target_include_directories(neobrutal-bar PRIVATE ${X11_INCLUDE_DIR})
  target_link_libraries(neobrutal-bar PRIVATE
    Qt6::Widgets
    Qt6::Network
    Qt6::DBus
    ${X11_LIBRARIES}
  )
endif()

install(TARGETS neobrutal-bar RUNTIME DESTINATION bin)
