/*
* SPDX-License-Identifier: Apache-2.0
*
* SPDX-FileCopyrightText: 2024~ Sailfish OS Community
* Authors:
*   Peter G. <sailfish@nephros.org>
*
*/
import QtQuick 2.2
import QtQuick.Window 2.0
import Nemo.FileManager 1.0
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import Sailfish.Pickers 1.0
import Nemo.Thumbnailer 1.0

SystemDialog {
    id: page

    /*  https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.impl.portal.FileChooser.html

        accept_label (s)
        The label for the accept button. Mnemonic underlines are allowed.

        modal (b)
        Whether to make the dialog modal. Default is yes.

        multiple (b)
        Whether to allow selection of multiple files. Default is no.

        directory (b)
        Whether to select for folders instead of files. Default is to select files.

        filters (a(sa(us)))
        A list of serialized file filters. See org.freedesktop.portal.FileChooser.OpenFile for details.

        current_filter ((sa(us)))
        Request that this filter be set by default at dialog creation. See org.freedesktop.portal.FileChooser.OpenFile for details.

        choices (a(ssa(ss)s))
        A list of serialized combo boxes. See org.freedesktop.portal.FileChooser.OpenFile for details.

        current_folder (ay)
        A suggested folder to open the files from. See org.freedesktop.portal.FileChooser.OpenFile for details.
        */
    property string title
    property var options
    property string selectedFile
    property bool windowVisible: visibility != Window.Hidden
                                 && visibility != Window.Minimized
    function init() {
        raise()
        show()
    }

    signal done(bool accepted, string selectedFile)
    property FileModel model: FileModel {
        id: fileModel
        active: page.status === PageStatus.Active
        path: StandardPaths.home
        includeDirectories: true
        sortBy: FileModel.SortByName
        directorySort: FileModel.SortDirectoriesBeforeFiles

        // Include parent directory even if not shown, so we can differentiate empty from inaccessible
        includeParentDirectory: true
    }
    Rectangle {
        width: page.width
        height: content.height
        color: Theme.overlayBackgroundColor

        Column {
            id: content
            width: parent.width

            SystemDialogHeader {
                id: header
                //% "Select location"
                //    title: qsTrId("components_pickers-he-select_location")
                //% "Select document"
                title: page.title ? page.title : qsTrId("components_pickers-he-select_document")

            }
            SilicaListView { id: view
                clip: true
                height: page.height - header.height - buttons.height
                width: parent.width
                model: page.model
                delegate: ListItem { id: litem
                    property bool selected: (view.currentIndex === index)
                    highlighted: down || menuOpen || selected
                    onClicked: {
                        if (isDir) {
                            page.model.path = absolutePath
                        } else {
                            view.currentIndex = index
                            page.selectedFile = Qt.resolvedUrl(absolutePath).toString()
                        }
                    }
                    contentHeight: content.height
                    width: parent.width
                    Row { id: content
                        height: icon.height
                        width: parent.width
                        Icon { id: icon
                            source: Theme.iconForMimeType(mimeType)
                            height: Theme.iconSizeMedium
                            width: height
                            sourceSize.width: height
                            sourceSize.height: height
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Loader {
                            anchors.fill: icon
                            anchors.centerIn: icon
                            active: (/^image/.test(mimeType) ||  /^video/.test(mimeType)
                            sourceComponent: Thumbnail {
                                sourceSize.width: width
                                sourceSize.height: height
                                source: absolutePath
                                fillMode: Thumbnail.PreserveAspectFit
                            }
                        }
                        Column {
                            width: parent.width - icon.width
                            anchors.verticalCenter: icon.verticalCenter
                            Label { text: fileName;  truncationMode: TruncationMode.Fade; font.pixelSize: Theme.fontSizeSmall; color: Theme.primaryColor; font.bold: litem.selected }
                            Label { text: mimeType;  truncationMode: TruncationMode.Fade; font.pixelSize: Theme.fontSizeTiny; color: Theme.secondaryColor }
                        }
                    }
                }
            }

            Item { id: buttons
                width: parent.width
                height: Math.max(cancelButton.implicitHeight, confirmButton.implicitHeight)

                SystemDialogTextButton {
                    id: cancelButton
                    width: page.width / 2
                    //
                    //% "Cancel"
                    text: qsTrId("components-he-dialog_cancel")

                    onClicked: {
                        page.done(false, null)
                        page.lower()
                    }
                }

                SystemDialogTextButton {
                    id: confirmButton
                    anchors.right: parent.right
                    width: page.width / 2

                    enabled: (page.selectedFile.length > 0)

                    //% "Accept"
                    text: qsTrId("components-he-dialog_accept")

                    onClicked: {
                        page.done(true, page.selectedFile)
                        page.lower()
                    }
                }
            }
        }
    }
}
