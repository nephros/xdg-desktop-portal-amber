TEMPLATE = aux

CONFIG += sailfish-qdoc-template
SAILFISH_QDOC.project = xdg-desktop-portal-sailfish
SAILFISH_QDOC.config = xdg-desktop-portal-sailfish.qdocconf
SAILFISH_QDOC.style = offline
SAILFISH_QDOC.path = /usr/share/doc/xdg-desktop-portal-sailfish

OTHER_FILES += $$PWD/index.qdoc

