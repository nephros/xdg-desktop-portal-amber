configure_file(org.freedesktop.impl.portal.desktop.sailfish.desktop.in org.freedesktop.impl.portal.desktop.sailfish.desktop @ONLY)
configure_file(org.freedesktop.impl.portal.desktop.sailfish.service.in org.freedesktop.impl.portal.desktop.sailfish.service @ONLY)
configure_file(xdg-desktop-portal-sailfish.service.in xdg-desktop-portal-sailfish.service @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.sailfish.service DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/dbus-1/services")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.sailfish.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xdg-desktop-portal-sailfish.service  DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/user")

install(FILES sailfish.portal DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal/portals/")
install(FILES xdg-desktop.conf DESTINATION "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/environment/sailfish/")
install(FILES lipstick-portals.conf DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal")

add_subdirectory(sailjail)
add_subdirectory(systemd-overrides)
