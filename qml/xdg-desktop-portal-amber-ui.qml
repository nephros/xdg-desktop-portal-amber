import QtQuick 2.2
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import Nemo.DBus 2.0

SystemDialog {
    id: page

    DBusAdaptor {
        service: "org.freedesktop.impl.portal.desktop.amber.ui"
        iface: "org.freedesktop.impl.portal.desktop.amber.ui"
        path: "/org/freedesktop/impl/portal/desktop/amber/ui"

        function openFilePicker(handle) {
            var picker = pageStack.animatorPush(Qt.resolvedUrl("FilePickerDialog.qml"))
            picker.done.connect(function(data) {
               responseInterface.path=handle
               responseInterface.response(picker.accepted, data)
            })
            //app.activate()
            window.activate()
        }
    }

    DBusInterface {
        id: responseInterface
        service: "org.freedesktop.portal.desktop"
        iface: "org.freedesktop.portal.Request"
        signal response(int response, var results)
    }

}
