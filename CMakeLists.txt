cmake_minimum_required(VERSION 3.10)
project(CyRkFaceDemo)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(ROCKIT_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/rockit)

if (WIN32)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

elseif (UNIX)
    find_package(PkgConfig REQUIRED)
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()

link_directories(
        ${ROCKIT_ROOT}/lib
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/ui/cyu_main.cpp  include/ui/cyu_main.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${ROCKIT_ROOT}/include
)


if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Core
            Qt5::Widgets
    )
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            Qt5::Core
            Qt5::Widgets
            rockit
    )
else ()
    message(FATAL_ERROR "Unsupported operating system")
endif ()