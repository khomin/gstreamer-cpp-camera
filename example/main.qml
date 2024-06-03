import QtQuick 2.15
import QtQuick.Window 2.15
import "."
import ImageAdapter 1.0

Window {
    id: window
    width: 1000;
    height: 800;
    visible: true
    title: qsTr("Hello World")
    color: "black"

    Column {
        Text {
            text: control.bitrate
            color: "orange"
            padding: 14
        }
        Row {
            LiveImage {
                width: window.width / 2
                height: window.height
                image: provider1.image
            }
            LiveImage {
                width: window.width / 2
                height: window.height
                image: provider2.image
            }
        }
    }
}
