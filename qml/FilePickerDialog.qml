import QtQuick 2.2
import Sailfish.Silica 1.0
import Sailfish.Lipstick 1.0
import Sailfish.Pickers 1.0

SystemDialog {
    id: page

    property string selectedFile

    ValueButton {
        anchors.centerIn: parent
        label: "File"
        value: selectedFile ? selectedFile : "None"
        onClicked: pageStack.push(filePickerPage)
    }

    Component {
        id: filePickerPage
        FilePickerPage {
            onSelectedContentPropertiesChanged: {
                page.selectedFile = selectedContentProperties.filePath
            }
        }
    }
}
