/*
* SPDX-License-Identifier: Apache-2.0
*
* SPDX-FileCopyrightText: 2024~ Sailfish OS Community
* Authors:
*   Peter G. <sailfish@nephros.org>
*
*/
import QtQuick 2.2
import QtQuick.Window 2.1 as QtQuick
import Sailfish.Silica 1.0
import Sailfish.FileManager 1.0
import Sailfish.Pickers 1.0
import Sailfish.Lipstick 1.0
import Nemo.DBus 2.0

ApplicationWindow { id: root

    cover: null
    property FilePickerDialog _filePickerDialog
    DBusAdaptor {
        service: "org.freedesktop.impl.portal.desktop.amber.ui"
        iface: "org.freedesktop.impl.portal.desktop.amber.ui"
        path: "/org/freedesktop/impl/portal/desktop/amber/ui"

        function openFilePicker(handle) {
            if (!_filePickerDialog) {
                var comp = Qt.createComponent(Qt.resolvedUrl("FilePickerDialog.qml"))
                if (comp.status == Component.Error) {
                    console.log("FilePickerDialog.qml error:", comp.errorString())
                    responseInterface.response(1, null)
                    return
                }
                _filePickerDialog = comp.createObject(root)
                console.log("FilePickerDialog.qml created.")

                _filePickerDialog.done.connect(function(data) {
                    responseInterface.path=handle
                    responseInterface.response(!_filePickerDialog.dismissed, data)
                })
                console.log("Activating.")
                root._finishPicker()
            }
            _filePickerDialog.init()
        }
    }

    function _finishPicker() {
        if (_filePickerDialog) {
            if (_filePickerDialog.windowVisible) {
                _filePickerDialog.lower()
            }
            return true
        }
        return false
    }

    DBusInterface {
        id: responseInterface
        service: "org.freedesktop.portal.desktop"
        iface: "org.freedesktop.portal.Request"
        signal response(int response, var results)
    }
}
