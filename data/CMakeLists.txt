configure_file(org.freedesktop.impl.portal.desktop.amber.desktop.in org.freedesktop.impl.portal.desktop.amber.desktop @ONLY)
configure_file(org.freedesktop.impl.portal.desktop.amber.service.in org.freedesktop.impl.portal.desktop.amber.service @ONLY)

install(FILES amber.portal DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/xdg-desktop-portal/portals")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.amber.service DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/dbus-1/services")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.impl.portal.desktop.amber.desktop DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")

install(FILES 51-jolla-ui-xdg.conf DESTINATION "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/lib/environment/nemo/")
