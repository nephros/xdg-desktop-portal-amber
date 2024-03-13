/*
* SPDX-License-Identifier: Apache-2.0
*
* SPDX-FileCopyrightText: 2024~ Sailfish OS Community
* Authors:
*   Peter G. <sailfish@nephros.org>
*
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0

/*! \qmltype ConfirmationDialog
    \brief XDG Desktop Access UI
    \ingroup uitypes
*/

SystemDialog { id: dialog

    /*! \qmlsignal ConfirmationDialog::done(int result) */
    signal done(int result)

    /*! \qmlproperty var ConfirmationDialog::dialogInfo
        \internal
        Configures the content of the dialog
    */
    property var dialogInfo: {
        "name": "",
        "subtitle": "",
        "body": "",
        "acceptLabel": "",
        "denyLabel": "",
        "iconHint": ""
    }
    /*! \qmlproperty var ConfirmationDialog::options
     *
        \sa Amber::FileChooserPortal, https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.impl.portal.FileChooser.html
    */
    property var options

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
                dialog.done(2)
            }
        }
    }

    title: dialogInfo.name
    contentHeight: contentColumn.height

    Column {
        id: contentColumn
        width: parent.width

        SystemDialogHeader {
            id: header

            title: dialogInfo.name
            // TODO
            //description: dialogInfo.appname
        }

        Label {
            width: header.width
            anchors.horizontalCenter: parent.horizontalCenter
            visible: text != ""
            color: Theme.highlightColor
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: Theme.fontSizeSmall
            text: dialogInfo.subtitle
        }
        Label {
            width: header.width
            anchors.horizontalCenter: parent.horizontalCenter
            visible: text != ""
            color: Theme.highlightColor
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: Theme.fontSizeSmall
            text: dialogInfo.body
        }
        SystemDialogTextButton {
            id: cancelButton
            width: dialog.width / 2
            //
            //% "Cancel"
            text: dialogInfo.denyLabel ? dialogInfo.denyLabel : qsTrId("components-he-dialog_cancel")

            onClicked: {
                dialog.done(1)
                dialog.lower()
            }
        }

        SystemDialogTextButton {
            id: confirmButton
            anchors.right: parent.right
            width: dialog.width / 2

            //% "Accept"
            text: dialogInfo.acceptLabel ? dialogInfo.acceptLabel : qsTrId("components-he-dialog_accept")

            onClicked: {
                dialog.done(0)
                dialog.lower()
            }
        }

    }
}
