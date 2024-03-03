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
            openFilePicker(handle, "{}")
        }
        function openFilePicker(handle, options) {
            console.log("Was asked for a file open dialog, using options", options)
            var dialogOptions = {}
            try {
                dialogOptions = JSON.parse(options)
            } catch (e) {
                console.log("Parsing options failed:", e)
            }
            if (!_filePickerDialog) {
                var comp = Qt.createComponent(Qt.resolvedUrl("FilePickerDialog.qml"))
                if (comp.status == Component.Error) {
                    console.log("FilePickerDialog.qml error:", comp.errorString())
                    responseInterface.response(2, null) // code 2 is "other" on org.freedesktop.portal.Request::Response
                    return
                }
                _filePickerDialog = comp.createObject(root, { "options": dialogOptions } )
                console.log("FilePickerDialog.qml created.")

                _filePickerDialog.done.connect(function(result, data) {
                    console.log("FilePickerDialog done:", result, data)
                    var rif = responseInterface.createObject(root, { "path": handle })
                    rif.emitSignal("Response", { "response": result ? 0 : 1, "results": [ data ] })
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

    Component {
        id: responseInterface
        DBusAdaptor {
            service: "org.freedesktop.portal.desktop"
            iface: "org.freedesktop.portal.Request"
        }
    }
}
