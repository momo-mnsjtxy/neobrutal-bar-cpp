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

find_package(Qt6 REQUIRED COMPONENTS Widgets Network DBus)
find_package(X11 REQUIRED)

add_executable(neobrutal-bar
  src/main.cpp
  src/Theme.cpp
  src/Config.cpp
  src/Icons.cpp
  src/providers/Provider.cpp
  src/providers/SystemInfo.cpp
  src/providers/WeatherProvider.cpp
  src/providers/WorkspaceProvider.cpp
  src/providers/MediaProvider.cpp
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

target_include_directories(neobrutal-bar PRIVATE src ${X11_INCLUDE_DIR})

target_link_libraries(neobrutal-bar PRIVATE
  Qt6::Widgets
  Qt6::Network
  Qt6::DBus
  ${X11_LIBRARIES}
)

install(TARGETS neobrutal-bar RUNTIME DESTINATION bin)
