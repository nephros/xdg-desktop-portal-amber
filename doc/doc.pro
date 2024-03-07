TEMPLATE = aux

CONFIG += sailfish-qdoc-template
SAILFISH_QDOC.project = xdg-desktop-portal-amber
SAILFISH_QDOC.config = xdg-desktop-portal-amber.qdocconf
SAILFISH_QDOC.style = offline
SAILFISH_QDOC.path = /usr/share/doc/xdg-desktop-portal-amber

OTHER_FILES += $$PWD/index.qdoc

