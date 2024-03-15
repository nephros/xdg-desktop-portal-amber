set(ICONSRC
  icon-m-xdg-desktop-portal
  icon-m-xdg-desktop-portal-dark
  )
set(SIZES
  86
  108
  128
  172
  256
  512
  1024
  )
foreach(SIZE ${SIZES})
  foreach(ICON ${ICONSRC})
    add_custom_command(
      OUTPUT ${PROJECT_BINARY_DIR}/icons/${SIZE}x${SIZE}/apps/${ICON}.png
      COMMAND sailfish_svg2png
      ARGS -z 1.0 -s 1 1 1 ${SIZE} 1 1 1 svgs/ ${PROJECT_BINARY_DIR}/icons/${SIZE}x${SIZE}/apps
     )
    list(APPEND PNGS ${PROJECT_BINARY_DIR}/icons/${SIZE}x${SIZE}/apps/${ICON}.png)
    install(FILES ${PROJECT_BINARY_DIR}/icons/${SIZE}x${SIZE}/apps/${ICON}.png DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/${SIZE}x${SIZE}/apps")
  endforeach()
endforeach()

add_custom_target (Icons ALL DEPENDS ${PNGS})
