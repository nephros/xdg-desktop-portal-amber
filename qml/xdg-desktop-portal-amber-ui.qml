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
        xml: [
             '<interface name="org.freedesktop.impl.portal.desktop.amber.ui">',
             '<method name="openFilePicker">',
             '   <arg type="s" name="handle" direction="in"/>',
             '   <arg type="s" name="title" direction="in"/>',
             '   <arg type="s" name="options" direction="in"/>',
             '</method>',
             '</interface>',
        ].join('\n')

        function openFilePicker(handle, title, options) {
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
                    var resp = responseInterface.createObject(root, { "path": handle })
                    resp.emitSignal("Response", { "response": 2, "results": [ ] }) // code 2 is "other" on org.freedesktop.portal.Request::Response
                    resp.destroy();
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
                    var asv = []
                    asv.push( { "current_filter": [ "All Files ", [ { 0: "*" } ] ] })
                    const uris = []
                    if ( typeof data === "string") { uris.push( data ) }
                    if ( typeof data === "object") { data.forEach(function(p) { uris.push(p) }) }
                    asv.push( { "uris": uris } )

                    const code = result ? 0 : 1

                    const payload = [ code, asv ]
                    // Important: this is read by the calling process:
                    console.log("### Results:\n", JSON.stringify(payload,null,null))

                    var resp = responseInterface.createObject(root, { "path": handle })
                    resp.emitSignal("Response", payload)
                    _filePickerDialog.destroy()
                    resp.destroy();
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

    Component {
        id: responseInterface
        DBusAdaptor {
            //service: "org.freedesktop.portal.Desktop"
            //iface: "org.freedesktop.portal.Request"
            service: "org.freedesktop.impl.portal.FileChooser"
            iface: "org.freedesktop.impl.portal.Request"
            Component.onCompleted: console.log("Created", service, iface, path)
            xml: [
                '<interface name="org.freedesktop.impl.portal.Request">',
                '    <method name="Close"></method>',
                '</interface>'
            ].join("\n")
        }
    }
}
