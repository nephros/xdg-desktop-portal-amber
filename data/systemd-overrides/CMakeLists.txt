set(SD_OVERRIDES_USER
    xdg-desktop-portal.service.d
    xdg-document-portal.d
    xdg-permission-store.d
)
install(DIRECTORY ${SD_OVERRIDES_USER} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/systemd/user" FILES_MATCHING PATTERN "*.conf")
