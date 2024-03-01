set(SRCS
    desktopportal.cpp
    access.cpp
    filechooser.cpp
    permissionstore.cpp
    screenshot.cpp
    wallpaper.cpp
    main.cpp
)

include(FindPkgConfig)
pkg_search_module(MDM REQUIRED sailfishmdm)
pkg_search_module(SFOSPolicy REQUIRED sailfishpolicy)

include_directories(
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${MDM_INCLUDE_DIRS}
	${SFOSPolicy_INCLUDE_DIRS}
)

add_executable(xdg-desktop-portal-amber ${SRCS})

set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD 14)
set_property(TARGET xdg-desktop-portal-amber PROPERTY CXX_STANDARD_REQUIRED on)

target_link_libraries(xdg-desktop-portal-amber
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    ${MDM_LIBRARIES}
	${SFOSPolicy_LIBRARIES}
)

install(TARGETS xdg-desktop-portal-amber DESTINATION ${CMAKE_INSTALL_FULL_LIBEXECDIR})
