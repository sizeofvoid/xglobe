cmake_minimum_required(VERSION 3.16.0)

# Version setup
set(XGLOBE_VERSION_MAJOR "0")
set(XGLOBE_VERSION_MINOR "6")
set(XGLOBE_VERSION_PATCH "0")
set(XGLOBE_VERSION "${XGLOBE_VERSION_MAJOR}.${XGLOBE_VERSION_MINOR}.${XGLOBE_VERSION_PATCH}")

project(xglobe
        VERSION ${XGLOBE_VERSION}
        DESCRIPTION "XGlobe is the further development of Thorsten Scheuermanns xglobe 0.5 release."
        HOMEPAGE_URL "https://github.com/sizeofvoid/xglobe"
        LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo Profile"
            FORCE)
endif()

include(GNUInstallDirs)

option(SET_DEFAULT_MAP "Set default map file." "map.png")
option(SET_DEFAULT_MAP_NIGHT "Set default night map file.")
option(SET_DEFAULT_MAP_BACK "Set default back map file.")
option(SET_DEFAULT_MARKER_FILE "Set default marker file." "xglobe-markers")

option(ENABLE_INSTALL_OPENBSD_MARKERS "Install OpenBSD markers" OFF)
option(ENABLE_INSTALL_GETCLOUDMAP "Install old xglobe 0.5 getcloudmap script" OFF)

set(INSTALL_XGLOBE_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/xglobe")

find_package(Qt5 COMPONENTS Core Gui Widgets X11Extras REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

find_program(XWALLPAPER_BIN xwallpaper)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Always use '-fPIC'/'-fPIE' option.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCE
    src/main.cpp
    src/compute.cpp
    src/command_line_parser.cpp
    src/geo_coordinate.cpp
    src/desktopwidget.cpp
    src/earthapp.cpp
    src/file.cpp
    src/markerlist.cpp
    src/moonpos.cpp
    src/random.cpp
    src/renderer.cpp
    src/stars.cpp
    src/sunpos.cpp)

add_executable(xglobe ${SOURCE})

target_link_libraries(xglobe PUBLIC X11
                                    Threads::Threads
                                    Qt5::Core
                                    Qt5::Gui
                                    Qt5::Widgets
                                    Qt5::X11Extras)

target_link_libraries(xglobe PRIVATE ${X11_LIBRARIES})
target_include_directories(xglobe PRIVATE ${X11_INCLUDE_DIR})

target_compile_features(xglobe PRIVATE cxx_std_17)
target_compile_options(xglobe PRIVATE "-Wall")
target_compile_options(xglobe PRIVATE "-pipe")
target_compile_options(xglobe PRIVATE "-fexceptions")

if (NOT XWALLPAPER_BIN)
    message(FATAL_ERROR "Missing xwallpaper, please install xwallpaper")
else()
    target_compile_definitions(xglobe PRIVATE XWALLPAPER_BIN="${XWALLPAPER_BIN}")
endif()

if (SET_DEFAULT_MAP)
    message(STATUS "Using default map ${SET_DEFAULT_MAP}")
    target_compile_definitions(xglobe PRIVATE DEFAULT_MAP="${SET_DEFAULT_MAP}")
endif()

if (SET_DEFAULT_MAP_NIGHT)
    message(STATUS "Using default night map ${SET_DEFAULT_MAP_NIGHT}")
    target_compile_definitions(xglobe PRIVATE DEFAULT_MAP_NIGHT="${SET_DEFAULT_MAP_NIGHT}")
endif()

if (SET_DEFAULT_MAP_BACK)
    message(STATUS "Using default back map ${SET_DEFAULT_MAP_BACK}")
    target_compile_definitions(xglobe PRIVATE DEFAULT_MAP_BACK="${SET_DEFAULT_MAP_BACK}")
endif()

if (SET_DEFAULT_MARKER_FILE)
    message(STATUS "Using default marker file ${SET_DEFAULT_MARKER_FILE}")
    target_compile_definitions(xglobe PRIVATE DEFAULT_MARKER_FILE="${SET_DEFAULT_MARKER_FILE}")
endif()

target_compile_definitions(xglobe PRIVATE XGLOBE_DATA_DIR="${INSTALL_XGLOBE_DATA_DIR}")
message(STATUS "Install data files in ${INSTALL_XGLOBE_DATA_DIR}")

# install
install(TARGETS xglobe
        RUNTIME DESTINATION bin COMPONENT Runtime)

install(FILES README.md README.xglobe-0.5 README.xglobe-0.5.maps
        TYPE DOC)

install(FILES xglobe-markers
        DESTINATION "${INSTALL_XGLOBE_DATA_DIR}")

if (ENABLE_INSTALL_GETCLOUDMAP)
    install(SCRIPT getcloudmap.sh
            README getcloudmap
            RUNTIME DESTINATION bin
            COMPONENT Runtime)
endif()

if (ENABLE_INSTALL_OPENBSD_MARKERS)
    install(FILES OpenBSD-markers
            DESTINATION "${INSTALL_XGLOBE_DATA_DIR}")
endif()
