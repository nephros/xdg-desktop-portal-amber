configure_file(org.freedesktop.impl.portal.desktop.amber.desktop.in org.freedesktop.impl.portal.desktop.amber.desktop @ONLY)
configure_file(org.freedesktop.impl.portal.desktop.amber.service.in org.freedesktop.impl.portal.desktop.amber.service @ONLY)
configure_file(xdg-desktop-portal-amber.service.in xdg-desktop-portal-amber.service @ONLY)
configure_file(xdg-desktop-portal-pre.service.in xdg-desktop-portal-pre.service @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.amber.service DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/dbus-1/services")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.amber.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xdg-desktop-portal-amber.service  DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/user")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xdg-desktop-portal-pre.service    DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/user")

install(FILES amber.portal DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal/portals/")
install(FILES xdg-desktop.conf DESTINATION "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/environment/amber/")
install(FILES lipstick-portals.conf DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal")

add_subdirectory(sailjail)
