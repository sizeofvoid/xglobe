cmake_minimum_required(VERSION 3.7.0)

project(xglobe)

# Version setup
set(XGLOBE_VERSION_MAJOR "0")
set(XGLOBE_VERSION_MINOR "6")
set(XGLOBE_VERSION_PATCH "0")
set(XGLOBE_VERSION "${XGLOBE_VERSION_MAJOR}.${XGLOBE_VERSION_MINOR}.${XGLOBE_VERSION_PATCH}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo Profile"
            FORCE)
endif()

set(QT_COMPONENTS Core Gui Widgets X11Extras)
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

find_program(XWALLPAPER_BIN xwallpaper)
if (NOT XWALLPAPER_BIN)
    message(FATAL_ERROR "Missing xwallpaper, please install xwallpaper")
else()
    add_compile_definitions(XWALLPAPER_BIN="${XWALLPAPER_BIN}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_REQUIRED_FLAGS -std=c++17)

set(SOURCE
    main.cpp
    compute.cpp
    command_line_parser.cpp
    desktopwidget.cpp
    earthapp.cpp
    file.cpp
    markerlist.cpp
    moonpos.cpp
    random.cpp
    renderer.cpp
    stars.cpp
    sunpos.cpp)

add_definitions(-DVERSION="${XGLOBE_VERSION}")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -funroll-loops" CACHE STRING "Set C++ Compiler Flags" FORCE)

add_executable(xglobe ${SOURCE})
target_link_libraries(xglobe PRIVATE X11 Threads::Threads Qt5::Core Qt5::Gui Qt5::Widgets Qt5::X11Extras)
