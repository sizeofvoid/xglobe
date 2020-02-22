cmake_minimum_required(VERSION 3.7.0)

project(xglobe)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo Profile"
            FORCE)
endif()

set(QT_COMPONENTS Core Gui Widgets)
find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
find_package(Threads REQUIRED)

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
    desktopwidget.cpp
    earthapp.cpp
    file.cpp
    markerlist.cpp
    moonpos.cpp
    random.cpp
    renderer.cpp
    stars.cpp
    sunpos.cpp)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -funroll-loops" CACHE STRING "Set C++ Compiler Flags" FORCE)

add_executable(xglobe ${SOURCE})
target_link_libraries(xglobe PRIVATE Threads::Threads Qt5::Core Qt5::Gui Qt5::Widgets)
