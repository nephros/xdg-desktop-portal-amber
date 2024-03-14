set(SRCS
    app.cpp
)
add_executable(xdg-desktop-portal-sailfish-ui ${SRCS})

set_property(TARGET xdg-desktop-portal-sailfish-ui PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-sailfish-ui PROPERTY CXX_STANDARD_REQUIRED on)

include(FindPkgConfig)
pkg_search_module(SailfishApp REQUIRED sailfishapp)

find_package(Qt5 COMPONENTS Core Quick Gui DBus REQUIRED)

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5DBus_INCLUDE_DIRS}
    ${Qt5_INCLUDE_DIRS}
    ${SailfishApp_INCLUDE_DIRS}
)

target_link_libraries(xdg-desktop-portal-sailfish-ui
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::DBus
    ${SailfishApp_LIBRARIES}
)

install(TARGETS xdg-desktop-portal-sailfish-ui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

install(FILES FilePickerDialog.qml DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal-sailfish-ui/qml")
install(FILES xdg-desktop-portal-sailfish-ui.qml DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal-sailfish-ui/qml")
install(FILES xdg-desktop-portal-sailfish-ui.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
install(FILES org.freedesktop.impl.portal.desktop.sailfish.ui.service DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/dbus-1/services")
