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
    property DBusInterface _request
    DBusAdaptor {
        service: "org.freedesktop.impl.portal.desktop.amber.ui"
        iface: "org.freedesktop.impl.portal.desktop.amber.ui"
        path: "/org/freedesktop/impl/portal/desktop/amber/ui"
        xml: [
             '<interface name="org.freedesktop.impl.portal.desktop.amber.ui">',
             '<method name="openFilePicker">',
             '   <arg type="s" name="handle" direction="in"/>',
             '   <arg type="s" name="title" direction="in"/>',
             '   <arg type="s" name="options" direction="in"/>',
             '</method>',
             '<signal name="pickerDone">',
             '    <arg type="i" name="response" direction="out"/>',
             '    <annotation name="org.qtproject.QtDBus.QtTypeName.Out0" value="uint"/>',
             //'    <arg type="s" name="results" direction="out"/>',
             //'    <annotation name="org.qtproject.QtDBus.QtTypeName.Out1" value="QString"/>',
             //'    <arg type="a{sv}" name="results"/>',
             //'    <annotation name="org.qtproject.QtDBus.QtTypeName.Out1" value="QVariantMap"/>',
             '    <arg type="av" name="results"/>',
             '    <annotation name="org.qtproject.QtDBus.QtTypeName.Out1" value="QVariantList"/>',
             '  </signal>',
             '</interface>',
        ].join('\n')

        function openFilePicker(handle, title, options) {
            console.log("Was asked for a file open dialog, using options", options)
            // Create a dbus interface to listen for messages on the handle object:
            _request = requestInterface.createObject(root, { "path": handle })
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
                    emitSignal("pickerDone", [ 2, [ "" ] ]) // code 2 is "other" on org.freedesktop.portal.Request::Response
                    _request.destroy()
                    return
                }
                _filePickerDialog = comp.createObject(root, { "title": title, "options": dialogOptions } )
                console.log("FilePickerDialog.qml created.")

                /* Example for a correct response:
                   signal time=1709492498.046692 sender=:1.5 -> destination=:1.618 serial=8010 path=/org/freedesktop/portal/desktop/request/1_618/qt3378739430; interface=org.freedesktop.portal.Request; member=Response
                      uint32 0
                      array [
                         dict entry(
                            string "current_filter"
                            variant struct {
                              string "All Files "
                              array [
                                 struct {
                                    uint32 0
                                    string "*"
                                 }
                              ]
                            }
                         )
                         dict entry(
                            string "uris"
                            variant array [
                              string "file:///home/piggz/gts2_auth.txt"
                            ]
                         )
                      ]
                */
                    _filePickerDialog.done.connect(function(result, data) {
                    console.log("FilePickerDialog done:", result, data)

                    // construct the response payload:
                        // [
                        //      uris (as)
                        //      choices (a(ss))
                        //      current_filter ((sa(us)))
                        // ]
                    // for now, we only support a single file uri and no filters:
                    const uris = []
                    if ( typeof data === "string") { uris = [ data ] }
                    if ( typeof data === "object") { data.forEach(function(p) { uris.push(p) }) }
                    const code = result ? 0 : 1

                    emitSignal("pickerDone", [ code, uris ] )
                    _filePickerDialog.destroy()
                    _request.destroy()
                }) // end function(result,data)

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

    Component { id: requestInterface
        DBusInterface {
            service: "org.freedesktop.impl.portal.desktop.amber"
            iface: "org.freedesktop.impl.portal.Request"
            signalsEnabled: true
            function close() {
                console.log("Received Close request.")
                if (_filePickerDialog.windowVisible) {
                    _filePickerDialog.lower()
                }
                _filePickerDialog.destroy()
            }
        }
    }
}
