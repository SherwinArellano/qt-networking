import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: app
    width: 1200
    height: 800
    visible: true
    title: qsTr("Networking")

    property var data: null
    property string errMessage: ""

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: "Networking"
            font.pointSize: 24
        }

        Button {
            text: "Load objects"
            onClicked: api.getObjects()
        }

        Label {
            text: "Result"
            font.pointSize: 24
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Label {
                property string output: JSON.stringify(app.data, null, 2)
                text: {
                    if (app.errMessage) return errMessage
                    if (app.data) return output
                    return "No response yet."
                }
            }
        }
    }

    Connections {
        target: api

        function onObjectsReady(data) {
            if (data.length !== undefined) console.log("Received array with length:", data.length)
            else console.log("Received object")

            app.errMessage = ""
            app.data = data
        }

        function onNetworkError(message, httpStatus) {
            app.errMessage = `HTTP error: ${httpStatus} ${message}`
            app.data = null
        }
    }
}
