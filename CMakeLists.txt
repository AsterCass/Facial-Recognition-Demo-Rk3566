cmake_minimum_required(VERSION 3.10)
project(CyRkFaceDemo)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (WIN32)
    set(GSTREAMER_INCLUDE_DIRS 3rdparty/include)
    # Qt
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

elseif (UNIX)
    # Gstreamer
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-video-1.0 gstreamer-app-1.0)

    # Qt
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/ui/cyu_main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${GSTREAMER_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Core
        Qt5::Widgets
        ${GSTREAMER_LIBRARIES}
)

message(STATUS "CMake sysroot: ${CMAKE_SYSROOT}")
message(STATUS "GStreamer include dirs: ${GSTREAMER_INCLUDE_DIRS}")
message(STATUS "GStreamer libraries: ${GSTREAMER_LIBRARIES}")