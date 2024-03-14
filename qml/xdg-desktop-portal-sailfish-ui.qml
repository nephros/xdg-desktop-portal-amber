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
import Sailfish.Pickers 1.0
import Sailfish.Lipstick 1.0
import Nemo.DBus 2.0

/*! \qmlmodule org.freedesktop.impl.portal.desktop.amber.ui
    \brief XDG Desktop Portal Amber QML UI
*/

/*! \qmltype xdg-desktop-portal-amber-ui
    \brief XDG Desktop UI Manager
    \ingroup uitypes
    \inqmlmodule org.freedesktop.impl.portal.desktop.amber.ui

    This application manages the display of various UI dialogs for XDG Desktop Portal Amber.

    It can be activated using the D-Bus session bus at:

    \list
    \li Service:     \c org.freedesktop.impl.portal.desktop.amber.ui
    \li Path:        \c /org/freedesktop/impl/portal/desktop/amber/ui
    \li Interface:   \c org.freedesktop.impl.portal.desktop.amber.ui
    \endlist
*/
ApplicationWindow { id: root

    cover: null
    /*! \qmlproperty ConfirmationDialog xdg-desktop-portal-amber-ui::_confirmationDialog
       Holds the instance of the picker dialog launched.
       \sa ConfirmationDialog
       \internal
    */
    property ConfirmationDialog _confirmationDialog
    /*! \qmlproperty FilePickerDialog xdg-desktop-portal-amber-ui::_filePickerDialog
       Holds the instance of the picker dialog launched.
       \sa FilePickerDialog
       \internal
    */
    property FilePickerDialog _filePickerDialog

    /*! \qmlmethod iconHintToUrl(hint)
        Maps XDP icon hints to an url of an available icon from Silica.
    */
    function iconHintToUrl(hint) {
        var url="image://theme/icon-m-question"
        switch (hint) {
            case "applets-screenshooter-symbolic": url="image://theme/icon-m-share-gallery"; break
            case "audio-input-microphone-symbolic": url="image://theme/icon-m-mic"; break
            case "audio-speakers-symbolic": url="image://theme/icon-m-speaker"; break
            case "camera-web-symbolic": url="image://theme/icon-m-camera"; break
            case "find-location-symbolic": url="image://theme/icon-m-location"; break
            case "preferences-desktop-wallpaper-symbolic": url="image://theme/icon-m-ambience"; break
        }
        return url;
    }
    /*! \qmlproperty DBusInterface xdg-desktop-portal-amber-ui::_request
        Holds the instance of the picker dialog launched.
        \sa Nemo::DBus
        \internal
    */
    property DBusInterface _request
    DBusAdaptor {
        service: "org.freedesktop.impl.portal.desktop.sailfish.ui"
        iface: "org.freedesktop.impl.portal.desktop.sailfish.ui"
        path: "/org/freedesktop/impl/portal/desktop/sailfish/ui"
        xml: [
             '<interface name="org.freedesktop.impl.portal.desktop.sailfish.ui">',
             '<method name="confirmationDialog">',
             '   <arg type="s" name="handle" direction="in"/>',
             '   <arg type="s" name="title" direction="in"/>',
             '   <arg type="s" name="subtitle" direction="in"/>',
             '   <arg type="s" name="body" direction="in"/>',
             '   <arg type="s" name="options" direction="in"/>',
             '</method>',
             '<method name="openFilePicker">',
             '   <arg type="s" name="handle" direction="in"/>',
             '   <arg type="s" name="title" direction="in"/>',
             '   <arg type="s" name="options" direction="in"/>',
             '</method>',
             '<signal name="confirmationDone">',
             '    <arg type="i" name="response" direction="out"/>',
             '    <annotation name="org.qtproject.QtDBus.QtTypeName.Out0" value="uint"/>',
             '  </signal>',
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

        function confirmationDialog(handle, title, subtitle, body, options) {
            console.log("Was asked for a confirmation dialog, using options", options)
            var dialogOptions = {}
            var dialogInfo = {}
            try {
                dialogOptions = JSON.parse(options)
            } catch (e) {
                console.log("Parsing options failed:", e)
            }
            var dialogInfo = {
                "title": "",
                "subtitle": "",
                "body": "",
                "acceptLabel": "",
                "denyLabel": "",
                "iconHint": ""
            }
            dialogInfo.title     = title
            dialogInfo.subtitle  = subtitle ? subtitle : "" 
            dialogInfo.body      = body     ? body : "" 

            dialogInfo.acceptLabel   = dialogOptions.grant_label ? dialogOptions.grant_label : "" 
            dialogInfo.denyLabel     = dialogOptions.deny_label  ? dialogOptions.deny_label : "" 
            dialogInfo.iconHint      = root.iconHintToUrl(dialogOptions.icon)

            // create dialog
            if (!_confirmationDialog) {
                var comp = Qt.createComponent(Qt.resolvedUrl("ConfirmationDialog.qml"))
                if (comp.status == Component.Error) {
                    console.log("ConfirmationDialog.qml error:", comp.errorString())
                    emitSignal("confirmationDone", [ 2 ]) // code 2 is "other" on org.freedesktop.portal.Request::Response
                    _request.destroy()
                    return
                }
                _confirmationDialog = comp.createObject(root, dialogInfo )
                console.log("ConfirmationDialog.qml created.")
                _confirmationDialog.done.connect(function(result) {
                    console.log("ConfirmationDialog done:", result)
                    emitSignal("confirmationDone", [ result ])
                    _confirmationDialog.destroy()
                    //_request.destroy()
                })
            }
            console.log("Activating.")
            root._finishPicker()
        }

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

    /*! Finishes dynamic creation of Picker
       \internal
    */
    function _finishPicker() {
        if (_filePickerDialog) {
            if (_filePickerDialog.windowVisible) {
                _filePickerDialog.lower()
            }
            return true
        }
        return false
    }

    /*! Finishes dynamic creation of Confirmation
       \internal
    */
    function _finishDialog() {
        if (_confirmationDialog) {
            if (_confirmationDialog.windowVisible) {
                _confirmationDialog.lower()
            }
            return true
        }
        return false
    }

    Component { id: requestInterface
        DBusInterface {
            service: "org.freedesktop.impl.portal.desktop.sailfish"
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
