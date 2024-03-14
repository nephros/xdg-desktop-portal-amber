set(SRCS
    desktopportal.cpp
    access.cpp
    account.cpp
    email.cpp
    filechooser.cpp
    lockdown.cpp
    screenshot.cpp
    settings.cpp
    wallpaper.cpp
    xdg-desktop-portal-sailfish.cpp
)

include(FindPkgConfig)
pkg_search_module(MLITE REQUIRED mlite mlite5)
pkg_search_module(UMD REQUIRED sailfishusermanager)
pkg_search_module(MDM REQUIRED sailfishmdm)
include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${MLITE_INCLUDE_DIRS}
    ${UMD_INCLUDE_DIRS}
    ${MDM_INCLUDE_DIRS}
)

add_executable(xdg-desktop-portal-sailfish ${SRCS})

set_property(TARGET xdg-desktop-portal-sailfish PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-sailfish PROPERTY CXX_STANDARD_REQUIRED on)

target_link_libraries(xdg-desktop-portal-sailfish
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    ${MLITE_LIBRARIES}
    ${MDM_LIBRARIES}
)

install(TARGETS xdg-desktop-portal-sailfish DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
