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

/*! \qmltype FilePickerDialog
    \brief XDG Desktop File Picker
    \ingroup uitypes
*/

SystemDialog {
    id: page

    /*! The dialog title. */
    property string title
    /*! \qmlproperty var FilePickerDialog::options
     *
        \sa Amber::FileChooserPortal, https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.impl.portal.FileChooser.html

        \list
        \li accept_label (s)
        The label for the accept button. Mnemonic underlines are allowed.

        \li modal (b)
        Whether to make the dialog modal. Default is yes.

        \li multiple (b)
        Whether to allow selection of multiple files. Default is no.

        \li directory (b)
        Whether to select for folders instead of files. Default is to select files.

        \li filters (a(sa(us)))
        A list of serialized file filters. See org.freedesktop.portal.FileChooser.OpenFile for details.

        \li current_filter ((sa(us)))
        Request that this filter be set by default at dialog creation. See org.freedesktop.portal.FileChooser.OpenFile for details.

        \li choices (a(ssa(ss)s))
        A list of serialized combo boxes. See org.freedesktop.portal.FileChooser.OpenFile for details.

        \li current_folder (ay)
        A suggested folder to open the files from. See org.freedesktop.portal.FileChooser.OpenFile for details.
        \endlist
    */
    property var options
    /*! The picked file name will be held here after selection by the user */
    property string selectedFile
    /*! \c true if the dialog should allow directories as result. */
    property bool wantDirectory: ( !!options && (options.directory == true))
    /*! \c true if the dialog should allow selecting multiple files/directories. */
    property bool wantMulti: ( !!options && (options.multiple == true))
    /*! \c true if the dialog is currently shown.
        \internal
    */
    property bool windowVisible: visibility != Window.Hidden
                                 && visibility != Window.Minimized
    /*! Raises/shows the widow when called.
       \internal
    */
    function init() {
        raise()
        show()
        autoCloseTimer.restart()
    }

    Timer {
        id: autoCloseTimer
        interval: 115000
        onTriggered: {
            console.log("Dialog shown too long without response, closing")
            if (windowVisible) {
                lower()
                page.done(2, null)
            }
        }
    }

    /*! \qmlsignal FilePickerDialog::done(bool accepted, string selectedFile)
     */
    signal done(bool accepted, string selectedFile)
    /*! \qmlproperty FileModel FilePickerDialog::model

        The model representing the contents of the file system
        \sa Nemo::FileManager
     */
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
                            // navigate
                            page.model.path = absolutePath
                        } else {
                            // do nothing for files if we want dirs
                            if (page.wantDirectory) return
                            // select
                            view.currentIndex = index
                            page.selectedFile = Qt.resolvedUrl(absolutePath).toString()
                        }
                    }
                    menu: ContextMenu {
                        enabled: (wantDirectory && isDir)
                        MenuItem {
                            //% "Select"
                            text: qsTrId("components-he-dialog_select")
                            onClicked: page.selectedFile = Qt.resolvedUrl(absolutePath).toString()
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
                            Loader {
                                anchors.fill: icon
                                anchors.centerIn: icon
                                active: /^image/.test(mimeType) ||  /^video/.test(mimeType)
                                sourceComponent: Thumbnail {
                                    sourceSize.width: width
                                    sourceSize.height: height
                                    source: absolutePath
                                    fillMode: Thumbnail.PreserveAspectFit
                                }
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
