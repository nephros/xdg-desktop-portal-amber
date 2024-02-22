set(SRCS
    screenshot.cpp
    desktopportal.cpp
    main.cpp
)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

add_executable(xdg-desktop-portal-amber ${SRCS})

set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD_REQUIRED on)

target_link_libraries(xdg-desktop-portal-amber
    Qt::Core
    Qt::DBus
)

install(TARGETS xdg-desktop-portal-amber DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
