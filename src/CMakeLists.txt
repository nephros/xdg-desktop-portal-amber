set(SRCS
    desktopportal.cpp
    screenshot.cpp
    main.cpp
)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
)

add_executable(xdg-desktop-portal-amber ${SRCS})

set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD_REQUIRED on)

target_link_libraries(xdg-desktop-portal-amber
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
)

install(TARGETS xdg-desktop-portal-amber DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
