set(SRCS
    app.cpp
)
add_executable(xdg-desktop-portal-amber-ui ${SRCS})

set_property(TARGET xdg-desktop-portal-amber-ui PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-amber-ui PROPERTY CXX_STANDARD_REQUIRED on)

include(FindPkgConfig)
pkg_search_module(SailfishApp REQUIRED sailfishapp)

include_directories(
    ${SailfishApp_INCLUDE_DIRS}
)

target_link_libraries(xdg-desktop-portal-amber-ui
    SailfishApp_LIBRARIES
)

install(TARGETS xdg-desktop-portal-amber-ui DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

install(FILES FilePickerDialog.qml DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/lipstick-xdg-desktop-portal-amber")
install(FILES xdg-desktop-portal-amber-ui.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
