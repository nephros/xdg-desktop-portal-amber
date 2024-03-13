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

SystemDialog { id: dialog

    signal done(int result)

    property var dialogInfo: ({
        "name": "",
        "subtitle": "",
        "body": "",
        "acceptLabel": "",
        "denyLabel": "",
        "iconHint": ""
    })

    property int result: 2 // neither confirm nor deny
    title: dialogInfo.name
    contentHeight: contentColumn.height

    Column {
        id: contentColumn
        width: parent.width

        SystemDialogHeader {
            id: header

            title: dialogInfo.name
            description: dialogInfo.appname
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
            width: page.width / 2
            //
            //% "Cancel"
            text: qsTrId("components-he-dialog_cancel")

            onClicked: {
                page.done(1)
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
                page.done(0)
                page.lower()
            }
        }

    }
}
