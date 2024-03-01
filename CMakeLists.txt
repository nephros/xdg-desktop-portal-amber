cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "1.0.0")

project(xdg-desktop-portal-lxqt VERSION ${PROJECT_VERSION})

set(QT_MIN_VERSION "5.6.0")

set(CMAKE_AUTOMOC on)

include(FeatureSummary)
include(GNUInstallDirs)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    DBus
)

add_subdirectory(data)
add_subdirectory(src)
add_subdirectory(qml)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
